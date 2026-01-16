/**
 * VL53L5CX Platform layer - EXACT COPY from ST official example
 * CubeIDE_F401RE_Example/Core/Src/platform.c
 * Modified only to use hi2c2 instead of hi2c1
 */

#include "platform.h"

extern I2C_HandleTypeDef hi2c2;  /* VL53L5CX is on I2C2 */

uint8_t VL53L5CX_RdByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 0;
	uint8_t data_write[2];
	uint8_t data_read[1];

	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c2, p_platform->address, data_write, 2, 100);
	status = HAL_I2C_Master_Receive(&hi2c2, p_platform->address, data_read, 1, 100);
	*p_value = data_read[0];

	return status;
}

uint8_t VL53L5CX_WrByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t data_write[3];
	uint8_t status = 0;

	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	data_write[2] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c2, p_platform->address, data_write, 3, 100);

	return status;
}

uint8_t VL53L5CX_WrMulti(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	/* CRITICAL: Use HAL_I2C_Mem_Write with I2C_MEMADD_SIZE_16BIT! */
	uint8_t status = HAL_I2C_Mem_Write(&hi2c2, p_platform->address, RegisterAdress,
									I2C_MEMADD_SIZE_16BIT, p_values, size, 65535);
	return status;
}

uint8_t VL53L5CX_RdMulti(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status;
	uint8_t data_write[2];
	data_write[0] = (RegisterAdress>>8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c2, p_platform->address, data_write, 2, 100);
	status += HAL_I2C_Master_Receive(&hi2c2, p_platform->address, p_values, size, 100);

	return status;
}

uint8_t VL53L5CX_Reset_Sensor(VL53L5CX_Platform *p_platform)
{
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	/* Not implemented - using XSHUT pin instead */
	return 0;
}

void VL53L5CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;

	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4)
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);

		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t VL53L5CX_WaitMs(
		VL53L5CX_Platform *p_platform,
		uint32_t TimeMs)
{
	/* Use HAL_Delay directly as in ST example */
	HAL_Delay(TimeMs);
	return 0;
}
