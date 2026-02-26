/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



int screen_digital_clock_1_min_value = 27;
int screen_digital_clock_1_hour_value = 11;
int screen_digital_clock_1_sec_value = 50;
void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 160, 128);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_change
    ui->screen_change = lv_tabview_create(ui->screen);
    lv_obj_set_pos(ui->screen_change, 0, 1);
    lv_obj_set_size(ui->screen_change, 160, 128);
    lv_obj_set_scrollbar_mode(ui->screen_change, LV_SCROLLBAR_MODE_OFF);
    lv_tabview_set_tab_bar_position(ui->screen_change, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(ui->screen_change, 15);

    //Write style for screen_change, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_change, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_change, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_change, lv_color_hex(0x4d4d4d), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_change, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_change, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_change, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_change, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_change, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_change, lv_color_hex(0xeaeff3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_change, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_change, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_change_extra_btnm_main_default
    static lv_style_t style_screen_change_extra_btnm_main_default;
    ui_init_style(&style_screen_change_extra_btnm_main_default);

    lv_style_set_border_width(&style_screen_change_extra_btnm_main_default, 0);
    lv_style_set_radius(&style_screen_change_extra_btnm_main_default, 0);
    lv_style_set_bg_opa(&style_screen_change_extra_btnm_main_default, 255);
    lv_style_set_bg_color(&style_screen_change_extra_btnm_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_change_extra_btnm_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_tabview_get_tab_bar(ui->screen_change), &style_screen_change_extra_btnm_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_change_extra_btnm_items_default
    static lv_style_t style_screen_change_extra_btnm_items_default;
    ui_init_style(&style_screen_change_extra_btnm_items_default);

    lv_style_set_text_color(&style_screen_change_extra_btnm_items_default, lv_color_hex(0x4d4d4d));
    lv_style_set_text_font(&style_screen_change_extra_btnm_items_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_change_extra_btnm_items_default, 255);
    lv_obj_add_style(lv_tabview_get_tab_bar(ui->screen_change), &style_screen_change_extra_btnm_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_change_extra_btnm_items_checked
    static lv_style_t style_screen_change_extra_btnm_items_checked;
    ui_init_style(&style_screen_change_extra_btnm_items_checked);

    lv_style_set_text_color(&style_screen_change_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_text_font(&style_screen_change_extra_btnm_items_checked, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_change_extra_btnm_items_checked, 255);
    lv_style_set_border_width(&style_screen_change_extra_btnm_items_checked, 4);
    lv_style_set_border_opa(&style_screen_change_extra_btnm_items_checked, 255);
    lv_style_set_border_color(&style_screen_change_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_border_side(&style_screen_change_extra_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_radius(&style_screen_change_extra_btnm_items_checked, 0);
    lv_style_set_bg_opa(&style_screen_change_extra_btnm_items_checked, 60);
    lv_style_set_bg_color(&style_screen_change_extra_btnm_items_checked, lv_color_hex(0x2195f6));
    lv_style_set_bg_grad_dir(&style_screen_change_extra_btnm_items_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_tabview_get_tab_bar(ui->screen_change), &style_screen_change_extra_btnm_items_checked, LV_PART_ITEMS|LV_STATE_CHECKED);

    //Write codes home
    ui->screen_change_tab_1 = lv_tabview_add_tab(ui->screen_change,"home");
    lv_obj_t * screen_change_tab_1_label = lv_label_create(ui->screen_change_tab_1);
    lv_label_set_text(screen_change_tab_1_label, "");

    //Write codes screen_label_day
    ui->screen_label_day = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_day, 66, 0);
    lv_obj_set_size(ui->screen_label_day, 76, 13);
    lv_label_set_text(ui->screen_label_day, "2025年4月3日");
    lv_label_set_long_mode(ui->screen_label_day, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_day, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_day, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_day, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_local
    ui->screen_label_local = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_local, 109, -11);
    lv_obj_set_size(ui->screen_label_local, 30, 11);
    lv_label_set_text(ui->screen_label_local, "深圳");
    lv_label_set_long_mode(ui->screen_label_local, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_local, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_local, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_local, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_local, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_local, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_local, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_digital_clock_1
    static bool screen_digital_clock_1_timer_enabled = false;
    ui->screen_digital_clock_1 = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_digital_clock_1, -8, -14);
    lv_obj_set_size(ui->screen_digital_clock_1, 69, 34);
    lv_label_set_text(ui->screen_digital_clock_1, "11:27:50");
    if (!screen_digital_clock_1_timer_enabled) {
        lv_timer_create(screen_digital_clock_1_timer, 1000, NULL);
        screen_digital_clock_1_timer_enabled = true;
    }

    //Write style for screen_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_digital_clock_1, lv_color_hex(0x020202), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_digital_clock_1, &lv_font_ZiTiQuanWeiJunHeiW22_19, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_digital_clock_1, lv_color_hex(0xf2f2f2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_digital_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_w_day
    ui->screen_label_w_day = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_w_day, 100, 13);
    lv_obj_set_size(ui->screen_label_w_day, 38, 11);
    lv_label_set_text(ui->screen_label_w_day, "星期零");
    lv_label_set_long_mode(ui->screen_label_w_day, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_w_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_w_day, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_w_day, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_w_day, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_w_day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_w_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_1
    ui->screen_label_1 = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_1, 96, 28);
    lv_obj_set_size(ui->screen_label_1, 30, 11);
    lv_label_set_text(ui->screen_label_1, "后日");
    lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_1, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_2
    ui->screen_label_2 = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_2, -9, 28);
    lv_obj_set_size(ui->screen_label_2, 30, 11);
    lv_label_set_text(ui->screen_label_2, "今日");
    lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_2, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_4
    ui->screen_label_4 = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_4, 43, 28);
    lv_obj_set_size(ui->screen_label_4, 30, 11);
    lv_label_set_text(ui->screen_label_4, "明日");
    lv_label_set_long_mode(ui->screen_label_4, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_4, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_today
    ui->screen_label_today = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_today, -13, 76);
    lv_obj_set_size(ui->screen_label_today, 43, 11);
    lv_label_set_text(ui->screen_label_today, "28~32℃");
    lv_label_set_long_mode(ui->screen_label_today, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_today, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_today, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_today, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_today, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_today, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_after
    ui->screen_label_after = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_after, 90, 75);
    lv_obj_set_size(ui->screen_label_after, 43, 11);
    lv_label_set_text(ui->screen_label_after, "28~32℃");
    lv_label_set_long_mode(ui->screen_label_after, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_after, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_after, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_after, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_after, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_after, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_tom
    ui->screen_label_tom = lv_label_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_label_tom, 38, 75);
    lv_obj_set_size(ui->screen_label_tom, 41, 11);
    lv_label_set_text(ui->screen_label_tom, "28~32℃");
    lv_label_set_long_mode(ui->screen_label_tom, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_tom, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_tom, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_tom, &lv_font_ZiTiQuanWeiJunHeiW22_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_tom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_tom, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_after
    ui->screen_img_after = lv_image_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_img_after, 95, 40);
    lv_obj_set_size(ui->screen_img_after, 30, 31);
    lv_obj_add_flag(ui->screen_img_after, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_after, &_5_RGB565A8_30x31);
    lv_image_set_pivot(ui->screen_img_after, 50,50);
    lv_image_set_rotation(ui->screen_img_after, 0);

    //Write style for screen_img_after, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_after, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_after, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_tom
    ui->screen_img_tom = lv_image_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_img_tom, 43, 39);
    lv_obj_set_size(ui->screen_img_tom, 30, 31);
    lv_obj_add_flag(ui->screen_img_tom, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_tom, &_4_RGB565A8_30x31);
    lv_image_set_pivot(ui->screen_img_tom, 50,50);
    lv_image_set_rotation(ui->screen_img_tom, 0);

    //Write style for screen_img_tom, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_tom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_tom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_today
    ui->screen_img_today = lv_image_create(ui->screen_change_tab_1);
    lv_obj_set_pos(ui->screen_img_today, -9, 39);
    lv_obj_set_size(ui->screen_img_today, 30, 31);
    lv_obj_add_flag(ui->screen_img_today, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_today, &_3_RGB565A8_30x31);
    lv_image_set_pivot(ui->screen_img_today, 50,50);
    lv_image_set_rotation(ui->screen_img_today, 0);

    //Write style for screen_img_today, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_today, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes AI_talk
    ui->screen_change_tab_2 = lv_tabview_add_tab(ui->screen_change,"AI_talk");
    lv_obj_t * screen_change_tab_2_label = lv_label_create(ui->screen_change_tab_2);
    lv_label_set_text(screen_change_tab_2_label, "");

    //Write codes screen_cont_1
    ui->screen_cont_1 = lv_obj_create(ui->screen_change_tab_2);
    lv_obj_set_pos(ui->screen_cont_1, -20, -16);
    lv_obj_set_size(ui->screen_cont_1, 120, 30);
    lv_obj_set_scrollbar_mode(ui->screen_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_cont_1, lv_color_hex(0xeaeff3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cont_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_btn_5
    ui->screen_btn_5 = lv_button_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_btn_5, 90, 3);
    lv_obj_set_size(ui->screen_btn_5, 20, 20);
    ui->screen_btn_5_label = lv_label_create(ui->screen_btn_5);
    lv_label_set_text(ui->screen_btn_5_label, "");
    lv_label_set_long_mode(ui->screen_btn_5_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_btn_5_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_btn_5, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_btn_5_label, LV_PCT(100));

    //Write style for screen_btn_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_btn_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_btn_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_btn_5, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_btn_5, &_qwen_RGB565A8_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_btn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_btn_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_btn_5, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_btn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_btn_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_btn_3
    ui->screen_btn_3 = lv_button_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_btn_3, 67, 3);
    lv_obj_set_size(ui->screen_btn_3, 20, 20);
    ui->screen_btn_3_label = lv_label_create(ui->screen_btn_3);
    lv_label_set_text(ui->screen_btn_3_label, "");
    lv_label_set_long_mode(ui->screen_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_btn_3_label, LV_PCT(100));

    //Write style for screen_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_btn_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_btn_3, &_zhipu_RGB565A8_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_btn_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_btn_2
    ui->screen_btn_2 = lv_button_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_btn_2, 45, 3);
    lv_obj_set_size(ui->screen_btn_2, 20, 20);
    ui->screen_btn_2_label = lv_label_create(ui->screen_btn_2);
    lv_label_set_text(ui->screen_btn_2_label, "");
    lv_label_set_long_mode(ui->screen_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_btn_2_label, LV_PCT(100));

    //Write style for screen_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_btn_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_btn_2, &_claude_RGB565A8_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_btn_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_btn_1
    ui->screen_btn_1 = lv_button_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_btn_1, 25, 3);
    lv_obj_set_size(ui->screen_btn_1, 20, 20);
    ui->screen_btn_1_label = lv_label_create(ui->screen_btn_1);
    lv_label_set_text(ui->screen_btn_1_label, "");
    lv_label_set_long_mode(ui->screen_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_btn_1_label, LV_PCT(100));

    //Write style for screen_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_btn_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_btn_1, &_chatgpt_RGB565A8_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_btn_4
    ui->screen_btn_4 = lv_button_create(ui->screen_change_tab_2);
    lv_obj_set_pos(ui->screen_btn_4, -14, -11);
    lv_obj_set_size(ui->screen_btn_4, 20, 20);
    ui->screen_btn_4_label = lv_label_create(ui->screen_btn_4);
    lv_label_set_text(ui->screen_btn_4_label, "");
    lv_label_set_long_mode(ui->screen_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_btn_4_label, LV_PCT(100));

    //Write style for screen_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_btn_4, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_btn_4, &_claude_RGB565A8_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_btn_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_btn_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_answer
    ui->screen_label_answer = lv_textarea_create(ui->screen_change_tab_2);
    lv_obj_set_pos(ui->screen_label_answer, -10, 13);
    lv_obj_set_size(ui->screen_label_answer, 121, 76);
    lv_textarea_set_text(ui->screen_label_answer, "Switch AI Claude");
    lv_textarea_set_placeholder_text(ui->screen_label_answer, "");
    lv_textarea_set_password_bullet(ui->screen_label_answer, "*");
    lv_textarea_set_password_mode(ui->screen_label_answer, false);
    lv_textarea_set_one_line(ui->screen_label_answer, false);
    lv_textarea_set_accepted_chars(ui->screen_label_answer, "");
    lv_textarea_set_max_length(ui->screen_label_answer, 318);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->screen_label_answer, ta_event_cb, LV_EVENT_ALL, NULL);
#endif

    //Write style for screen_label_answer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_label_answer, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_answer, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_answer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_answer, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_answer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_label_answer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_label_answer, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_answer, 5, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_label_answer, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_label_answer, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_label_answer, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_label_answer, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_answer, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_label_ask
    ui->screen_label_ask = lv_textarea_create(ui->screen_change_tab_2);
    lv_obj_set_pos(ui->screen_label_ask, 12, -14);
    lv_obj_set_size(ui->screen_label_ask, 122, 26);
    lv_textarea_set_text(ui->screen_label_ask, "Hello World");
    lv_textarea_set_placeholder_text(ui->screen_label_ask, "");
    lv_textarea_set_password_bullet(ui->screen_label_ask, "*");
    lv_textarea_set_password_mode(ui->screen_label_ask, false);
    lv_textarea_set_one_line(ui->screen_label_ask, false);
    lv_textarea_set_accepted_chars(ui->screen_label_ask, "");
    lv_textarea_set_max_length(ui->screen_label_ask, 100);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->screen_label_ask, ta_event_cb, LV_EVENT_ALL, NULL);
#endif

    //Write style for screen_label_ask, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_label_ask, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_ask, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_ask, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_ask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_ask, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_ask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_label_ask, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_label_ask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_label_ask, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_label_ask, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_ask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_ask, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_ask, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_ask, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_ask, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_label_ask, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_label_ask, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_label_ask, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_label_ask, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_ask, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //The custom code of screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen);

}
