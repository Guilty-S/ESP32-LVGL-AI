//
// Created by Administrator on 2026/2/24.
//

#ifndef ESP32_CLION_WEATHER_H
#define ESP32_CLION_WEATHER_H
typedef struct {
    int high_temp;
    int low_temp;
    char code[4];
}weather_data_pkt_t;
// 定义回调函数原型：参数分别为 (第几天, 图标路径, 最低温, 最高温)
// day: 0-今天, 1-明天, 2-后天
typedef void (*weather_update_cb_t)(int day, const char* img_path, int low, int high);

// 提供一个函数，让外部（main.c）把 UI 刷新函数传进来
void weather_set_ui_callback(weather_update_cb_t cb);
void weather_start(void);
#endif //ESP32_CLION_WEATHER_H
