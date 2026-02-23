#ifndef __ST7735_H__
#define __ST7735_H__

#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "fonts.h"

// --- 引脚配置 ---
#define ST7735_SPI_HOST    SPI2_HOST
#define ST7735_PIN_MOSI    23   // SDA
#define ST7735_PIN_CLK     18   // SCL
#define ST7735_PIN_CS      5    // CS
#define ST7735_PIN_DC      16   // DC/RS
#define ST7735_PIN_RST     17   // RES

// --- 1. MADCTL 控制位定义 ---
#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08  // 如果颜色红蓝反转，请将下面旋转逻辑里的 RGB 改为 BGR
#define ST7735_MADCTL_MH  0x04

// --- 2. 旋转逻辑 ---
#define ST7735_ROTATION_ANGLE 90

#if ST7735_ROTATION_ANGLE == 0
#define ST7735_WIDTH  128
    #define ST7735_HEIGHT 160
    #define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB)
#elif ST7735_ROTATION_ANGLE == 90
#define ST7735_WIDTH  160
    #define ST7735_HEIGHT 128
    #define ST7735_ROTATION (ST7735_MADCTL_MV | ST7735_MADCTL_MY | ST7735_MADCTL_RGB)
#elif ST7735_ROTATION_ANGLE == 180
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_ROTATION (ST7735_MADCTL_RGB)
#elif ST7735_ROTATION_ANGLE == 270
#define ST7735_WIDTH  160
    #define ST7735_HEIGHT 128
    #define ST7735_ROTATION (ST7735_MADCTL_MV | ST7735_MADCTL_MX | ST7735_MADCTL_RGB)
#else
    #error "Unsupported rotation angle"
#endif

// 指令定义
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A

// 颜色定义 (RGB565)
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

// 函数声明
void ST7735_Unselect(void);
void ST7735_Init(void);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7735_Flush(int x1, int y1, int x2, int y2, void *data);

#endif