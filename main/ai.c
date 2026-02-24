#include "ai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h" // 用于 HTTPS 证书验证
#include "cJSON.h"
#include <string.h>
#include <stdio.h>

// ==================== AI 聊天功能配置 ====================
// 这里以 DeepSeek 为例，你可以换成通义千问、Kimi等任何兼容格式的 API
#define AI_API_URL      "https://open.bigmodel.cn/api/paas/v4/chat/completions"
#define AI_API_KEY      "2023c448090d4e039823d4ea20bdd2b2.MXBC7gD7HZ0UU8jX" // 替换为你的真实 API Key
#define AI_MODEL_NAME   "glm-4-flash"     // 模型名称

#define AI_RX_BUFF_LEN  4096 // AI回复可能较长，分配一个大点的接收缓存
static const char *TAG_AI = "ai_chat";
static uint8_t ai_rx_buff[AI_RX_BUFF_LEN];
static int ai_rx_size = 0;

// HTTP 接收回调函数
static esp_err_t _ai_http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA: {
            int copy_len = 0;
            if (evt->data_len > AI_RX_BUFF_LEN - ai_rx_size - 1) { // 留1字节给 '\0'
                copy_len = AI_RX_BUFF_LEN - ai_rx_size - 1;
            } else {
                copy_len = evt->data_len;
            }
            if (copy_len > 0) {
                memcpy(&ai_rx_buff[ai_rx_size], evt->data, copy_len);
                ai_rx_size += copy_len;
            }
            break;
        }
        default:
            break;
    }
    return ESP_OK;
}

// 解析 AI 返回的 JSON
static esp_err_t parse_ai_response(const char *json_data, char *out_reply, size_t max_len) {
    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        ESP_LOGE(TAG_AI, "AI JSON Parse Failed");
        return ESP_FAIL;
    }

    // 标准 OpenAI 格式: root -> choices[0] -> message -> content
    cJSON *choices = cJSON_GetObjectItem(root, "choices");
    if (choices && cJSON_IsArray(choices)) {
        cJSON *choice_0 = cJSON_GetArrayItem(choices, 0);
        if (choice_0) {
            cJSON *message = cJSON_GetObjectItem(choice_0, "message");
            if (message) {
                cJSON *content = cJSON_GetObjectItem(message, "content");
                if (content && content->valuestring) {
                    snprintf(out_reply, max_len, "%s", content->valuestring);
                    cJSON_Delete(root);
                    return ESP_OK;
                }
            }
        }
    }

    ESP_LOGE(TAG_AI, "Invalid AI JSON format or API error.");
    cJSON_Delete(root);
    return ESP_FAIL;
}

// 发起 AI 对话请求
esp_err_t ai_chat_request(const char *prompt, char *out_reply, size_t max_len) {
    esp_err_t err = ESP_FAIL;

    // 1. 使用 cJSON 构建 POST 请求的 Body
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", AI_MODEL_NAME);

    cJSON *messages = cJSON_CreateArray();
    cJSON *msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "role", "user");
    cJSON_AddStringToObject(msg, "content", prompt);
    cJSON_AddItemToArray(messages, msg);

    cJSON_AddItemToObject(root, "messages", messages);

    char *post_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    ESP_LOGI(TAG_AI, "Sending request to AI...");

    // 2. 配置 HTTP 客户端 (HTTPS 需要验证证书)
    esp_http_client_config_t config = {
            .url = AI_API_URL,
            .event_handler = _ai_http_event_handler,
            .timeout_ms = 30000, // AI 回复较慢，设置30秒超时
            .crt_bundle_attach = esp_crt_bundle_attach, // 支持 HTTPS 证书验证
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 3. 设置 POST 方法和 HTTP 头
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    char auth_header[128];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", AI_API_KEY);
    esp_http_client_set_header(client, "Authorization", auth_header);

    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // 4. 清空接收缓存并执行请求
    memset(ai_rx_buff, 0, AI_RX_BUFF_LEN);
    ai_rx_size = 0;

    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ai_rx_buff[ai_rx_size] = '\0';
        if (parse_ai_response((char *)ai_rx_buff, out_reply, max_len) == ESP_OK) {
            ESP_LOGI(TAG_AI, "AI Answer Parsed Successfully.");
        } else {
            err = ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG_AI, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    free(post_data);
    esp_http_client_cleanup(client);

    return err;
}

// 独立的 AI 测试任务 (避免阻塞主线程或回调函数)
static void ai_test_task(void *param) {
    char ai_reply[1024]; // 存放回复
    const char *my_question = "你好，请用一句话介绍一下你自己。";

    ESP_LOGI(TAG_AI, "Me: %s", my_question);

    esp_err_t err = ai_chat_request(my_question, ai_reply, sizeof(ai_reply));
    if (err == ESP_OK) {
        ESP_LOGI(TAG_AI, "AI: %s", ai_reply);
        // 如果你需要把文字显示在 LVGL 屏幕上，可以在这里调用 UI 更新函数
        // 例: update_ai_label(&guider_ui, ai_reply);
    } else {
        ESP_LOGE(TAG_AI, "AI Chat Failed.");
    }

    vTaskDelete(NULL);
}

void ai_chat_start(void) {
    // 栈空间设为 8192，HTTPS 握手较消耗内存
    xTaskCreatePinnedToCore(ai_test_task, "ai_task", 8192, NULL, 3, NULL, 1);
}