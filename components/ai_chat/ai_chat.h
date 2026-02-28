#ifndef AI_CHAT_H   // 修改这里
#define AI_CHAT_H   // 修改这里

// ... 内容 ...
#include "gui_guider.h"

extern lv_ui guider_ui;

#include "esp_err.h"
typedef struct {
    const char* name;
    const char* url;
    const char* key;
    const char* model;
    const char* welcome_msg;
} ai_config_t;

void ai_chat_set_config(ai_config_t conf);
// 现有的函数声明保持不变...
typedef void (*ai_stream_cb_t)(const char *fragment);
void ai_chat_start_with_prompt(const char *current_prompt);
void ai_set_stream_callback(ai_stream_cb_t cb);
void my_ai_stream_cb(const char *fragment);
// 只需要传入你想说的话即可
esp_err_t ai_chat_request(const char *prompt);

void ai_chat_start(void);

#endif 
