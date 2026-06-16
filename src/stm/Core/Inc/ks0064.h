/**
 ******************************************************************************
 * @file    ks0064.h
 * @brief   Driver for Keyestudio KS0064 8x8 LED Matrix (HT16K33)
 *
 * Two devices on the same I2C bus, each with a distinct address.
 * Addresses depend on the A0/A1/A2 jumper configuration on the board.
 *
 * HAL uses 8-bit addresses:
 *   7-bit 0x70  →  8-bit 0xE0   (A2=A1=A0 = 0)
 *   7-bit 0x71  →  8-bit 0xE2   (A0 = 1)
 *
 * Adjust KS0064_ADDR_LEFT / KS0064_ADDR_RIGHT to match your boards.
 ******************************************************************************
 */

#ifndef INC_KS0064_H_
#define INC_KS0064_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* -------------------------------------------------------------------------
 * I2C addresses (7-bit).  The driver shifts left by 1 before HAL calls,
 * matching the convention used in main.c (MATRIX_ADDR_1 / MATRIX_ADDR_2).
 * -------------------------------------------------------------------------*/
#define KS0064_ADDR_LEFT    0x71U   /* MATRIX_ADDR_1 – confirmed by I2C scan */
#define KS0064_ADDR_RIGHT   0x74U   /* MATRIX_ADDR_2 – confirmed by I2C scan */

/* -------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

/**
 * @brief  Initialise a KS0064 module.
 *         Enables the oscillator, turns on the display at maximum brightness
 *         and clears all LEDs.
 * @param  hi2c  Pointer to the I2C handle.
 * @param  addr  8-bit I2C address of the device.
 */
void KS0064_Init(I2C_HandleTypeDef *hi2c, uint8_t addr);

/**
 * @brief  Turn all 64 LEDs on or off.
 * @param  hi2c  Pointer to the I2C handle.
 * @param  addr  8-bit I2C address of the device.
 * @param  on    1 = all LEDs on, 0 = all LEDs off.
 */
void KS0064_SetAll(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t on);

/**
 * @brief  Set display brightness.
 * @param  hi2c   Pointer to the I2C handle.
 * @param  addr   8-bit I2C address of the device.
 * @param  level  Brightness 0 (dim) … 15 (max).
 */
void KS0064_SetBrightness(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t level);

/**
 * @brief  Turn the display on or off using a single-byte command.
 *         Requires the RAM to be pre-loaded with the desired pattern (done by
 *         KS0064_Init). Much shorter I2C transaction than KS0064_SetAll.
 * @param  hi2c  Pointer to the I2C handle.
 * @param  addr  7-bit I2C address of the device.
 * @param  on    1 = display on (shows pre-loaded pattern), 0 = display off.
 * @return HAL status – check for HAL_OK to detect I2C bus failures.
 */
HAL_StatusTypeDef KS0064_SetDisplay(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t on);

#endif /* INC_KS0064_H_ */
