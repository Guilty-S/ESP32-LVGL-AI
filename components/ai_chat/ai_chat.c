#include "ai_chat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include "esp_lvgl_port.h"
// #define AI_API_URL      "http://1.95.142.151:3000/v1/chat/completions"
// #define AI_API_KEY      "sk-WQYbcQdw9N3l7JMnBN4i5c4mqSLjEyfHg4MJevYbMWQC5tIe" // claude
// #define AI_MODEL_NAME   "claude-3-5-sonnet-20240620"
// #define AI_API_URL      "https://open.bigmodel.cn/api/paas/v4/chat/completions"
// #define AI_API_KEY      "2023c448090d4e039823d4ea20bdd2b2.MXBC7gD7HZ0UU8jX" // 暂时代替chatgpt
// #define AI_MODEL_NAME   "glm-4-flash"
// #define AI_API_URL      "https://open.bigmodel.cn/api/paas/v4/chat/completions"
// #define AI_API_KEY      "2023c448090d4e039823d4ea20bdd2b2.MXBC7gD7HZ0UU8jX" // glm
// #define AI_MODEL_NAME   "glm-4-flash"
// #define AI_API_URL      "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
// #define AI_API_KEY      "sk-68ff9e5765c44b46ace7aa21fa747812" // qwen
// #define AI_MODEL_NAME   "qwen-flash-2025-07-28"

static const char *TAG_AI = "ai_chat";
static ai_config_t current_conf = {
    .name = "Claude",
    .url = "http://1.95.142.151:3000/v1/chat/completions",
    .key = "sk-WQYbcQdw9N3l7JMnBN4i5c4mqSLjEyfHg4MJevYbMWQC5tIe",
    .model = "claude-3-5-sonnet-20240620",
    .welcome_msg = "Hi! I am Claude 3.5."
};

static char ai_buffer[512];
static int ai_buf_pos = 0;
void ai_chat_set_config(ai_config_t conf) {
    current_conf = conf;
}
static ai_stream_cb_t g_ai_stream_cb = NULL; // 保存界面的回调函数
void ai_set_stream_callback(ai_stream_cb_t cb) {
    g_ai_stream_cb = cb;
}


// 行缓冲区，用于拼接流式输出被截断的数据包
static char line_buffer[2048];
static int line_len = 0;

// HTTP 接收回调函数（流式专属解析核心）
static esp_err_t _ai_http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA: {
            // 服务器每次发来一个片段，可能包含多个字符或者不完整的行
            for (int i = 0; i < evt->data_len; i++) {
                char c = ((char *) evt->data)[i];
                if (c == '\n') {
                    line_buffer[line_len] = '\0'; // 形成完整的一行

                    // 判断是否是 AI 发来的有效数据行 (Server-Sent Events 格式)
                    if (strncmp(line_buffer, "data: ", 6) == 0) {
                        // 如果遇到结束标志，直接退出
                        if (strstr(line_buffer, "[DONE]")) {
//                            printf("\n"); // 换行收尾
                            if (g_ai_stream_cb) g_ai_stream_cb("\n");
                            break;
                        }

                        // 只解析 data: 后面的 JSON 字符串
                        cJSON *root = cJSON_Parse(line_buffer + 6);
                        if (root) {
                            cJSON *choices = cJSON_GetObjectItem(root, "choices");
                            if (choices && cJSON_IsArray(choices)) {
                                cJSON *choice = cJSON_GetArrayItem(choices, 0);
                                cJSON *delta = cJSON_GetObjectItem(choice, "delta");
                                cJSON *content = cJSON_GetObjectItem(delta, "content");

                                // 如果解析到了字，立刻打印出来！
                                if (content && content->valuestring) {
//                                    printf("%s", content->valuestring);
//                                    fflush(stdout); // 强制立刻输出到串口
                                    if (g_ai_stream_cb) {
                                        g_ai_stream_cb(content->valuestring);
                                    }
                                }
                            }
                            cJSON_Delete(root);
                        }
                    }
                    line_len = 0; // 一行解析完，清空缓冲区准备接下一行
                } else {
                    // 如果没遇到换行符，继续存入行缓冲区
                    if (line_len < sizeof(line_buffer) - 1) {
                        line_buffer[line_len++] = c;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    return ESP_OK;
}

// 发起 AI 对话请求
esp_err_t ai_chat_request(const char *prompt) {
    esp_err_t err = ESP_FAIL;

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", current_conf.model);

    // 【核心秘诀】：告诉 AI 开启流式输出！
    cJSON_AddBoolToObject(root, "stream", true);

    cJSON *messages = cJSON_CreateArray();
    cJSON *msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "role", "user");
    cJSON_AddStringToObject(msg, "content", prompt);
    cJSON_AddItemToArray(messages, msg);
    cJSON_AddItemToObject(root, "messages", messages);

    char *post_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    esp_http_client_config_t config = {
            .url = current_conf.url,
            .event_handler = _ai_http_event_handler,
            .timeout_ms = 30000,
            .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    char auth_header[128];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", current_conf.key);
    esp_http_client_set_header(client, "Authorization", auth_header);

    // 强制关闭 HTTP Keep-Alive，有助于提升单次流式请求的稳定性
    esp_http_client_set_header(client, "Connection", "close");

    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // 清空行缓冲
    memset(line_buffer, 0, sizeof(line_buffer));
    line_len = 0;

    ESP_LOGI(TAG_AI, "Waiting for AI's first token...");

    // 开始执行请求，这里会阻塞，但回调函数会源源不断地把字打印出来
    printf("AI: ");
    err = esp_http_client_perform(client);

    if (err != ESP_OK) {
        ESP_LOGE(TAG_AI, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    free(post_data);
    esp_http_client_cleanup(client);

    return err;
}
void my_ai_stream_cb(const char *fragment) {
    // 1. 先把收到的碎片存进缓冲区，而不是直接画屏幕
    int len = strlen(fragment);
    if (ai_buf_pos + len < sizeof(ai_buffer) - 1) {
        strcpy(ai_buffer + ai_buf_pos, fragment);
        ai_buf_pos += len;
    }
    // 2. 只有当缓冲区攒够了 15 个字符，或者包含换行符时，才去刷新屏幕
    // 这样可以把 100 次每秒的刷新降到 5-6 次，既流畅又不卡顿
    if (ai_buf_pos > 15 || strchr(fragment, '\n')) {
        if (lvgl_port_lock(portMAX_DELAY)) {
            // 一次性把攒好的字加上去
            lv_textarea_add_text(guider_ui.screen_label_answer, ai_buffer);
            // 确保滚动条在最下面
            // (如果是 LVGL 8.3 及以上，通常加字会自动滚，这行可选)
            // lv_obj_scroll_to_view(lv_textarea_get_cursor_pos(guider_ui.screen_ta_1), LV_ANIM_OFF);
            lvgl_port_unlock();
        }
        // 3. 清空缓冲区，准备攒下一波
        ai_buf_pos = 0;
        ai_buffer[0] = '\0';
    }
}
// 独立的 AI 测试任务
static void ai_test_task(void *param) {
    const char *my_question = "你是什么模型？50字介绍(英文)"; // 故意让它多说点测试流式速度

    ESP_LOGI(TAG_AI, "Me: %s", my_question);

    // 调用请求函数，现在不需要 out_reply 缓冲区了，因为它会直接在底层实时打印
    ai_chat_request(my_question);

    vTaskDelete(NULL);
}

// 内部任务函数
static void ai_task_entry(void *param) {
    char *prompt = (char *)param; // 接收传进来的字符串
    
    ESP_LOGI(TAG_AI, "AI Task Started with prompt: %s", prompt);
    ai_chat_request(prompt);

    free(prompt);  // 重要：释放 strdup 分配的内存
    vTaskDelete(NULL); // 任务结束，销毁自己
}

// 供外部调用的启动函数
void ai_chat_start_with_prompt(const char *current_prompt) {
    if (current_prompt == NULL || strlen(current_prompt) == 0) {
        ESP_LOGW(TAG_AI, "Prompt is empty, ignore.");
        return;
    }

    // 重要：必须拷贝字符串！因为 current_prompt 来自 UI 缓存，随时会变
    char *prompt_copy = strdup(current_prompt); 
    
    // 创建任务，将拷贝的字符串作为参数传入
    xTaskCreatePinnedToCore(ai_task_entry, "ai_task", 8192, prompt_copy, 3, NULL, 1);
}
void ai_chat_start(void) {
    ai_chat_start_with_prompt("Introduce yourself.");
}