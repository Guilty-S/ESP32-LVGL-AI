#include "freertos/FreeRTOS.h"//RTOS支持
#include "freertos/task.h"
#include "esp_spiffs.h"//挂载功能
#include "esp_log.h"
#include "nvs_flash.h"//Flash存储
#include "cJSON.h"//解析
#include "button.h"
#include "esp_sntp.h"
#include "weather.h"//组件依赖
#include "ai_chat.h"
#include "ap_wifi.h"
#include "esp_lvgl_port.h"
#include "lv_port.h"
#include "gui_guider.h"
#include "custom.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <driver/gpio.h>

#define TAG     "main"

static void img_spiffs_init(void) {
    //定义挂载点
    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/img",            //挂载点
            .partition_label = "img",         //分区名称
            .max_files = 5,                    //最大打开的文件数
            .format_if_mount_failed = false    //挂载失败是否执行格式化
    };
    //挂载spiffs
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
}

lv_ui guider_ui;
static char ai_buffer[512];
static int ai_buf_pos = 0;

void my_ai_stream_cb(const char *fragment) {
    // 1. 先把收到的碎片存进缓冲区，而不是直接画屏幕
    int len = strlen(fragment);
    if (ai_buf_pos + len < sizeof(ai_buffer) - 1) {
        strcpy(ai_buffer + ai_buf_pos, fragment);
        ai_buf_pos += len;
    }
    // 2. 只有当缓冲区攒够了 15 个字符，或者包含换行符时，才去刷新屏幕
    // 这样可以把 100 次每秒的刷新降到 5-6 次，既流畅又不卡顿
    if (ai_buf_pos > 15 || strchr(fragment, '\n')) {
        if (lvgl_port_lock(portMAX_DELAY)) {
            // 一次性把攒好的字加上去
            lv_textarea_add_text(guider_ui.screen_label_answer, ai_buffer);
            // 确保滚动条在最下面
            // (如果是 LVGL 8.3 及以上，通常加字会自动滚，这行可选)
            // lv_obj_scroll_to_view(lv_textarea_get_cursor_pos(guider_ui.screen_ta_1), LV_ANIM_OFF);
            lvgl_port_unlock();
        }
        // 3. 清空缓冲区，准备攒下一波
        ai_buf_pos = 0;
        ai_buffer[0] = '\0';
    }
}

int get_button_level(int gpio) {
    return gpio_get_level(gpio);
}

void long_press(int gpio) {
    ap_wifi_apcfg(true);
}

// 1. 定义预设问题库
static const char *my_questions[] = {
        "你是什么模型.",
        "讲一个故事.",
        "解释一下ESP32.",
        "关于月球.",
        "关于LVGL?"
};
static int question_idx = 0;

// 2. 新按键的回调：仅负责切换并显示问题
void switch_question_press(int gpio) {
    question_idx = (question_idx + 1) % 5; // 5个问题循环

    if (lvgl_port_lock(portMAX_DELAY)) {
        // 在新增加的 textarea (或label) 上显示当前选中的问题
        // 假设你在 GUI Guider 里的名字叫 label_ask
        lv_textarea_set_text(guider_ui.screen_label_ask, my_questions[question_idx]);
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Switched to Question: %s", my_questions[question_idx]);
}

// 原有的短按回调（GPIO 33）：负责拼接要求并发送
void short_press(int gpio) {
    if (lvgl_port_lock(portMAX_DELAY)) {
        // 1. 获取当前屏幕上显示的问题文字 (注意：这是只读的)
        const char *original_prompt = lv_textarea_get_text(guider_ui.screen_label_ask);

        // 2. 清空回答区域，准备显示
        lv_textarea_set_text(guider_ui.screen_label_answer, "");

        // ================= 新增拼接逻辑 =================
        // 定义你要追加的提示词
        const char *append_str = "，请控制在50字以内。";

        // 计算需要的内存大小：原字符串长度 + 追加字符串长度 + 1 (结束符 '\0')
        size_t needed_size = strlen(original_prompt) + strlen(append_str) + 1;

        // 动态分配一块新内存用来存拼接后的完整句子
        char *combined_prompt = (char *) malloc(needed_size);

        if (combined_prompt != NULL) {
            // 用 sprintf 将两者拼接到 combined_prompt 中
            sprintf(combined_prompt, "%s%s", original_prompt, append_str);

            // 3. 启动 AI（把拼接好的新字符串传进去）
            ai_chat_start_with_prompt(combined_prompt);

            // 4. 发送完毕后，释放我们刚刚申请的内存
            // (因为你的 ai_chat_start_with_prompt 内部有 strdup 拷贝，所以这里安全释放)
            free(combined_prompt);
        } else {
            ESP_LOGE(TAG, "Malloc failed for combined prompt!");
        }
        // ==============================================

        // 5. 操作完毕，解锁 LVGL
        lvgl_port_unlock();
    }
}

void button_init(void) {
    gpio_config(&(gpio_config_t) {
            .pin_bit_mask = (1ULL << 33) | (1ULL << 25),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
    });
    button_config_t button_cfg =
            {
                    .active_level = 0,
                    .getlevel_cb = get_button_level,
                    .gpio_num = 33,
                    .long_cb = long_press,
                    .long_press_time = 3000,
                    .short_cb = short_press,
            };
    button_event_set(&button_cfg);
    // 新增 GPIO 25 的按键配置 (用于切换任务)
    button_config_t switch_btn_cfg = {
            .active_level = 0,
            .getlevel_cb = get_button_level,
            .gpio_num = 25,  // <--- 你的新按键引脚
            .short_cb = switch_question_press, // 绑定切换函数
    };
    button_event_set(&switch_btn_cfg);
}





void app_main(void) {
    img_spiffs_init();
    lv_port_init();
    if (lvgl_port_lock(0)) {
        setup_ui(&guider_ui);
        custom_init(&guider_ui);
        lv_obj_set_style_text_line_space(guider_ui.screen_label_answer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_scrollbar_mode(guider_ui.screen_label_ask, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_scrollbar_mode(guider_ui.screen_label_ask, LV_SCROLLBAR_MODE_OFF);
        lvgl_port_unlock();
    }
    ESP_ERROR_CHECK(nvs_flash_init());
    button_init();
    ap_wifi_init(wifi_state_handle);
    setenv("TZ", "CST-8", 1);
    tzset();
    weather_set_ui_callback(update_weather_ui_bridge);
    local_set_ui_callback(update_local_ui_bridge);
    ai_set_stream_callback(my_ai_stream_cb);
    weather_start();
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}