#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include <string.h>
#include <driver/gpio.h>
#include "button.h"
#include "ap_wifi.h"
#include "lv_port.h"
#include "gui_guider.h"
#include "custom.h"
#include "esp_lvgl_port.h"
#include <time.h>
#include "esp_sntp.h"
#include "weather.h"

// 【1. 引入刚才写好的 AI 头文件】
#include "ai.h"

#define TAG     "main"
lv_ui guider_ui;

int get_button_level(int gpio) {
    return gpio_get_level(gpio);
}

void long_press(int gpio) {
    ap_wifi_apcfg(true);
}

// 【2. 新增一个短按回调函数，用于触发AI】
void short_press(int gpio) {
    ESP_LOGI(TAG, "Button short pressed! Starting AI chat...");
    // 启动 AI 聊天任务 (不阻塞当前按键中断/回调)
    ai_chat_start();
}

void button_init(void) {
    gpio_config(&(gpio_config_t) {
            .pin_bit_mask = (1ULL << 33),
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
                    // 【3. 将短按回调绑定到我们的触发函数】
                    .short_cb = short_press,
            };
    button_event_set(&button_cfg);
}

static void sntp_finish_callback(struct timeval *tv) {
    struct tm t;
    localtime_r(&tv->tv_sec, &t);
    set_home_time(&guider_ui, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_wday, t.tm_hour, t.tm_min, t.tm_sec);
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
        lvgl_port_unlock();
    }
    ESP_ERROR_CHECK(nvs_flash_init());
    button_init();
    ap_wifi_init(wifi_state_handle);
    setenv("TZ", "CST-8", 1);
    tzset();
    weather_start();

    // while(1) 被你注释掉了，这没关系，FreeRTOS 的任务会继续在后台运行
}