#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include <string.h>
#include <driver/gpio.h>
#include "ap_wifi.h"
#include "button.h"

#define TAG     "main"

int get_button_level(int gpio) {
    return gpio_get_level(gpio);
}

void long_press(int gpio) {
    ap_wifi_apcfg(true);
}

void button_init(void) {
    gpio_config(&(gpio_config_t){
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
                    .short_cb = NULL,
            };
    button_event_set(&button_cfg);
}

void wifi_state_handle(WIFI_STATE state) {
    if (state == WIFI_STATE_CONNECTED) {
        ESP_LOGI(TAG, "Wifi connected");
    } else if (state == WIFI_STATE_DISCONNECTED) {
        ESP_LOGI(TAG, "Wifi disconnected");
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    button_init();
    ap_wifi_init(wifi_state_handle);
}
