#include "freertos/FreeRTOS.h"//RTOS支持
#include "freertos/task.h"
#include "esp_spiffs.h"//挂载功能
#include "esp_log.h"
#include "nvs_flash.h"//Flash存储
#include "cJSON.h"//解析
#include "my_button.h"
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
#include "app_control.h"

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