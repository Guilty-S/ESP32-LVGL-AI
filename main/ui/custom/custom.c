#include "custom.h"

/* 定义一个静态变量来记录当前的展开/收起状态。
 * 假设你的 UI 刚加载时是展开的，这里默认为 true。如果是收起的则改为 false */
static bool is_expanded = true;

/* --------------------------------------------------------
 *  动画执行回调函数：负责实时更新宽度和内部按钮的透明度
 * -------------------------------------------------------- */
static void cont_1_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = var;
    int32_t w;

    // 【关键修改点 1】：修改你的最大和最小宽度！
    // 假设你的 cont_1 收起时宽度为 0，完全展开时宽度为 120 (请根据GUI Guider实际像素修改)
    int32_t min_width = 0;   
    int32_t max_width = 100; 

    // lv_map 的作用是将 0~256 的动画值等比例映射到 min_width ~ max_width
    w = lv_map(v, 0, 256, min_width, max_width);
    lv_obj_set_width(obj, w);

    // 【可选】：处理缩放方向
    // LVGL默认改变宽度是从右向左缩（左上角X坐标固定）。
    // 因为你的 btn_4 在右侧，如果你希望 cont_1 是“向右侧收缩”进 btn_4 里，
    // 你需要同时动态改变X坐标。假设 cont_1 展开时的起始 X 坐标是 20：
    // lv_obj_set_x(obj, 20 + (max_width - w)); 
    // （如果不需要这个效果或你在UI里已经用了对齐锚点，请忽略这行）

    // 限制透明度最大值为 255 (LV_OPA_COVER)
    if(v > LV_OPA_COVER) v = LV_OPA_COVER;

    // 遍历修改 cont_1 内部的所有子对象（btn_1, btn_2, btn_3）的透明度
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(obj); i++) {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }
}

/* --------------------------------------------------------
 *  按钮点击事件回调：负责触发动画
 * -------------------------------------------------------- */
void btn_4_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_anim_t a;
        lv_anim_init(&a);
        
        // 设置动画的目标对象为你的容器 cont_1
        lv_anim_set_var(&a, guider_ui.screen_cont_1); 
        lv_anim_set_exec_cb(&a, cont_1_anim_cb);
        
        // 动画持续时间，200毫秒
        lv_anim_set_time(&a, 200);

        if(!is_expanded) {
            // 当前是收起状态 -> 执行展开动画 (透明度/映射值从 0 变到 256)
            lv_anim_set_values(&a, 0, 256);
            is_expanded = true;
        } else {
            // 当前是展开状态 -> 执行收起动画 (透明度/映射值从 256 变到 0)
            lv_anim_set_values(&a, 256, 0);
            is_expanded = false;
        }
        
        // 启动动画
        lv_anim_start(&a);
    }
}

/* --------------------------------------------------------
 *  初始化绑定函数：将点击事件绑定给 btn_4
 * -------------------------------------------------------- */
void custom_init(lv_ui *ui)
{
    /* 手动绑定 btn_4 的点击事件 */
    /* 注：请确保屏幕初始化后调用了 custom_init(&guider_ui) */
    lv_obj_add_event_cb(ui->screen_btn_4, btn_4_event_cb, LV_EVENT_CLICKED, NULL);
}