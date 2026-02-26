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
void weather_start(void);
#endif //ESP32_CLION_WEATHER_H
