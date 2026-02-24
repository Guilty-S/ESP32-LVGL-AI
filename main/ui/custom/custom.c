/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "esp_lvgl_port.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */
extern int screen_home_label_big_hour_value;
extern int screen_home_label_big_min_value;
extern int screen_home_label_big_sec_value;

int screen_home_label_big_year_value;
int screen_home_label_big_month_value;
int screen_home_label_big_day_value;
int screen_home_label_big_w_day_value;

void set_home_time(lv_ui *ui, int year, int month, int day, int w_day, int hour, int min, int sec) {
    screen_home_label_big_hour_value = hour;
    screen_home_label_big_min_value = min;
    screen_home_label_big_sec_value = sec;
    screen_home_label_big_year_value = year;
    screen_home_label_big_month_value = month;
    screen_home_label_big_day_value = day;
    screen_home_label_big_w_day_value = w_day;
    char text[64];
    snprintf(text, sizeof(text), "%04d年%02d月%02d日", year, month, day);
    char week_text[64];
    static const char* week_days_text[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    snprintf(week_text, sizeof(week_text), "%s", week_days_text[screen_home_label_big_w_day_value]);
    lvgl_port_lock(0);
    lv_label_set_text(ui->screen_home_label_1, text);
    lv_label_set_text(ui->screen_home_label_2, week_text);
    lvgl_port_unlock();
}

void custom_init(lv_ui *ui) {
    /* Add your codes here */
}

