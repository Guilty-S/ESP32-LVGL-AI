#include "lvgl.h"
#include "lv_port.h"
#include "lsm6dsv16x.h"
#include "ui_home.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include <stdio.h>

#define LED_GPIO GPIO_NUM_2

// 全局变量声明
char lsm_msg[20];
char led_msg[20];
extern lv_group_t *g;

// 【修复1】这里必须加分号 ;
LV_IMAGE_DECLARE(lsm);

static lv_obj_t *s_lsm_image;
static lv_obj_t *s_lsm_label;
static lv_obj_t *s_led_label;
int32_t led_value;
static lv_obj_t *s_light_slider;
static lv_obj_t *test_slider;

// 【修复2】提前声明 PWM 函数，防止编译警告或链接错误
void led_pwm_init(void);
void set_led_brightness(uint32_t duty);

// 定时器回调
void ui_update_timer_cb(lv_timer_t *timer) {
    char buf[32];
    // 假设 pitch 已经在 lsm6dsv16x.h 中声明为 extern float pitch;
    snprintf(buf, sizeof(buf), "LSM: %.2f", pitch);
    lv_label_set_text(s_lsm_label, buf);

    snprintf(buf, sizeof(buf), "LED: %d%%", (int) led_value);
    lv_label_set_text(s_led_label, buf);
}

// 事件回调
void light_slider_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * slider = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        led_value = lv_slider_get_value(slider);
        set_led_brightness(255 * led_value / 100);
    } else if (code == LV_EVENT_CLICKED) {
        // 点击切换编辑模式
        if(g) {
            bool is_editing = lv_group_get_editing(g);
            lv_group_set_editing(g, !is_editing);
        }
    }
}

// UI 创建主函数
void ui_home_create(void) {
    // 【v9 适配】使用 lv_screen_active()
    lv_obj_t * scr = lv_screen_active();

    // 1. 设置屏幕样式
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    // 2. 创建 Slider 1
    s_light_slider = lv_slider_create(scr);
    lv_obj_set_pos(s_light_slider, 20, 110);
    lv_obj_set_size(s_light_slider, 90, 10);
    lv_slider_set_range(s_light_slider, 0, 100);
    lv_slider_set_value(s_light_slider, led_value, LV_ANIM_OFF);

    // 3. 创建 Slider 2
    test_slider = lv_slider_create(scr);
    lv_obj_set_pos(test_slider, 20, 80);
    lv_obj_set_size(test_slider, 90, 10);
    lv_slider_set_range(test_slider, 0, 100);
    lv_slider_set_value(test_slider, led_value, LV_ANIM_OFF);

    // 4. Slider 样式
    lv_obj_set_style_bg_color(s_light_slider, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(test_slider, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(s_light_slider, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_KNOB | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(test_slider, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_KNOB | LV_STATE_FOCUSED);

    lv_obj_set_style_bg_color(s_light_slider, lv_palette_main(LV_PALETTE_RED), LV_PART_KNOB | LV_STATE_EDITED);
    lv_obj_set_style_bg_color(test_slider, lv_palette_main(LV_PALETTE_RED), LV_PART_KNOB | LV_STATE_EDITED);

    // 事件绑定
    lv_obj_add_event_cb(s_light_slider, light_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(test_slider, light_slider_event_cb, LV_EVENT_ALL, NULL);

    // 5. 图片 (v9 适配: lv_image_create)
    s_lsm_image = lv_image_create(scr);
    lv_image_set_src(s_lsm_image, &lsm);
    lv_image_set_scale(s_lsm_image, 40); // v9 使用 scale 替代 zoom
    lv_image_set_pivot(s_lsm_image, 0, 0);
    lv_obj_set_pos(s_lsm_image, 0, 0);

    // 6. Label
    s_lsm_label = lv_label_create(scr);
    lv_obj_set_pos(s_lsm_label, 40, 0);
    sprintf(lsm_msg, "LSM:%.2f", pitch);
    lv_label_set_text(s_lsm_label, lsm_msg);
    lv_obj_set_style_text_color(s_lsm_label, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_obj_set_style_text_font(s_lsm_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);

    s_led_label = lv_label_create(scr);
    lv_obj_set_pos(s_led_label, 40, 40);
    sprintf(led_msg, "LED:%d%%", (int) led_value);
    lv_label_set_text(s_led_label, led_msg);
    lv_obj_set_style_text_color(s_led_label, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_obj_set_style_text_font(s_led_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);

    // 启动刷新定时器
    lv_timer_create(ui_update_timer_cb, 50, NULL);
}

// ------------------ PWM 驱动部分 ------------------

void led_pwm_init(void) {
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_LOW_SPEED_MODE,
            .timer_num        = LEDC_TIMER_0,
            .duty_resolution  = LEDC_TIMER_8_BIT,
            .freq_hz          = 5000,
            .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
            .speed_mode     = LEDC_LOW_SPEED_MODE,
            .channel        = LEDC_CHANNEL_0,
            .timer_sel      = LEDC_TIMER_0,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = LED_GPIO,
            .duty           = 0,
            .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void set_led_brightness(uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}