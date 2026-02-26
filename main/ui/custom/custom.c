#include "custom.h"

/* 初始状态设为 false（收起） */
static bool is_expanded = false;

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
        } else {
            set_children_focusable(false); // 收起并移除焦点
            lv_anim_set_values(&a, 256, 0);
            is_expanded = false;
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

    /* 2. 禁止 Textarea 参与焦点游走 */
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