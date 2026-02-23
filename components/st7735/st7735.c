#include "st7735.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static spi_device_handle_t spi_dev;

// --- 手动控制 CS 引脚的移植 ---

static void ST7735_Select() {
    gpio_set_level(ST7735_PIN_CS, 0); // 对应 STM32 的 RESET
}

void ST7735_Unselect() {
    gpio_set_level(ST7735_PIN_CS, 1); // 对应 STM32 的 SET
}

// --- 通信底层修改 ---

static void ST7735_WriteCommand(uint8_t cmd) {
    ST7735_Select();                  // 手动选中
    gpio_set_level(ST7735_PIN_DC, 0); // DC拉低表示命令
    spi_transaction_t t = {
            .length = 8,
            .tx_buffer = &cmd,
    };
    spi_device_polling_transmit(spi_dev, &t);
    ST7735_Unselect();                // 手动释放
}

static void ST7735_WriteData(const uint8_t *buff, size_t buff_size) {
    if (buff_size == 0) return;
    ST7735_Select();                  // 手动选中
    gpio_set_level(ST7735_PIN_DC, 1); // DC拉高表示数据
    spi_transaction_t t = {
            .length = buff_size * 8,
            .tx_buffer = buff,
    };
    spi_device_polling_transmit(spi_dev, &t);
    ST7735_Unselect();                // 手动释放
}

static void ST7735_Reset() {
    gpio_set_level(ST7735_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(ST7735_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void ST7735_Init() {
    // 1. GPIO 配置：必须包含 CS 引脚
    gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << ST7735_PIN_DC) | (1ULL << ST7735_PIN_RST) | (1ULL << ST7735_PIN_CS),
            .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    ST7735_Unselect(); // 初始状态不选中

    // 2. SPI 总线初始化
    spi_bus_config_t buscfg = {
            .mosi_io_num = ST7735_PIN_MOSI,
            .miso_io_num = -1,
            .sclk_io_num = ST7735_PIN_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = ST7735_WIDTH * ST7735_HEIGHT * 2
    };
    spi_bus_initialize(ST7735_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

    // 3. SPI 设备配置：spics_io_num 设为 -1 表示手动控制
    spi_device_interface_config_t devcfg = {
            .clock_speed_hz = 40 * 1000 * 1000,
            .mode = 0,
            .spics_io_num = -1, // <--- 关键修改：手动控制 CS 引脚
            .queue_size = 7,
            .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY,
    };
    spi_bus_add_device(ST7735_SPI_HOST, &devcfg, &spi_dev);

    // 4. 初始化序列
    ST7735_Reset();

    ST7735_WriteCommand(ST7735_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    ST7735_WriteCommand(ST7735_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(255));

    ST7735_WriteCommand(ST7735_MADCTL);
    uint8_t madctl = ST7735_ROTATION;
    ST7735_WriteData(&madctl, 1);

    ST7735_WriteCommand(ST7735_COLMOD);
    uint8_t colmod = 0x05;
    ST7735_WriteData(&colmod, 1);

    ST7735_WriteCommand(ST7735_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
}

// --- 绘图函数部分 ---

static void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data_x[] = {0x00, x0, 0x00, x1};
    ST7735_WriteData(data_x, sizeof(data_x));

    ST7735_WriteCommand(ST7735_RASET);
    uint8_t data_y[] = {0x00, y0, 0x00, y1};
    ST7735_WriteData(data_y, sizeof(data_y));

    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    ST7735_SetAddressWindow(x, y, x, y);
    uint8_t data[] = {color >> 8, color & 0xFF};
    ST7735_WriteData(data, sizeof(data));
}

#define BLOCK_SIZE 32 // 一次处理的行数

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    // 计算块的大小（字节）
    // w * BLOCK_SIZE 个像素，每个像素 2 字节
    uint32_t bytes_per_line = w * 2;
    uint32_t chunk_height = BLOCK_SIZE;
    if (chunk_height > h) chunk_height = h;

    size_t chunk_size = bytes_per_line * chunk_height;

    // 申请 DMA 内存
    uint8_t *buffer = heap_caps_malloc(chunk_size, MALLOC_CAP_DMA);
    if (!buffer) return;

    // 填充 buffer
    for (int i = 0; i < chunk_size / 2; i++) {
        buffer[i * 2] = color >> 8;
        buffer[i * 2 + 1] = color & 0xFF;
    }

    ST7735_Select();
    gpio_set_level(ST7735_PIN_DC, 1);

    int rows_left = h;
    while (rows_left > 0) {
        int rows_to_send = (rows_left > BLOCK_SIZE) ? BLOCK_SIZE : rows_left;
        size_t bytes_to_send = rows_to_send * bytes_per_line;

        spi_transaction_t t = {
                .length = bytes_to_send * 8,
                .tx_buffer = buffer,
        };
        // 这里如果是大数据量，transmit 比 polling_transmit 更适合配合 DMA
        spi_device_transmit(spi_dev, &t);

        rows_left -= rows_to_send;
    }

    ST7735_Unselect();
    free(buffer);
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

//void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
//    while(*str) {
//        ST7735_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);
//        for(int i = 0; i < font.height; i++) {
//            uint16_t b = font.data[(*str - 32) * font.height + i];
//            for(int j = 0; j < font.width; j++) {
//                uint16_t pixel = ((b << j) & 0x8000) ? color : bgcolor;
//                uint8_t pdata[] = { pixel >> 8, pixel & 0xFF };
//                ST7735_WriteData(pdata, 2);
//            }
//        }
//        x += font.width;
//        str++;
//    }
//}
void ST7735_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor) {
    // 1. 在栈上分配一个缓冲区，大小为一个字符所需的字节数
    // 注意：如果字体非常大，可能需要用 malloc，小字体用栈即可
    size_t buf_size = font.width * font.height * 2;
    uint8_t *buffer = (uint8_t *) heap_caps_malloc(buf_size, MALLOC_CAP_DMA); // 推荐用 DMA 内存，或者普通 malloc 也可以
    if (!buffer) return;

    while (*str) {
        // 边界检查（同原代码）
        if (x + font.width > ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height > ST7735_HEIGHT) break;
            if (*str == ' ') {
                str++;
                continue;
            }
        }

        // 2. 设定写入窗口（整个字符区域）
        ST7735_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

        // 3. 在内存中渲染字符
        int buf_idx = 0;
        for (int i = 0; i < font.height; i++) {
            uint16_t b = font.data[(*str - 32) * font.height + i];
            for (int j = 0; j < font.width; j++) {
                uint16_t pixel = ((b << j) & 0x8000) ? color : bgcolor;
                // ST7735 是大端模式，高位在前
                buffer[buf_idx++] = pixel >> 8;
                buffer[buf_idx++] = pixel & 0xFF;
            }
        }

        // 4. 一次性通过 SPI 发送整个字符的数据
        ST7735_Select();
        gpio_set_level(ST7735_PIN_DC, 1);

        spi_transaction_t t = {
                .length = buf_size * 8, // bit length
                .tx_buffer = buffer,
        };
        spi_device_polling_transmit(spi_dev, &t);

        ST7735_Unselect();

        x += font.width;
        str++;
    }

    free(buffer);
}
/**
 * @brief 适配 LVGL 的显示刷新函数
 * @param x1, y1 矩形区域左上角坐标
 * @param x2, y2 矩形区域右下角坐标
 * @param data   指向颜色缓冲区(RGB565格式)的指针
 */
void ST7735_Flush(int x1, int y1, int x2, int y2, void *data) {
    // 1. 计算要刷新的总像素数和字节数
    // 注意：x2, y2 是包含在内的，所以宽 = x2 - x1 + 1
    uint32_t width = x2 - x1 + 1;
    uint32_t height = y2 - y1 + 1;
    uint32_t bytes_to_send = width * height * 2; // RGB565 占 2 字节

    // 2. 设置显示窗口 (底层已包含 CASET, RASET 和 RAMWR 指令)
    ST7735_SetAddressWindow(x1, y1, x2, y2);

    // 3. 进入数据模式并拉低 CS
    ST7735_Select();
    gpio_set_level(ST7735_PIN_DC, 1); // DC=1 表示数据

    // 4. SPI 传输数据
    // 注意：如果 data 是在 DMA 区域申请的，传输会非常快
    spi_transaction_t t = {
            .length = bytes_to_send * 8, // 长度单位是 bit
            .tx_buffer = data,
    };

    // 这里使用 polling 模式，等待发送完成
    // 如果想要更高性能，可以使用 spi_device_transmit (中断模式)
    spi_device_polling_transmit(spi_dev, &t);

    // 5. 释放 CS
    ST7735_Unselect();
}