/*
 * ina226.c
 * Driver for INA226 Current/Voltage Monitor
 * Created on: Jan 27, 2026
 * Author: rcosta-c
 */

#include "ina226.h"
#include <string.h>

/* Private variables */
static I2C_HandleTypeDef *ina226_i2c = NULL;
static uint8_t ina226_addr = INA226_I2C_ADDR << 1; /* 8-bit address */

/* Private function prototypes */
static HAL_StatusTypeDef INA226_WriteRegister(uint8_t reg, uint16_t value);
static HAL_StatusTypeDef INA226_ReadRegister(uint8_t reg, uint16_t *value);

/**
  * @brief  Write a 16-bit value to an INA226 register
  * @param  reg: Register address
  * @param  value: 16-bit value to write
  * @retval HAL status
  */
static HAL_StatusTypeDef INA226_WriteRegister(uint8_t reg, uint16_t value)
{
    uint8_t data[3];
    data[0] = reg;
    data[1] = (value >> 8) & 0xFF;  /* MSB first */
    data[2] = value & 0xFF;         /* LSB */

    return HAL_I2C_Master_Transmit(ina226_i2c, ina226_addr, data, 3, HAL_MAX_DELAY);
}

/**
  * @brief  Read a 16-bit value from an INA226 register
  * @param  reg: Register address
  * @param  value: Pointer to store the 16-bit value
  * @retval HAL status
  */
static HAL_StatusTypeDef INA226_ReadRegister(uint8_t reg, uint16_t *value)
{
    HAL_StatusTypeDef status;
    uint8_t data[2];

    /* Write register address */
    status = HAL_I2C_Master_Transmit(ina226_i2c, ina226_addr, &reg, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return status;
    }

    /* Read 2 bytes */
    status = HAL_I2C_Master_Receive(ina226_i2c, ina226_addr, data, 2, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return status;
    }

    /* Combine MSB and LSB */
    *value = ((uint16_t)data[0] << 8) | data[1];

    return HAL_OK;
}

/**
  * @brief  Initialize the INA226 sensor
  * @param  hi2c: Pointer to I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_Init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;
    uint16_t manufacturer_id;

    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    ina226_i2c = hi2c;

    /* Check if device is connected */
    status = HAL_I2C_IsDeviceReady(ina226_i2c, ina226_addr, 3, 100);
    if (status != HAL_OK) {
        return status;
    }

    /* Verify manufacturer ID */
    status = INA226_ReadRegister(INA226_REG_MANUFACTURER, &manufacturer_id);
    if (status != HAL_OK) {
        return status;
    }

    if (manufacturer_id != INA226_MANUFACTURER_ID) {
        /* Not a TI INA226 chip */
        return HAL_ERROR;
    }

    /* Reset the device */
    status = INA226_WriteRegister(INA226_REG_CONFIG, INA226_CONFIG_RESET);
    if (status != HAL_OK) {
        return status;
    }

    /* Wait for reset to complete */
    HAL_Delay(1);

    /* Configure: 16 averages, 1.1ms conversion, continuous mode */
    status = INA226_WriteRegister(INA226_REG_CONFIG, INA226_CONFIG_DEFAULT);
    if (status != HAL_OK) {
        return status;
    }

    /* Set calibration register for ±20A range */
    status = INA226_WriteRegister(INA226_REG_CALIBRATION, INA226_CALIBRATION_VALUE);
    if (status != HAL_OK) {
        return status;
    }

    return HAL_OK;
}

/**
  * @brief  Reset the INA226 sensor
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_Reset(void)
{
    return INA226_WriteRegister(INA226_REG_CONFIG, INA226_CONFIG_RESET);
}

/**
  * @brief  Read bus voltage
  * @param  voltage_V: Pointer to store voltage in Volts
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_ReadVoltage(float *voltage_V)
{
    HAL_StatusTypeDef status;
    uint16_t raw_value;

    status = INA226_ReadRegister(INA226_REG_BUS_VOLTAGE, &raw_value);
    if (status != HAL_OK) {
        return status;
    }

    /* Convert to Volts: LSB = 1.25mV */
    *voltage_V = (float)raw_value * INA226_BUS_VOLTAGE_LSB / 1000.0f;

    return HAL_OK;
}

/**
  * @brief  Read current
  * @param  current_A: Pointer to store current in Amperes (negative = discharge)
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_ReadCurrent(float *current_A)
{
    HAL_StatusTypeDef status;
    uint16_t raw_value;
    int16_t signed_value;

    status = INA226_ReadRegister(INA226_REG_CURRENT, &raw_value);
    if (status != HAL_OK) {
        return status;
    }

    /* Convert to signed value */
    signed_value = (int16_t)raw_value;

    /* Convert to Amperes: LSB = 1mA (based on calibration) */
    *current_A = (float)signed_value * INA226_CURRENT_LSB_MA / 1000.0f;

    return HAL_OK;
}

/**
  * @brief  Read power
  * @param  power_W: Pointer to store power in Watts
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_ReadPower(float *power_W)
{
    HAL_StatusTypeDef status;
    uint16_t raw_value;

    status = INA226_ReadRegister(INA226_REG_POWER, &raw_value);
    if (status != HAL_OK) {
        return status;
    }

    /* Convert to Watts: Power_LSB = 25 × Current_LSB = 25mW */
    *power_W = (float)raw_value * INA226_POWER_LSB * INA226_CURRENT_LSB_MA / 1000.0f;

    return HAL_OK;
}

/**
  * @brief  Read all sensor data
  * @param  data: Pointer to INA226_Data_t structure
  * @retval HAL status
  */
HAL_StatusTypeDef INA226_ReadAll(INA226_Data_t *data)
{
    HAL_StatusTypeDef status;
    uint16_t raw_bus, raw_shunt, raw_current, raw_power;
    int16_t signed_shunt, signed_current;

    data->valid = 0;

    /* Read bus voltage */
    status = INA226_ReadRegister(INA226_REG_BUS_VOLTAGE, &raw_bus);
    if (status != HAL_OK) {
        return status;
    }

    /* Read shunt voltage */
    status = INA226_ReadRegister(INA226_REG_SHUNT_VOLTAGE, &raw_shunt);
    if (status != HAL_OK) {
        return status;
    }

    /* Read current */
    status = INA226_ReadRegister(INA226_REG_CURRENT, &raw_current);
    if (status != HAL_OK) {
        return status;
    }

    /* Read power */
    status = INA226_ReadRegister(INA226_REG_POWER, &raw_power);
    if (status != HAL_OK) {
        return status;
    }

    /* Convert values */
    signed_shunt = (int16_t)raw_shunt;
    signed_current = (int16_t)raw_current;

    /* Bus voltage: LSB = 1.25mV */
    data->voltage_V = (float)raw_bus * INA226_BUS_VOLTAGE_LSB / 1000.0f;

    /* Shunt voltage: LSB = 2.5µV */
    data->shunt_voltage_mV = (float)signed_shunt * INA226_SHUNT_VOLTAGE_LSB / 1000.0f;

    /* Current: LSB = 1mA */
    data->current_A = (float)signed_current * INA226_CURRENT_LSB_MA / 1000.0f;

    /* Power: LSB = 25mW */
    data->power_W = (float)raw_power * INA226_POWER_LSB * INA226_CURRENT_LSB_MA / 1000.0f;

    data->valid = 1;

    return HAL_OK;
}

/**
  * @brief  Check if INA226 is connected
  * @retval HAL status (HAL_OK if connected)
  */
HAL_StatusTypeDef INA226_IsConnected(void)
{
    return HAL_I2C_IsDeviceReady(ina226_i2c, ina226_addr, 3, 100);
}

/**
  * @brief  Get manufacturer ID
  * @retval Manufacturer ID (0x5449 for TI)
  */
uint16_t INA226_GetManufacturerID(void)
{
    uint16_t id = 0;
    INA226_ReadRegister(INA226_REG_MANUFACTURER, &id);
    return id;
}

/**
  * @brief  Get die ID
  * @retval Die ID (0x2260 for INA226)
  */
uint16_t INA226_GetDieID(void)
{
    uint16_t id = 0;
    INA226_ReadRegister(INA226_REG_DIE_ID, &id);
    return id;
}
