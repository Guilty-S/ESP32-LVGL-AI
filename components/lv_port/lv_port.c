#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "st7735.h"  // 你的驱动
#include "driver/gpio.h"
#include "lvgl_private.h" // 某些 v9 版本可能需要此头文件来调用内部 swap
#include "../../managed_components/espressif__esp_lvgl_port/include/esp_lvgl_port.h"

// 按键定义
#define BTN_PREV_IO    14
#define BTN_NEXT_IO    13
#define BTN_ENTER_IO   12
#define BTN_ESC_IO     33

static const char *TAG = "lv_port";
lv_indev_t * indev_keypad;
lv_group_t * g;

/**
 * @brief 显示刷新回调 (LVGL v9)
 */
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    // 【关键修复】LVGL v9 移除了 LV_COLOR_16_SWAP 宏。
    // 如果你的屏幕颜色不对（花屏或红蓝反转），必须在这里手动交换字节。
    // ST7735 通常需要这一步，因为 ESP32 是小端序。
    lv_draw_sw_rgb565_swap(px_map, w * h);

    // 调用你的 ST7735 驱动
    ST7735_Flush(area->x1, area->y1, area->x2, area->y2, (void *)px_map);

    // 通知 LVGL 刷新结束
    lv_display_flush_ready(disp);
}

static void lv_disp_init(void)
{
    // 1. 创建显示对象
    lv_display_t * disp = lv_display_create(ST7735_WIDTH, ST7735_HEIGHT);

    // 2. 明确设置为 RGB565 格式
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    // 3. 分配显存 (建议用 heap_caps_malloc 分配在内部 RAM 并支持 DMA)
    // 分配 1/10 屏幕大小的缓冲
    const size_t buf_size_byte = ST7735_WIDTH * ST7735_HEIGHT * 2 / 10;

    void *buf1 = heap_caps_malloc(buf_size_byte, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    void *buf2 = heap_caps_malloc(buf_size_byte, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);

    if(!buf1 || !buf2) {
        ESP_LOGE(TAG, "Display buffer malloc failed");
        return;
    }

    // 4. 设置缓冲和回调
    lv_display_set_buffers(disp, buf1, buf2, buf_size_byte, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, disp_flush);
}

// ------------------ 输入设备 (按键) 部分 ------------------

static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    data->state = LV_INDEV_STATE_REL;

    // v9 获取组的方式
    lv_group_t * group = lv_indev_get_group(indev);
    bool is_editing = false;
    if (group) {
        is_editing = lv_group_get_editing(group);
    }

    if(gpio_get_level(BTN_PREV_IO) == 0) {
        data->state = LV_INDEV_STATE_PR;
        last_key = LV_KEY_PREV;
    } else if(gpio_get_level(BTN_NEXT_IO) == 0) {
        data->state = LV_INDEV_STATE_PR;
        last_key = LV_KEY_NEXT;
    } else if(gpio_get_level(BTN_ENTER_IO) == 0) {
        data->state = LV_INDEV_STATE_PR;
        last_key = LV_KEY_ENTER;
    } else if(gpio_get_level(BTN_ESC_IO) == 0) {
        data->state = LV_INDEV_STATE_PR;
        last_key = LV_KEY_ESC;
    }
    data->key = last_key;
}

void lv_indev_init(void)
{
    // GPIO 初始化
    gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL<<BTN_PREV_IO)|(1ULL<<BTN_NEXT_IO)|(1ULL<<BTN_ENTER_IO)|(1ULL<<BTN_ESC_IO),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // 创建输入设备
    indev_keypad = lv_indev_create();
    lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_keypad, keypad_read);

    // 创建组并绑定
    g = lv_group_create();
    lv_indev_set_group(indev_keypad, g);
    lv_group_set_default(g);
}

void lv_port_init(void)
{
    ST7735_Init();
    const lvgl_port_cfg_t lvgl_cfg = {
            .task_priority = 4,       // 任务优先级
            .task_stack = 8192,       // 栈大小
            .task_affinity = 1,       // 运行在核心 1
            .task_max_sleep_ms = 500,
            .timer_period_ms = 5      // Tick 周期
    };
    lvgl_port_init(&lvgl_cfg);
    if(lvgl_port_lock(0)){
        lv_disp_init();
        lv_indev_init();
        lvgl_port_unlock();
    }
}