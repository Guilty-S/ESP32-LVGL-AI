#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "button.h"
#include "ap_wifi.h"
#include "lv_port.h"
#include "gui_guider.h"
#include "custom.h"
#include "esp_lvgl_port.h"
#include "esp_sntp.h"
#include "weather.h"
#include "ai_chat.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <driver/gpio.h>

#define TAG     "main"
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
static const char* my_questions[] = {
    "Introduce yourself in 50 words (English).",
    "Tell a short joke about AI.",
    "Explain what is ESP32 in one sentence.",
    "Write a 4-line poem about the moon.",
    "What is the latest version of LVGL?"
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

// 3. 原有的短按回调（GPIO 33）：负责发送当前 label_ask 里的文字
void short_press(int gpio) {
    if (lvgl_port_lock(portMAX_DELAY)) {
        // 获取当前屏幕上显示的问题文字
        const char* current_prompt = lv_textarea_get_text(guider_ui.screen_label_ask);
        
        // 清空回答区域，准备显示
        lv_textarea_set_text(guider_ui.screen_label_answer, "");
        lvgl_port_unlock();
        
        // 启动 AI（这里调用你带参数的启动函数）
        ai_chat_start_with_prompt(current_prompt); 
    }
}

void button_init(void) {
     gpio_config(&(gpio_config_t) {
             .pin_bit_mask = (1ULL << 33)| (1ULL << 25),
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

static void sntp_finish_callback(struct timeval *tv) {
    struct tm t;
    localtime_r(&tv->tv_sec, &t);
//    set_home_time(&guider_ui, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_wday, t.tm_hour, t.tm_min, t.tm_sec);
}

void my_sntp_init(void) {
    if (!esp_sntp_enabled()) {
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "ntp.aliyun.com");
        esp_sntp_setservername(1, "time.asia.apple.com");
        esp_sntp_setservername(2, "pool.ntp.org");
        esp_sntp_set_time_sync_notification_cb(sntp_finish_callback);
        esp_sntp_init();
    }
}

void wifi_state_handle(WIFI_STATE state) {
    if (state == WIFI_STATE_CONNECTED) {
        my_sntp_init();
        ESP_LOGI(TAG, "Wifi connected");

        // 【可选】如果你想连上WiFi后自动和AI说一句话，也可以在这里写：
        // ai_chat_start();

    } else if (state == WIFI_STATE_DISCONNECTED) {
        ESP_LOGI(TAG, "Wifi disconnected");
    }
}

void app_main(void) {
    lv_port_init();
    if (lvgl_port_lock(portMAX_DELAY)) {
        setup_ui(&guider_ui);
        custom_init(&guider_ui);
        lv_obj_set_style_text_line_space(guider_ui.screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_scrollbar_mode(guider_ui.screen_label_ask, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_scrollbar_mode(guider_ui.screen_label_ask, LV_SCROLLBAR_MODE_OFF);
        lvgl_port_unlock();
    }
     ai_set_stream_callback(my_ai_stream_cb);
    ESP_ERROR_CHECK(nvs_flash_init());
     button_init();
    ap_wifi_init(wifi_state_handle);
//    setenv("TZ", "CST-8", 1);
//    tzset();
//    weather_start();

    // while(1) 被你注释掉了，这没关系，FreeRTOS 的任务会继续在后台运行
}