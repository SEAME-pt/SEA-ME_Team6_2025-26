/**
 ******************************************************************************
 * @file    ks0064.c
 * @brief   Driver for Keyestudio KS0064 8x8 LED Matrix (HT16K33)
 ******************************************************************************
 */

#include "ks0064.h"

/* -------------------------------------------------------------------------
 * HT16K33 command bytes
 * -------------------------------------------------------------------------*/
#define HT16K33_CMD_OSCILLATOR_ON   0x21U   /* System setup – oscillator on   */
#define HT16K33_CMD_DISPLAY_ON      0x81U   /* Display on, no blink           */
#define HT16K33_CMD_BRIGHTNESS_BASE 0xE0U   /* Dimming set – OR with 0..15    */
#define HT16K33_CMD_RAM_START       0x00U   /* Start address of display RAM   */

#define HT16K33_RAM_BYTES           16U     /* 8 rows × 2 bytes/row           */
#define I2C_TIMEOUT_MS              100U

/* -------------------------------------------------------------------------
 * Public functions
 * -------------------------------------------------------------------------*/

void KS0064_Init(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
    uint8_t cmd;
    uint16_t hal_addr = (uint16_t)(addr << 1);

    /* 1. Enable internal oscillator */
    cmd = HT16K33_CMD_OSCILLATOR_ON;
    HAL_I2C_Master_Transmit(hi2c, hal_addr, &cmd, 1, I2C_TIMEOUT_MS);

    /* 2. Turn on display, no blink */
    cmd = HT16K33_CMD_DISPLAY_ON;
    HAL_I2C_Master_Transmit(hi2c, hal_addr, &cmd, 1, I2C_TIMEOUT_MS);

    /* 3. Set maximum brightness */
    KS0064_SetBrightness(hi2c, addr, 15);

    /* 4. Clear all LEDs */
    KS0064_SetAll(hi2c, addr, 0);
}

void KS0064_SetAll(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t on)
{
    /*
     * HT16K33 RAM layout: 8 rows × 2 bytes each (16 bytes total).
     * For an 8×8 matrix: even byte = LED columns 0-7, odd byte = reserved (0x00).
     * Matches the format used in matrix_all_on() / matrix_clear() in main.c.
     */
    uint8_t buf[1 + HT16K33_RAM_BYTES];
    uint8_t fill = on ? 0xFFU : 0x00U;

    buf[0] = HT16K33_CMD_RAM_START;
    for (uint8_t row = 0; row < 8U; row++)
    {
        buf[1U + (row * 2U)]      = fill;   /* LED data byte  */
        buf[1U + (row * 2U) + 1U] = 0x00U; /* reserved byte  */
    }

    HAL_I2C_Master_Transmit(hi2c, (uint16_t)(addr << 1), buf, sizeof(buf), I2C_TIMEOUT_MS);
}

void KS0064_SetBrightness(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t level)
{
    if (level > 15U)
    {
        level = 15U;
    }
    uint8_t cmd = HT16K33_CMD_BRIGHTNESS_BASE | level;
    HAL_I2C_Master_Transmit(hi2c, (uint16_t)(addr << 1), &cmd, 1, I2C_TIMEOUT_MS);
}
