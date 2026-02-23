//
// Created by Administrator on 2026/2/20.
//

#ifndef ESP32_CLION_UI_HOME_H
#define ESP32_CLION_UI_HOME_H

#include "misc/lv_types.h"

void led_pwm_init(void);
void set_led_brightness(uint32_t duty);
void ui_home_create(void);
void ui_update_timer_cb(lv_timer_t * timer);
#endif //ESP32_CLION_LV_UI_H
