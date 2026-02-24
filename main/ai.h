#ifndef _AI_H_
#define _AI_H_

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief 发送文本给AI并获取回复 (阻塞型函数)
 * @param prompt     用户想对AI说的话
 * @param out_reply  用于存放AI回复的缓冲区
 * @param max_len    缓冲区最大长度
 * @return esp_err_t ESP_OK 成功, ESP_FAIL 失败
 */
esp_err_t ai_chat_request(const char *prompt, char *out_reply, size_t max_len);

/**
 * @brief 启动一个独立的AI测试任务
 */
void ai_chat_start(void);

#endif // _AI_H_