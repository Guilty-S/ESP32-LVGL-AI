#ifndef CUSTOM_H
#define CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "gui_guider.h"
#include "lvgl.h"

/*********************
 *   GLOBAL FUNCTIONS
 *********************/

/**
 * @brief 初始化自定义UI逻辑
 * 
 * 此函数应在 main.c 的 app_main 中，在 setup_ui(&guider_ui) 之后立即调用。
 * 它负责绑定事件、初始化动画状态、以及禁用不必要的焦点。
 * 
 * @param ui 指向 GUI Guider 生成的全局 UI 结构体 (guider_ui) 的指针
 */
void custom_init(lv_ui *ui);

/**
 * @brief 子按钮（模型切换按钮 btn_1, 2, 3, 5）的事件回调
 */
void child_btn_event_cb(lv_event_t * e);

/**
 * @brief 主按钮（展开/收起按钮 btn_4）的事件回调
 */
void btn_4_event_cb(lv_event_t * e);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CUSTOM_H */