/**
  ******************************************************************************
  * @file    lcd1602.h
  * @brief   Driver for Waveshare LCD1602 RGB Module I2C
  ******************************************************************************
  */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* I2C Addresses (8-bit) */
#define LCD_ADDRESS     0x7C
#define RGB_ADDRESS     0xC0

/* LCD Commands */
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET    0x20
#define LCD_SETDDRAMADDR   0x80
#define LCD_SETCGRAMADDR   0x40

/* Function Prototypes */
void LCD1602_Init(I2C_HandleTypeDef *hi2c);
void LCD1602_Clear(void);
void LCD1602_SetCursor(uint8_t col, uint8_t row);
void LCD1602_Print(const char *str);
void LCD1602_SetRGB(uint8_t r, uint8_t g, uint8_t b);

/* Display Update Functions */
void LCD1602_UpdateDisplay(float temp, float humidity, uint16_t tof_dist, float speed);
void LCD1602_UpdateLine1(float temp, float humidity);
void LCD1602_UpdateLine2(uint16_t tof_dist, float speed);

#endif /* INC_LCD1602_H_ */
