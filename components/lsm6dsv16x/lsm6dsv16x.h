#ifndef _LSM6DSV16X_APP_H_
#define _LSM6DSV16X_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include "esp_err.h" /* 包含 ESP-IDF 错误类型定义 */

/* ================= 全局变量声明 ================= */

/*
 * 陀螺仪和加速度计原始数据
 * int16_t 对应 STM32 代码中的 short
 */
extern int16_t gyro[3];
extern int16_t accel[3];

/* 解算后的欧拉角 */
extern float pitch, roll, yaw;

/*
 * 中断标志位
 * 必须加 volatile，因为该变量会在中断服务函数(ISR)中被修改
 */
extern volatile uint8_t mems_event_detected;

/* ================= 函数接口声明 ================= */

/**
 * @brief 初始化 ESP32 的 I2C 总线
 * @return ESP_OK 成功, 其他值失败
 */
esp_err_t LSM_Init_I2C_Bus(void);
void lsm6dsv16x_gpio_init(void);
/**
 * @brief 初始化 LSM6DSV16X 传感器 (复位、设置量程、ODR、FIFO等)
 * @return 0: 成功, 1: 通信失败, 2: ID不匹配
 */
uint8_t LSM6DSV16X_Init(void);

/**
 * @brief 读取 FIFO 数据并进行姿态解算
 *        通常在 mems_event_detected 置位时调用
 */
void Read_LSM6DSV16X(void);

#ifdef __cplusplus
}
#endif

#endif  /* _LSM6DSV16X_APP_H_ */