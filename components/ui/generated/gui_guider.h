/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_change;
	lv_obj_t *screen_change_tab_1;
	lv_obj_t *screen_change_tab_2;
	lv_obj_t *screen_label_day;
	lv_obj_t *screen_label_local;
	lv_obj_t *screen_digital_clock_1;
	lv_obj_t *screen_label_w_day;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_label_2;
	lv_obj_t *screen_label_4;
	lv_obj_t *screen_label_today;
	lv_obj_t *screen_label_after;
	lv_obj_t *screen_label_tom;
	lv_obj_t *screen_img_after;
	lv_obj_t *screen_img_tom;
	lv_obj_t *screen_img_today;
	lv_obj_t *screen_cont_1;
	lv_obj_t *screen_btn_5;
	lv_obj_t *screen_btn_5_label;
	lv_obj_t *screen_btn_3;
	lv_obj_t *screen_btn_3_label;
	lv_obj_t *screen_btn_2;
	lv_obj_t *screen_btn_2_label;
	lv_obj_t *screen_btn_1;
	lv_obj_t *screen_btn_1_label;
	lv_obj_t *screen_btn_4;
	lv_obj_t *screen_btn_4_label;
	lv_obj_t *screen_label_answer;
	lv_obj_t *screen_label_ask;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen(lv_ui *ui);
LV_IMAGE_DECLARE(_5_RGB565A8_30x31);
LV_IMAGE_DECLARE(_4_RGB565A8_30x31);
LV_IMAGE_DECLARE(_3_RGB565A8_30x31);

LV_IMAGE_DECLARE(_qwen_RGB565A8_20x20);

LV_IMAGE_DECLARE(_zhipu_RGB565A8_20x20);

LV_IMAGE_DECLARE(_claude_RGB565A8_20x20);

LV_IMAGE_DECLARE(_chatgpt_RGB565A8_20x20);

LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_10)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_18)
LV_FONT_DECLARE(lv_font_montserratMedium_10)
LV_FONT_DECLARE(lv_customer_font_ZiTiQuanWeiJunHeiW22_10)


#ifdef __cplusplus
}
#endif
#endif
