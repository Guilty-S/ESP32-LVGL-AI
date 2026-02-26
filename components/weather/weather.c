#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "weather.h"
#include "wifi_manager.h"
#include <string.h>
#include <stdio.h>
static weather_update_cb_t g_weather_ui_cb = NULL; // 存放回调函数的指针
// 实现设置回调的函数
void weather_set_ui_callback(weather_update_cb_t cb) {
    g_weather_ui_cb = cb;
}

#define WEATHER_BUFF_LEN 2048
#define WEATHER_PRIVATE_KEY "SETYVdDsalvhmIpVf"
#define WEATHER_LOCATION "WSSU6EXX52RE"
static const char *TAG = "weather";
static uint8_t weather_data_buff[WEATHER_BUFF_LEN];
static int weather_data_size = 0;
static char city_name[48];
static char region_name[48];

static esp_err_t _weather_http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA: {
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//            printf("HTTP_EVENT_ON_DATA data=%.*s\r\n", evt->data_len, (char *) evt->data);
            int copy_len = 0;
            //因为这个HTTP_EVENT_ON_DATA事件可能会不只一次的触发，也就是
            //会分多次接收，因此这里需要考虑多次接收数据的完整
            //weather_data_size会记录数据的长度
            //weather_data_buff则保存了收到的所有http数据
            if (evt->data_len > WEATHER_BUFF_LEN - weather_data_size) {
                copy_len = WEATHER_BUFF_LEN - weather_data_size;
            } else {
                copy_len = evt->data_len;
            }
            memcpy(&weather_data_buff[weather_data_size], evt->data, copy_len);
            weather_data_size += copy_len;
            break;
        }
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
        default:
            break;
    }
    return ESP_OK;
}

static esp_err_t pasre_weather(const char *weather_data) {
    weather_data_pkt_t data[3];
    char city[48];
    cJSON *wj_js = cJSON_Parse(weather_data);
    if (!wj_js) {
        ESP_LOGE(TAG, "Invalid JSON format");
        return ESP_FAIL;
    }
    cJSON *results = cJSON_GetObjectItem(wj_js, "results");
    if (!results) {
        ESP_LOGE(TAG, "Invalid result");
        return ESP_FAIL;
    }
    cJSON *result_child_js = results->child;
    cJSON *location_js = cJSON_GetObjectItem(result_child_js, "location");
    cJSON *name_js = cJSON_GetObjectItem(location_js, "name");
    if (name_js) {
        snprintf(city, sizeof(city), "%s", cJSON_GetStringValue(name_js));
    }
    cJSON *daily_js = cJSON_GetObjectItem(result_child_js, "daily");
    int index = 0;
    if (daily_js) {
        cJSON *daily_child_js = daily_js->child;
        while (daily_child_js) {
            cJSON *high_js = cJSON_GetObjectItem(daily_child_js, "high");
            cJSON *low_js = cJSON_GetObjectItem(daily_child_js, "low");
            cJSON *code_js = cJSON_GetObjectItem(daily_child_js, "code_day");
            if (index < 3) {
                sscanf(cJSON_GetStringValue(high_js), "%d", &data[index].high_temp);
                sscanf(cJSON_GetStringValue(low_js), "%d", &data[index].low_temp);
                snprintf(data[index].code, sizeof(data[index].code), "%s", cJSON_GetStringValue(code_js));
                index++;
                daily_child_js = daily_child_js->next;
            }
        }
        char img_path[32];
        snprintf(img_path, sizeof(img_path), "/img/%s@1x.png", data[index].code);
        if (g_weather_ui_cb) {
                g_weather_ui_cb(index, img_path, data[index].low_temp, data[index].high_temp);
        }
    }
    cJSON_Delete(wj_js);
    return ESP_OK;
}

static esp_err_t weather_http_connect(void) {
    static char url[256];
    snprintf(url, sizeof(url),
             "http://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s%s&language=zh-Hans&unit=c&start=0&days=3",
             WEATHER_PRIVATE_KEY, region_name, city_name);
    esp_http_client_config_t config =
            {
                    .url = url,
                    .event_handler = _weather_http_event_handler,
                    .timeout_ms = 10000, // 设置为 10 秒
            };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    memset(weather_data_buff, 0, WEATHER_BUFF_LEN);
    weather_data_size = 0;
    //此函数将完成一个完整HTTP请求才返回
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        weather_data_buff[weather_data_size] = '\0'; // 必须加结束符
        pasre_weather((char *) weather_data_buff);   // 只在这里解析一次
        ESP_LOGI(TAG, "Full data received: %s", (char *)weather_data_buff);
    } else {
        ESP_LOGE(TAG, "Weather request failed: %s", esp_err_to_name(err));
    }
    //释放掉HTTP资源
    esp_http_client_cleanup(client);
    return err;
}

static esp_err_t pasre_location(char *location_data) {
    cJSON *location_js = cJSON_Parse(location_data);
    if (!location_js) {
        ESP_LOGI(TAG, "Invaild location json");
        return ESP_FAIL;
    }
    cJSON *city_js = cJSON_GetObjectItem(location_js, "city");
    if (!city_js) {
        ESP_LOGI(TAG, "Invaild location city");
        return ESP_FAIL;
    }
    cJSON *region_js = cJSON_GetObjectItem(location_js, "regionName");
    if (!region_js) {
        ESP_LOGI(TAG, "Invaild location region");
        return ESP_FAIL;
    }
    snprintf(city_name, sizeof(city_name), "%s", cJSON_GetStringValue(city_js));
    snprintf(region_name, sizeof(region_name), "%s", cJSON_GetStringValue(region_js));
    //地理位置保存在city_name中
    ESP_LOGI(TAG, "location->city name:%s", city_name);
    ESP_LOGI(TAG, "location->region name:%s", region_name);
    cJSON_Delete(location_js);
    return ESP_OK;
}

/** 发起HTTP请求，获取当前地理位置（城市）
 * @param 无
 * @return 错误
*/
static esp_err_t location_http_connect(void) {
    static char url[256];
    snprintf(url, sizeof(url),
             "http://ip-api.com/json/?lang=en");
    esp_http_client_config_t config =
            {
                    .url = url,
                    .event_handler = _weather_http_event_handler,
                    .timeout_ms = 10000, // 设置为 10 秒

            };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    memset(weather_data_buff, 0, WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err = esp_http_client_perform(client);
    pasre_location((char *) weather_data_buff);
    esp_http_client_cleanup(client);
    return err;
}

static void weather_task(void *param) {
    while (1) {
        //先判断一下网络是否已经连接了，如果没有连接循环2秒等待
        if (!wifi_manager_is_connect()) {
            vTaskDelay(pdMS_TO_TICKS(2000));
        } else    //发现已经连接上网络
        {
            //先根据ip地址请求地理位置
            location_http_connect();
            //然后获取天气信息
            weather_http_connect();
            //延时半小时
            vTaskDelay(pdMS_TO_TICKS(1000 * 1800));
        }
    }
}

/** 启动天气信息状况定时获取
 * @param 无
 * @return 无
*/
void weather_start(void) {
    //这里会新建一个任务，定时获取天气状况信息
    xTaskCreatePinnedToCore(weather_task, "weather", 4096, NULL, 2, NULL, 1);
}
