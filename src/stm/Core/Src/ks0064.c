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
#define I2C_TIMEOUT_MS               50U    /* I2C2 is dedicated to matrices – no bus contention */

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

    /* 2. Set maximum brightness */
    KS0064_SetBrightness(hi2c, addr, 15);

    /* 3. Pre-load RAM with all-on pattern while display is still off (no visual
     *    glitch). KS0064_SetDisplay(on=1) will show this pattern instantly with
     *    a single 1-byte command instead of re-writing 17 bytes each time. */
    KS0064_SetAll(hi2c, addr, 1);

    /* 4. Display off – KS0064_SetDisplay controls on/off at runtime */
    cmd = 0x80U;
    HAL_I2C_Master_Transmit(hi2c, hal_addr, &cmd, 1, I2C_TIMEOUT_MS);
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

HAL_StatusTypeDef KS0064_SetDisplay(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t on)
{
    /* When turning on: always re-write the full all-on RAM pattern before
     * enabling the display.  The pre-load-once strategy was an optimisation
     * for a shared I2C bus (SRF08 on I2C1), but the matrices now live on a
     * dedicated I2C2 bus so the extra ~1.5 ms write is free.  Without this,
     * any I2C glitch or chip reset between KS0064_Init and this call leaves
     * stale / partial RAM in the HT16K33, causing only a few dots to appear
     * instead of all 64 LEDs lighting up. */
    if (on)
    {
        KS0064_SetAll(hi2c, addr, 1);
    }
    uint8_t cmd = on ? HT16K33_CMD_DISPLAY_ON : 0x80U;
    return HAL_I2C_Master_Transmit(hi2c, (uint16_t)(addr << 1), &cmd, 1, I2C_TIMEOUT_MS);
}
