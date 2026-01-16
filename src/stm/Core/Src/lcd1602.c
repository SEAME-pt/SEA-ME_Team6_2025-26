/**
  ******************************************************************************
  * @file    lcd1602.c
  * @brief   Driver implementation for Waveshare LCD1602 RGB Module I2C
  ******************************************************************************
  */

#include "lcd1602.h"
#include <string.h>
#include <stdio.h>

/* Private Variables */
static I2C_HandleTypeDef *lcd_i2c;

/**
  * @brief  Send command to LCD
  * @param  cmd: command byte
  * @retval None
  */
static void LCD_SendCommand(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd}; // Control byte + command
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDRESS, data, 2, 100);
}

/**
  * @brief  Send data to LCD
  * @param  data_byte: data byte
  * @retval None
  */
static void LCD_SendData(uint8_t data_byte)
{
    uint8_t data[2] = {0x40, data_byte}; // RS=1 for data
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDRESS, data, 2, 100);
}

/**
  * @brief  Initialize RGB backlight controller
  * @retval None
  */
static void RGB_Init(void)
{
    uint8_t data[2];

    HAL_Delay(10);

    // Mode1 = 0x00 (normal mode)
    data[0] = 0x00;
    data[1] = 0x00;
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    HAL_Delay(5);

    // Mode2 = 0x00
    data[0] = 0x01;
    data[1] = 0x00;
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    HAL_Delay(5);

    // LEDOUT = 0xFF (all LEDs fully on)
    data[0] = 0x08;
    data[1] = 0xFF;
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    HAL_Delay(5);

    // Force PWM values to maximum (registers 0x02, 0x03, 0x04)
    data[0] = 0x02; data[1] = 0xFF; // Blue
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    data[0] = 0x03; data[1] = 0xFF; // Green
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    data[0] = 0x04; data[1] = 0xFF; // Red
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 2, 100);
    HAL_Delay(10);
}

/**
  * @brief  Initialize LCD1602
  * @param  hi2c: pointer to I2C handle
  * @retval None
  */
void LCD1602_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;
    HAL_Delay(100); // Wait for LCD power-up

    // Function set: 4-bit, 2 lines, 5x8 dots
    LCD_SendCommand(0x28);
    HAL_Delay(5);

    // Display OFF
    LCD_SendCommand(0x08);
    HAL_Delay(5);

    // Clear display
    LCD_SendCommand(0x01);
    HAL_Delay(10);

    // Entry mode: increment, no shift
    LCD_SendCommand(0x06);
    HAL_Delay(5);

    // Display ON, cursor OFF, blink OFF
    LCD_SendCommand(0x0C);
    HAL_Delay(5);

    // Initialize RGB backlight
    RGB_Init();
    HAL_Delay(10);

    // Force backlight white - try multiple times
    LCD1602_SetRGB(255, 255, 255);
    HAL_Delay(10);
    LCD1602_SetRGB(255, 255, 255);
    HAL_Delay(10);
    LCD1602_SetRGB(255, 255, 255);
}

/**
  * @brief  Clear LCD display
  * @retval None
  */
void LCD1602_Clear(void)
{
    LCD_SendCommand(LCD_CLEARDISPLAY);
    HAL_Delay(2);
}

/**
  * @brief  Set cursor position
  * @param  col: column (0-15)
  * @param  row: row (0-1)
  * @retval None
  */
void LCD1602_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t addr = (row == 0) ? col : (0x40 + col);
    LCD_SendCommand(LCD_SETDDRAMADDR | addr);
}

/**
  * @brief  Print string to LCD
  * @param  str: null-terminated string
  * @retval None
  */
void LCD1602_Print(const char *str)
{
    while (*str) {
        LCD_SendData(*str++);
    }
}

/**
  * @brief  Set RGB backlight color
  * @param  r: red intensity (0-255)
  * @param  g: green intensity (0-255)
  * @param  b: blue intensity (0-255)
  * @retval None
  */
void LCD1602_SetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t data[4];
    // Auto-increment from register 0x02
    data[0] = 0x82; // Auto-inc + start at PWM0
    data[1] = b;    // Blue (PWM0)
    data[2] = g;    // Green (PWM1)
    data[3] = r;    // Red (PWM2)
    HAL_I2C_Master_Transmit(lcd_i2c, RGB_ADDRESS, data, 4, 100);
}

/**
  * @brief  Update Line 1 with temperature and humidity
  * @param  temp: temperature in °C
  * @param  humidity: humidity in %
  * @retval None
  */
void LCD1602_UpdateLine1(float temp, float humidity)
{
    char buf[17];  // 16 chars + null terminator

    LCD1602_SetCursor(0, 0);
    snprintf(buf, sizeof(buf), "T:%.1fC H:%.1f%%  ", temp, humidity);
    LCD1602_Print(buf);
}

/**
  * @brief  Update Line 2 with ToF distance and speed
  * @param  tof_dist: distance in mm
  * @param  speed: speed in km/h
  * @retval None
  */
void LCD1602_UpdateLine2(uint16_t tof_dist, float speed)
{
    char buf[17];  // 16 chars + null terminator
    float speed_mh = speed * 1000.0f;  // Convert km/h to m/h

    LCD1602_SetCursor(0, 1);
    if (tof_dist < 1000) {
        // Show distance in mm if less than 1m
        snprintf(buf, sizeof(buf), "D:%dmm S:%.0fm  ", tof_dist, speed_mh);
    } else {
        // Show distance in meters if >= 1m
        snprintf(buf, sizeof(buf), "D:%.1fm S:%.0fm  ", tof_dist / 1000.0f, speed_mh);
    }
    LCD1602_Print(buf);
}

/**
  * @brief  Update display with sensor data (both lines)
  * @param  temp: temperature in °C
  * @param  humidity: humidity in %
  * @param  tof_dist: distance in mm
  * @param  speed: speed in km/h (converted to m/h for display)
  * @retval None
  */
void LCD1602_UpdateDisplay(float temp, float humidity, uint16_t tof_dist, float speed)
{
    LCD1602_UpdateLine1(temp, humidity);
    LCD1602_UpdateLine2(tof_dist, speed);
}
