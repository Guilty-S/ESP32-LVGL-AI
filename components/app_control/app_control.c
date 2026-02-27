#include "app_control.h"
#include <stdbool.h>
#include "ap_wifi.h"
#include "ai_chat.h"
#include <driver/gpio.h>
#include <esp_spiffs.h>
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "lvgl.h"
#include "my_button.h"
// 1. 定义预设问题库
static const char *my_questions[] = {
        "你是什么模型.",
        "讲一个故事.",
        "解释一下ESP32.",
        "关于月球.",
        "关于LVGL?"
};
static char ai_buffer[512];
static int ai_buf_pos = 0;
static const char* TAG = "app_control";
int get_button_level(int gpio) {
    return gpio_get_level(gpio);
}

void long_press(int gpio) {
    ap_wifi_apcfg(true);
}

// 原有的短按回调（GPIO 33）：负责拼接要求并发送
void short_press(int gpio) {
    if (lvgl_port_lock(portMAX_DELAY)) {
        // 1. 获取当前屏幕上显示的问题文字 (注意：这是只读的)
        const char *original_prompt = lv_textarea_get_text(guider_ui.screen_label_ask);

        // 2. 清空回答区域，准备显示
        lv_textarea_set_text(guider_ui.screen_label_answer, "");

        // ================= 新增拼接逻辑 =================
        // 定义你要追加的提示词
        const char *append_str = "，请控制在50字以内。";

        // 计算需要的内存大小：原字符串长度 + 追加字符串长度 + 1 (结束符 '\0')
        size_t needed_size = strlen(original_prompt) + strlen(append_str) + 1;

        // 动态分配一块新内存用来存拼接后的完整句子
        char *combined_prompt = (char *) malloc(needed_size);

        if (combined_prompt != NULL) {
            // 用 sprintf 将两者拼接到 combined_prompt 中
            sprintf(combined_prompt, "%s%s", original_prompt, append_str);

            // 3. 启动 AI（把拼接好的新字符串传进去）
            ai_chat_start_with_prompt(combined_prompt);

            // 4. 发送完毕后，释放我们刚刚申请的内存
            // (因为你的 ai_chat_start_with_prompt 内部有 strdup 拷贝，所以这里安全释放)
            free(combined_prompt);
        } else {
            ESP_LOGE(TAG, "Malloc failed for combined prompt!");
        }
        // ==============================================

        // 5. 操作完毕，解锁 LVGL
        lvgl_port_unlock();
    }
}
static int question_idx = 0;

// 2. 新按键的回调：仅负责切换并显示问题
void switch_question_press(int gpio) {
    question_idx = (question_idx + 1) % 5; // 5个问题循环

    if (lvgl_port_lock(portMAX_DELAY)) {
        // 在新增加的 textarea (或label) 上显示当前选中的问题
        // 假设你在 GUI Guider 里的名字叫 label_ask
        lv_textarea_set_text(guider_ui.screen_label_ask, my_questions[question_idx]);
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Switched to Question: %s", my_questions[question_idx]);
}
void button_init(void) {
    gpio_config(&(gpio_config_t) {
            .pin_bit_mask = (1ULL << 33) | (1ULL << 25),
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
                    .short_cb = short_press,
            };
    button_event_set(&button_cfg);
    // 新增 GPIO 25 的按键配置 (用于切换任务)
    button_config_t switch_btn_cfg = {
            .active_level = 0,
            .getlevel_cb = get_button_level,
            .gpio_num = 25,  // <--- 你的新按键引脚
            .short_cb = switch_question_press, // 绑定切换函数
    };
    button_event_set(&switch_btn_cfg);
}

void img_spiffs_init(void) {
    //定义挂载点
    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/img",            //挂载点
            .partition_label = "img",         //分区名称
            .max_files = 5,                    //最大打开的文件数
            .format_if_mount_failed = false    //挂载失败是否执行格式化
    };
    //挂载spiffs
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
}

