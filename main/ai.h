#ifndef _AI_H_
#define _AI_H_

#include "esp_err.h"

// 只需要传入你想说的话即可
esp_err_t ai_chat_request(const char *prompt);

void ai_chat_start(void);

#endif // _AI_H_