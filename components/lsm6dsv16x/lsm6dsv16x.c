#include <string.h>
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

/* 引入 ST 官方驱动头文件 */
#include "lsm6dsv16x_reg.h"

/* ================= 用户配置区 ================= */
static const char *TAG = "LSM6DSV16X";

/* I2C 引脚配置 */
#define I2C_MASTER_SCL_IO           25      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           26      /*!< GPIO number used for I2C master data */
#define I2C_MASTER_NUM              0       /*!< I2C master i2c port number */
#define I2C_MASTER_FREQ_HZ          400000  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/* 中断引脚配置 */
#define LSM_INT_PIN                 27      /*!< GPIO connected to INT1/INT2 */

/* 设备地址 */
/* 注意：ESP32 I2C 驱动使用 7位地址，不需要像 STM32 那样左移一位 */
#define LSM6DSV16X_I2C_ADD          0x6A

#define BOOT_TIME           10
#define RAD_TO_DEG          (180.0f / 3.14159265358979323846f)

/* ================= 全局变量 ================= */
volatile uint8_t mems_event_detected = 0;
static uint8_t whoamI;
static lsm6dsv16x_fifo_sflp_raw_t fifo_sflp;

lsm6dsv16x_fifo_status_t fifo_status;
stmdev_ctx_t dev_ctx;
lsm6dsv16x_reset_t rst;
lsm6dsv16x_pin_int_route_t pin_int;

float_t quat[4];
int16_t gyro[3], accel[3];
float pitch, roll, yaw;

/* ================= 内部函数声明 ================= */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

static void platform_delay(uint32_t ms);

/* ================= 数学辅助函数 (保持原样) ================= */

static void quat_to_euler(const float q[4], float *roll, float *pitch, float *yaw) {
    float x = q[0], y = q[1], z = q[2], w = q[3];

    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2.0f * (w * y - z * x);
    if (fabsf(sinp) >= 1.0f)
        *pitch = copysignf(M_PI / 2.0f, sinp);
    else
        *pitch = asinf(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    *yaw = atan2f(siny_cosp, cosy_cosp);

    *roll *= RAD_TO_DEG;
    *pitch *= RAD_TO_DEG;
    *yaw *= RAD_TO_DEG;
}

static float_t npy_half_to_float(uint16_t h) {
    union {
        float_t ret;
        uint32_t retbits;
    } conv;
    conv.retbits = lsm6dsv16x_from_f16_to_f32(h);
    return conv.ret;
}

static void sflp2q(float_t quat[4], uint8_t raw[6]) {
    float_t sumsq = 0;
    uint16_t sflp[3];

    memcpy(&sflp[0], &raw[0], 2);
    memcpy(&sflp[1], &raw[2], 2);
    memcpy(&sflp[2], &raw[4], 2);

    quat[0] = npy_half_to_float(sflp[0]);
    quat[1] = npy_half_to_float(sflp[1]);
    quat[2] = npy_half_to_float(sflp[2]);

    for (uint8_t i = 0; i < 3; i++)
        sumsq += quat[i] * quat[i];

    if (sumsq < 1e-6f) {
        quat[0] = 1.0f;
        quat[1] = 0.0f;
        quat[2] = 0.0f;
        quat[3] = 0.0f;
        return;
    }

    if (sumsq > 1.0f) {
        float_t n = sqrtf(sumsq);
        quat[0] /= n;
        quat[1] /= n;
        quat[2] /= n;
        sumsq = 1.0f;
    }

    float_t one_minus_sumsq = 1.0f - sumsq;
    if (one_minus_sumsq < 0.0f) one_minus_sumsq = 0.0f;

    quat[3] = sqrtf(one_minus_sumsq);
}

/* ================= 平台适配层 (Platform Dependent) ================= */

/**
 * @brief I2C 写函数适配 ESP32
 * STM32 HAL_I2C_Mem_Write 自动发送 寄存器地址 + 数据
 * ESP32 需要手动将 寄存器地址 和 数据 拼接到同一个 Buffer 中发送
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
    esp_err_t ret;
    uint8_t *data_wr = NULL;

    // 分配内存：寄存器地址(1 byte) + 数据长度
    data_wr = (uint8_t *) malloc(len + 1);
    if (data_wr == NULL) return -1;

    data_wr[0] = reg;
    memcpy(&data_wr[1], bufp, len);

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, LSM6DSV16X_I2C_ADD, data_wr, len + 1,
                                     I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    free(data_wr);

    return (ret == ESP_OK) ? 0 : -1;
}

/**
 * @brief I2C 读函数适配 ESP32
 * 使用 i2c_master_write_read_device 实现先写寄存器地址，再 Restart 读取数据
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, LSM6DSV16X_I2C_ADD, &reg, 1, bufp, len,
                                       I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (ret == ESP_OK) ? 0 : -1;
}

static void platform_delay(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* GPIO 中断处理 ISR */
static void IRAM_ATTR lsm6dsv16x_isr_handler(void *arg) {
    mems_event_detected = 1;
    /* 如果使用 FreeRTOS 任务通知，可以在这里 xTaskNotifyFromISR */
}

/* 初始化 I2C 总线 */
esp_err_t LSM_Init_I2C_Bus(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .scl_io_num = I2C_MASTER_SCL_IO,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/* 初始化 GPIO 中断 */
void lsm6dsv16x_gpio_init(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE; // 上升沿触发，根据 DRDY_PULSED 模式调整
    io_conf.pin_bit_mask = (1ULL << LSM_INT_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0; // 传感器通常是推挽输出，如果开漏需要上拉
    gpio_config(&io_conf);
//    esp_err_t err = gpio_install_isr_service(0);
//    if (err == ESP_OK) {
//        ESP_LOGI(TAG, "GPIO ISR Service installed successfully.");
//    } else if (err == ESP_ERR_INVALID_STATE) {
//        ESP_LOGW(TAG, "GPIO ISR Service already installed, skipping...");
//    } else {
//        ESP_LOGE(TAG, "Failed to install ISR service: %s", esp_err_to_name(err));
//        return;
//    }
    gpio_isr_handler_add(LSM_INT_PIN, lsm6dsv16x_isr_handler, (void *) LSM_INT_PIN);
}

/* ================= 主逻辑函数 ================= */

uint8_t LSM6DSV16X_Init(void) {
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = NULL; // ESP32 驱动直接使用 I2C 端口号宏，不需要句柄指针

    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);

    /* Check device ID */
    if (lsm6dsv16x_device_id_get(&dev_ctx, &whoamI) != 0) {
        ESP_LOGE(TAG, "I2C Communication Error");
        return 1;
    }

    ESP_LOGI(TAG, "Device ID: 0x%02X", whoamI);
    if (whoamI != LSM6DSV16X_ID) {
        ESP_LOGE(TAG, "Device ID Mismatch");
        return 2;
    }

    /* Restore default configuration */
    lsm6dsv16x_reset_set(&dev_ctx, LSM6DSV16X_GLOBAL_RST);
    do {
        lsm6dsv16x_reset_get(&dev_ctx, &rst);
    } while (rst != LSM6DSV16X_READY);

    /* Enable Block Data Update */
    lsm6dsv16x_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    /* Set full scale */
    lsm6dsv16x_xl_full_scale_set(&dev_ctx, LSM6DSV16X_4g);
    lsm6dsv16x_gy_full_scale_set(&dev_ctx, LSM6DSV16X_2000dps);

    /* Set FIFO batch of sflp data */
    fifo_sflp.game_rotation = 1;
    fifo_sflp.gravity = 0;
    fifo_sflp.gbias = 0;
    lsm6dsv16x_fifo_sflp_batch_set(&dev_ctx, fifo_sflp);

    lsm6dsv16x_fifo_xl_batch_set(&dev_ctx, LSM6DSV16X_XL_BATCHED_AT_60Hz);
    lsm6dsv16x_fifo_gy_batch_set(&dev_ctx, LSM6DSV16X_GY_BATCHED_AT_60Hz);

    /* Set FIFO mode to Stream mode */
    lsm6dsv16x_fifo_mode_set(&dev_ctx, LSM6DSV16X_STREAM_MODE);

    /* Set Output Data Rate */
    lsm6dsv16x_xl_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_60Hz);
    lsm6dsv16x_gy_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_60Hz);
    lsm6dsv16x_sflp_data_rate_set(&dev_ctx, LSM6DSV16X_SFLP_60Hz);
    lsm6dsv16x_sflp_game_rotation_set(&dev_ctx, PROPERTY_ENABLE);

    /* Interrupt configuration */
    pin_int.drdy_xl = PROPERTY_ENABLE;
    lsm6dsv16x_pin_int2_route_set(&dev_ctx, &pin_int); // 假设连接的是 INT2，如果是 INT1 请用 int1_route
    lsm6dsv16x_data_ready_mode_set(&dev_ctx, LSM6DSV16X_DRDY_PULSED);

    return 0; // Success
}

void Read_LSM6DSV16X(void) {
    uint16_t num;
    int16_t datax, datay, dataz;

    /* Read watermark flag */
    if (lsm6dsv16x_fifo_status_get(&dev_ctx, &fifo_status) != 0) return;

    num = fifo_status.fifo_level;

    // 增加一个保护，避免一次读太多卡死
    if (num > 100) num = 100;

    while (num--) {
        lsm6dsv16x_fifo_out_raw_t f_data;

        /* Read FIFO sensor value */
        if (lsm6dsv16x_fifo_out_raw_get(&dev_ctx, &f_data) != 0) break;

        memcpy(&datax, &f_data.data[0], 2);
        memcpy(&datay, &f_data.data[2], 2);
        memcpy(&dataz, &f_data.data[4], 2);

        switch (f_data.tag) {
            case LSM6DSV16X_SFLP_GAME_ROTATION_VECTOR_TAG:
                sflp2q(quat, &f_data.data[0]);
                quat_to_euler(quat, &roll, &pitch, &yaw);
                /* 打印输出以便调试 */
                // ESP_LOGI(TAG, "Euler: P:%.2f R:%.2f Y:%.2f", pitch, roll, yaw);
                break;
            case LSM6DSV16X_XL_NC_TAG:
                accel[0] = (short) lsm6dsv16x_from_fs4_to_mg(datax);
                accel[1] = (short) lsm6dsv16x_from_fs4_to_mg(datay);
                accel[2] = (short) lsm6dsv16x_from_fs4_to_mg(dataz);
                break;
            case LSM6DSV16X_GY_NC_TAG:
                gyro[0] = (short) (lsm6dsv16x_from_fs2000_to_mdps(datax) / 1000);
                gyro[1] = (short) (lsm6dsv16x_from_fs2000_to_mdps(datay) / 1000);
                gyro[2] = (short) (lsm6dsv16x_from_fs2000_to_mdps(dataz) / 1000);
                break;
            default:
                break;
        }
    }
}
