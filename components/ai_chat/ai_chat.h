#ifndef _AI_H_
#define _AI_H_

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

void ai_set_stream_callback(ai_stream_cb_t cb);

// 只需要传入你想说的话即可
esp_err_t ai_chat_request(const char *prompt);

void ai_chat_start(void);

#endif 
