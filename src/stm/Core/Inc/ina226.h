/*
 * ina226.h
 * Driver for INA226 Current/Voltage Monitor
 * Created on: Jan 27, 2026
 * Author: rcosta-c
 *
 * Module specs:
 * - Voltage range: DC 0V ~ 36V
 * - Current range: -20A ~ 20A
 * - Resolution: 16 bits
 * - Accuracy: +/-1%
 */

#ifndef INA226_H
#define INA226_H

#include "main.h"

/* I2C Address (7-bit) - Configured via pads on module */
#define INA226_I2C_ADDR          0x40

/* Register Addresses */
#define INA226_REG_CONFIG        0x00
#define INA226_REG_SHUNT_VOLTAGE 0x01
#define INA226_REG_BUS_VOLTAGE   0x02
#define INA226_REG_POWER         0x03
#define INA226_REG_CURRENT       0x04
#define INA226_REG_CALIBRATION   0x05
#define INA226_REG_MASK_ENABLE   0x06
#define INA226_REG_ALERT_LIMIT   0x07
#define INA226_REG_MANUFACTURER  0xFE
#define INA226_REG_DIE_ID        0xFF

/* Expected ID values */
#define INA226_MANUFACTURER_ID   0x5449  /* "TI" */
#define INA226_DIE_ID            0x2260

/* Configuration Register Bits */
/* Averaging Mode (bits 11-9) */
#define INA226_AVG_1             (0 << 9)
#define INA226_AVG_4             (1 << 9)
#define INA226_AVG_16            (2 << 9)
#define INA226_AVG_64            (3 << 9)
#define INA226_AVG_128           (4 << 9)
#define INA226_AVG_256           (5 << 9)
#define INA226_AVG_512           (6 << 9)
#define INA226_AVG_1024          (7 << 9)

/* Bus Voltage Conversion Time (bits 8-6) */
#define INA226_VBUS_140US        (0 << 6)
#define INA226_VBUS_204US        (1 << 6)
#define INA226_VBUS_332US        (2 << 6)
#define INA226_VBUS_588US        (3 << 6)
#define INA226_VBUS_1100US       (4 << 6)
#define INA226_VBUS_2116US       (5 << 6)
#define INA226_VBUS_4156US       (6 << 6)
#define INA226_VBUS_8244US       (7 << 6)

/* Shunt Voltage Conversion Time (bits 5-3) */
#define INA226_VSHUNT_140US      (0 << 3)
#define INA226_VSHUNT_204US      (1 << 3)
#define INA226_VSHUNT_332US      (2 << 3)
#define INA226_VSHUNT_588US      (3 << 3)
#define INA226_VSHUNT_1100US     (4 << 3)
#define INA226_VSHUNT_2116US     (5 << 3)
#define INA226_VSHUNT_4156US     (6 << 3)
#define INA226_VSHUNT_8244US     (7 << 3)

/* Operating Mode (bits 2-0) */
#define INA226_MODE_POWER_DOWN   0
#define INA226_MODE_SHUNT_TRIG   1
#define INA226_MODE_BUS_TRIG     2
#define INA226_MODE_SHUNT_BUS_TRIG 3
#define INA226_MODE_POWER_DOWN2  4
#define INA226_MODE_SHUNT_CONT   5
#define INA226_MODE_BUS_CONT     6
#define INA226_MODE_SHUNT_BUS_CONT 7

/* Reset bit */
#define INA226_CONFIG_RESET      (1 << 15)

/* Default configuration: 16 averages, 1.1ms conversion, continuous */
#define INA226_CONFIG_DEFAULT    (INA226_AVG_16 | INA226_VBUS_1100US | \
                                  INA226_VSHUNT_1100US | INA226_MODE_SHUNT_BUS_CONT)

/*
 * Calibration for ±20A module with ~4mΩ shunt
 * Current_LSB = 20A / 32768 ≈ 0.61mA, using 1mA for simplicity
 * CAL = 0.00512 / (Current_LSB × Rshunt)
 * CAL = 0.00512 / (0.001 × 0.004) = 1280
 */
#define INA226_CALIBRATION_VALUE 1280
#define INA226_CURRENT_LSB_MA    1.0f    /* 1mA per LSB */

/* LSB values from datasheet */
#define INA226_BUS_VOLTAGE_LSB   1.25f   /* 1.25mV per LSB */
#define INA226_SHUNT_VOLTAGE_LSB 2.5f    /* 2.5µV per LSB */
#define INA226_POWER_LSB         25.0f   /* 25 × Current_LSB per LSB */

/* Data structure for INA226 readings */
typedef struct {
    float voltage_V;        /* Bus voltage in Volts */
    float current_A;        /* Current in Amperes (negative = discharge) */
    float power_W;          /* Power in Watts */
    float shunt_voltage_mV; /* Shunt voltage in millivolts */
    uint8_t valid;          /* 1 if readings are valid */
} INA226_Data_t;

/* Function Prototypes */
HAL_StatusTypeDef INA226_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef INA226_Reset(void);
HAL_StatusTypeDef INA226_ReadVoltage(float *voltage_V);
HAL_StatusTypeDef INA226_ReadCurrent(float *current_A);
HAL_StatusTypeDef INA226_ReadPower(float *power_W);
HAL_StatusTypeDef INA226_ReadAll(INA226_Data_t *data);
HAL_StatusTypeDef INA226_IsConnected(void);
uint16_t INA226_GetManufacturerID(void);
uint16_t INA226_GetDieID(void);

#endif /* INA226_H */
