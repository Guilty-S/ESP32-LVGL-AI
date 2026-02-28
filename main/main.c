#include "freertos/FreeRTOS.h"//RTOS支持
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"//Flash存储
#include "my_button.h"
#include "weather.h"//组件依赖
#include "ai_chat.h"
#include "ap_wifi.h"
#include "esp_lvgl_port.h"
#include "lv_port.h"
#include "gui_guider.h"
#include "custom.h"
#include <string.h>
#include "app_control.h"

#define TAG     "main"

lv_ui guider_ui;

void app_main(void) {
    img_spiffs_init();
    lv_port_init();
    lvgl_ui_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    button_init();
    ap_wifi_init(wifi_state_handle);
    local_set_ui_callback(update_local_ui_bridge);
    ai_set_stream_callback(my_ai_stream_cb);
    weather_start();
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}