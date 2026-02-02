/*
 * hts221.h
 * Driver for HTS221 Humidity and Temperature Sensor
 * Created on: Dec 23, 2025
 * Author: rcosta-c
 */

#ifndef HTS221_H
#define HTS221_H

#ifdef UNIT_TESTING
/* In unit tests include the lightweight HAL mock so we don't pull platform headers */
#include "stm32u5xx_hal.h"
#else
#include "main.h"
#endif

/* I2C Address (8-bit write address) */
#define HTS221_I2C_ADDR      0xBE

/* Registers */
#define HTS221_WHO_AM_I      0x0F
#define HTS221_CTRL_REG1     0x20
#define HTS221_STATUS_REG    0x27
#define HTS221_HUMIDITY_OUT_L 0x28
#define HTS221_TEMP_OUT_L    0x2A

/* Calibration Registers */
#define HTS221_H0_RH_X2      0x30
#define HTS221_H1_RH_X2      0x31
#define HTS221_T0_DEGC_X8    0x32
#define HTS221_T1_DEGC_X8    0x33
#define HTS221_T1_T0_MSB     0x35
#define HTS221_H0_T0_OUT     0x36
#define HTS221_H1_T0_OUT     0x3A
#define HTS221_T0_OUT        0x3C
#define HTS221_T1_OUT        0x3E

#define HTS221_ID            0xBC

/* Struct to hold calibration data */
typedef struct {
    int16_t H0_rH_x2;
    int16_t H1_rH_x2;
    int16_t T0_degC_x8;
    int16_t T1_degC_x8;
    int16_t H0_T0_OUT;
    int16_t H1_T0_OUT;
    int16_t T0_OUT;
    int16_t T1_OUT;
} HTS221_Calibration_t;

/* Function Prototypes */
HAL_StatusTypeDef HTS221_Init(void);
HAL_StatusTypeDef HTS221_ReadHumidity(float *humidity);
HAL_StatusTypeDef HTS221_ReadTemperature(float *temperature);



#endif
