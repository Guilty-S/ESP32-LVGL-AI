#include "custom.h"
#include "ai_chat.h"
#include "esp_lvgl_port.h"


/* 初始状态设为 false（收起） */
static bool is_expanded = false;
static const ai_config_t configs[] = {
    {"Claude", "http://1.95.142.151:3000/v1/chat/completions", "sk-WQYbcQdw9N3l7JMnBN4i5c4mqSLjEyfHg4MJevYbMWQC5tIe", "claude-3-5-sonnet-20240620", "你好,我是Claude."},
    {"ChatGPT", "https://open.bigmodel.cn/api/paas/v4/chat/completions", "2023c448090d4e039823d4ea20bdd2b2.MXBC7gD7HZ0UU8jX", "glm-4-flash", "你好,我是ChatGPT."},
    {"GLM", "https://open.bigmodel.cn/api/paas/v4/chat/completions", "2023c448090d4e039823d4ea20bdd2b2.MXBC7gD7HZ0UU8jX", "glm-4-flash", "你好,我是GLM-4."},
    {"Qwen", "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions", "sk-68ff9e5765c44b46ace7aa21fa747812", "qwen-max", "你好,我是Qwen."},
};
/* --------------------------------------------------------
 *  [核心修改]：精确控制顺序和焦点跳转
 * -------------------------------------------------------- */
static void set_children_focusable(bool focusable)
{
    // 获取当前主按钮所在的组
    lv_group_t * g = lv_obj_get_group(guider_ui.screen_btn_4);
    if(g == NULL) return;

    if(focusable) {
        /* 展开时：按照你想要的顺序 [1 -> 2 -> 3 -> 5] 依次加入组 */
        /* LVGL 组遵循先入先出原则，后加入的会在导航顺序的后面 */
        lv_group_add_obj(g, guider_ui.screen_btn_1);
        lv_group_add_obj(g, guider_ui.screen_btn_2);
        lv_group_add_obj(g, guider_ui.screen_btn_3);
        lv_group_add_obj(g, guider_ui.screen_btn_5);

        // 允许触摸点击聚焦（可选）
        lv_obj_add_flag(guider_ui.screen_btn_1, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_add_flag(guider_ui.screen_btn_2, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_add_flag(guider_ui.screen_btn_3, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_add_flag(guider_ui.screen_btn_5, LV_OBJ_FLAG_CLICK_FOCUSABLE);

        /* 【关键一步】：强制将焦点从 btn_4 跳转到 btn_1 */
        lv_group_focus_obj(guider_ui.screen_btn_1);

    } else {
        /* 收起时：将所有子按钮移除组 */
        lv_group_remove_obj(guider_ui.screen_btn_1);
        lv_group_remove_obj(guider_ui.screen_btn_2);
        lv_group_remove_obj(guider_ui.screen_btn_3);
        lv_group_remove_obj(guider_ui.screen_btn_5);

        lv_obj_clear_flag(guider_ui.screen_btn_1, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_clear_flag(guider_ui.screen_btn_2, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_clear_flag(guider_ui.screen_btn_3, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_clear_flag(guider_ui.screen_btn_5, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        
        /* 焦点会自动回到 btn_4 (因为它是组里剩下的合法对象) */
    }
}

/* --------------------------------------------------------
 *  动画执行回调函数 (保持不变，但建议确认 max_width 足够大以容纳 4 个按钮)
 * -------------------------------------------------------- */
static void cont_1_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = var;
    int32_t w;

    int32_t min_width = 0;   
    int32_t max_width = 120; // 【注意】：加了 btn_5，宽度可能需要从 130 改大到 160 左右

    w = lv_map(v, 0, 256, min_width, max_width);
    lv_obj_set_width(obj, w);

    if(v > LV_OPA_COVER) v = LV_OPA_COVER;

    // 内部透明度渐变
    lv_obj_set_style_opa(guider_ui.screen_btn_1, v, 0);
    lv_obj_set_style_opa(guider_ui.screen_btn_2, v, 0);
    lv_obj_set_style_opa(guider_ui.screen_btn_3, v, 0);
    lv_obj_set_style_opa(guider_ui.screen_btn_5, v, 0);
}

/* --------------------------------------------------------
 *  子按钮点击事件回调
 * -------------------------------------------------------- */
void child_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * clicked_btn = lv_event_get_target(e);
        int model_index = 0;
        // 判断点击的是哪个按钮，并确定对应的配置索引
        if (clicked_btn == guider_ui.screen_btn_1) model_index = 1;
        else if (clicked_btn == guider_ui.screen_btn_2) model_index = 0;
        else if (clicked_btn == guider_ui.screen_btn_3) model_index = 2;
        else if (clicked_btn == guider_ui.screen_btn_5) model_index = 3;
        ai_chat_set_config(configs[model_index]);
        lv_textarea_set_text(guider_ui.screen_label_answer, configs[model_index].welcome_msg);


        const void * bg_img = lv_obj_get_style_bg_img_src(clicked_btn, LV_PART_MAIN);
        if (bg_img != NULL) {
            lv_obj_set_style_bg_img_src(guider_ui.screen_btn_4, bg_img, LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        if(is_expanded) {
            set_children_focusable(false); // 收起并移除焦点

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, guider_ui.screen_cont_1); 
            lv_anim_set_exec_cb(&a, cont_1_anim_cb);
            lv_anim_set_time(&a, 200);
            lv_anim_set_values(&a, 256, 0); 
            lv_anim_start(&a);
            
            is_expanded = false;
            lv_obj_clear_flag(guider_ui.screen_label_ask, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/* --------------------------------------------------------
 *  主按钮 (btn_4) 点击事件回调
 * -------------------------------------------------------- */
void btn_4_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_anim_t a;
        lv_anim_init(&a);
        
        lv_anim_set_var(&a, guider_ui.screen_cont_1); 
        lv_anim_set_exec_cb(&a, cont_1_anim_cb);
        lv_anim_set_time(&a, 200);

        if(!is_expanded) {
            set_children_focusable(true); // 展开并跳转焦点到 btn_1
            lv_anim_set_values(&a, 0, 256);
            is_expanded = true;
            lv_obj_add_flag(guider_ui.screen_label_ask, LV_OBJ_FLAG_HIDDEN);//防遮挡
        } else {
            set_children_focusable(false); // 收起并移除焦点
            lv_anim_set_values(&a, 256, 0);
            is_expanded = false;
            lv_obj_clear_flag(guider_ui.screen_label_ask, LV_OBJ_FLAG_HIDDEN);//恢复
        }
        
        lv_anim_start(&a);
    }
}

/* --------------------------------------------------------
 *  初始化
 * -------------------------------------------------------- */
void custom_init(lv_ui *ui)
{
    /* 1. 初始隐藏容器 */
    lv_obj_set_width(ui->screen_cont_1, 0);
    // 初始设置所有子项透明度为 0
    lv_obj_set_style_opa(ui->screen_btn_1, 0, 0);
    lv_obj_set_style_opa(ui->screen_btn_2, 0, 0);
    lv_obj_set_style_opa(ui->screen_btn_3, 0, 0);
    lv_obj_set_style_opa(ui->screen_btn_5, 0, 0);
    
    is_expanded = false;
    // 1. 禁用滑动条
    lv_obj_set_scrollbar_mode(ui->screen_label_ask, LV_SCROLLBAR_MODE_OFF);
    // 2. 踢出焦点组，防止按键误入
    lv_group_t * g = lv_obj_get_group(ui->screen_label_ask);
    if (g) lv_group_remove_obj(ui->screen_label_ask);
        // 3. 禁止点击获取焦点
    lv_obj_clear_flag(ui->screen_label_ask, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_textarea_set_text(ui->screen_label_ask, "请选择问题.");

    /* 禁止 Textarea 参与焦点游走 */
    lv_group_t * g_ta = lv_obj_get_group(ui->screen_label_answer);
    if (g_ta != NULL) {
        lv_group_remove_obj(ui->screen_label_answer);
    }
    lv_obj_clear_flag(ui->screen_label_answer, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    /* 3. 绑定事件 (确保包含了 btn_5) */
    lv_obj_add_event_cb(ui->screen_btn_4, btn_4_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_btn_1, child_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_btn_2, child_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_btn_3, child_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_btn_5, child_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // 初始状态下确保子按钮不在组内
    set_children_focusable(false);
}

extern int screen_digital_clock_1_min_value;
extern int screen_digital_clock_1_hour_value;
extern int screen_digital_clock_1_sec_value;

int screen_home_label_big_year_value;
int screen_home_label_big_month_value;
int screen_home_label_big_day_value;
int screen_home_label_big_w_day_value;

void set_home_time(lv_ui *ui, int year, int month, int day, int w_day, int hour, int min, int sec) {
    screen_digital_clock_1_hour_value = hour;
    screen_digital_clock_1_min_value = min;
    screen_digital_clock_1_sec_value = sec;
    screen_home_label_big_year_value = year;
    screen_home_label_big_month_value = month;
    screen_home_label_big_day_value = day;
    screen_home_label_big_w_day_value = w_day;
    char text[64];
    snprintf(text, sizeof(text), "%04d年%02d月%02d日", year, month, day);
    char week_text[64];
    static const char* week_days_text[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    snprintf(week_text, sizeof(week_text), "%s", week_days_text[screen_home_label_big_w_day_value]);
    lvgl_port_lock(portMAX_DELAY);
    lv_label_set_text(ui->screen_label_day, text);
    lv_label_set_text(ui->screen_label_w_day, week_text);
    lvgl_port_unlock();
}
void set_today_img(lv_ui* ui, const char* img_path,int low,int high)
{
    char temp_text[32];
    lvgl_port_lock(0);
    lv_img_set_src(ui->screen_img_today, img_path);
    snprintf(temp_text, sizeof(temp_text), "%d-%d℃", low, high);
    lv_label_set_text(ui->screen_label_today, temp_text);
    lvgl_port_unlock();
}
void set_tomorrow_img(lv_ui* ui, const char* img_path,int low,int high)
{
    char temp_text[32];
    lvgl_port_lock(0);
    lv_img_set_src(ui->screen_img_tom, img_path);
    snprintf(temp_text, sizeof(temp_text), "%d-%d℃", low, high);
    lv_label_set_text(ui->screen_label_tom, temp_text);
    lvgl_port_unlock();
}
void set_after_img(lv_ui* ui, const char* img_path,int low,int high)
{
    char temp_text[32];
    lvgl_port_lock(0);
    lv_img_set_src(ui->screen_img_after, img_path);
    snprintf(temp_text, sizeof(temp_text), "%d-%d℃", low, high);
    lv_label_set_text(ui->screen_label_after, temp_text);
    lvgl_port_unlock();
}
void update_weather_ui_bridge(int day, const char* img_path, int low, int high) {
    // 根据天数调用你原来的函数
    if (day == 0) set_today_img(&guider_ui, img_path, low, high);
    else if (day == 1) set_tomorrow_img(&guider_ui, img_path, low, high);
    else if (day == 2) set_after_img(&guider_ui, img_path, low, high);
}
void set_home_city(lv_ui *ui, const char* city_name) {
    char city_text[32];
    lvgl_port_lock(0);
    snprintf(city_text,sizeof(city_text),"%s", city_name);
    lv_label_set_text(ui->screen_label_local, city_text);
    lvgl_port_unlock();
}
void update_local_ui_bridge(const char* city_name) {
    set_home_city(&guider_ui, city_name);
}