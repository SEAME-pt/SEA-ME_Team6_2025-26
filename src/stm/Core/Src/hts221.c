/*
 * hts221.c
 * Driver implementation for HTS221
 */

#include "hts221.h"
#include "i2c.h" // Garante que tens acesso a hi2c2

/* Handle externo definido no main.c */
extern I2C_HandleTypeDef hi2c2;

/* Variável interna para guardar a calibração */
static HTS221_Calibration_t CalibData;

/* --- Funções Auxiliares --- */
static HAL_StatusTypeDef HTS221_WriteReg(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, HTS221_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

static HAL_StatusTypeDef HTS221_ReadReg(uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c2, HTS221_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100);
}

static HAL_StatusTypeDef HTS221_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length)
{
  /* O bit MSB do endereço deve ser 1 para auto-incremento em alguns sensores,
     mas no HTS221 funciona direto com I2C_Mem_Read se o registo suportar burst */
  return HAL_I2C_Mem_Read(&hi2c2, HTS221_I2C_ADDR, reg | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

/* --- Inicialização e Leitura de Calibração --- */
HAL_StatusTypeDef HTS221_Init(void)
{
  HAL_StatusTypeDef status;
  uint8_t id;
  uint8_t buffer[16];

  /* 1. Verificar ID */
  status = HTS221_ReadReg(HTS221_WHO_AM_I, &id);
  if (status != HAL_OK || id != HTS221_ID) return HAL_ERROR;

  /* 2. Power Down primeiro (recomendado) */
  HTS221_WriteReg(HTS221_CTRL_REG1, 0x00);

  /* 3. LER CALIBRAÇÃO (Necessário para cálculos) */
  /* Ler registos 0x30 a 0x3F de uma vez ou individualmente */

  // Ler H0_rH_x2 (0x30) e H1_rH_x2 (0x31)
  status = HTS221_ReadRegMulti(0x30, buffer, 2);
  CalibData.H0_rH_x2 = buffer[0];
  CalibData.H1_rH_x2 = buffer[1];

  // Ler T0_degC_x8 (0x32), T1_degC_x8 (0x33) e MSB (0x35)
  uint8_t t0_degc, t1_degc, msb;
  HTS221_ReadReg(0x32, &t0_degc);
  HTS221_ReadReg(0x33, &t1_degc);
  HTS221_ReadReg(0x35, &msb);

  // Os bits MSB da temperatura estão misturados no reg 0x35
  uint16_t T0_degC_x8_u16 = (((uint16_t)(msb & 0x03)) << 8) | ((uint16_t)t0_degc);
  uint16_t T1_degC_x8_u16 = (((uint16_t)(msb & 0x0C)) << 6) | ((uint16_t)t1_degc);

  CalibData.T0_degC_x8 = (int16_t)T0_degC_x8_u16;
  CalibData.T1_degC_x8 = (int16_t)T1_degC_x8_u16;

  // Ler H0_T0_OUT (0x36, 0x37)
  status = HTS221_ReadRegMulti(0x36, buffer, 2);
  CalibData.H0_T0_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

  // Ler H1_T0_OUT (0x3A, 0x3B)
  status = HTS221_ReadRegMulti(0x3A, buffer, 2);
  CalibData.H1_T0_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

  // Ler T0_OUT (0x3C, 0x3D)
  status = HTS221_ReadRegMulti(0x3C, buffer, 2);
  CalibData.T0_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

  // Ler T1_OUT (0x3E, 0x3F)
  status = HTS221_ReadRegMulti(0x3E, buffer, 2);
  CalibData.T1_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

  /* 4. Ativar Sensor (PD=1, BDU=1, ODR=1Hz) */
  /* PD (bit 7) = 1, BDU (bit 2) = 1, ODR (bit 1:0) = 01 (1Hz) -> 1000 0101 = 0x85 */
  status = HTS221_WriteReg(HTS221_CTRL_REG1, 0x85);

  return status;
}

/* --- Leitura de Humidade --- */
HAL_StatusTypeDef HTS221_ReadHumidity(float *humidity)
{
  uint8_t buffer[2];
  int16_t h_t_out;

  /* Ler valor raw (ADC) */
  if (HTS221_ReadRegMulti(HTS221_HUMIDITY_OUT_L, buffer, 2) != HAL_OK) return HAL_ERROR;
  h_t_out = (int16_t)((buffer[1] << 8) | buffer[0]);

  /* Interpolação Linear */
  /* y = y0 + (x - x0) * (y1 - y0) / (x1 - x0) */
  float h0_rh = CalibData.H0_rH_x2 / 2.0f;
  float h1_rh = CalibData.H1_rH_x2 / 2.0f;

  *humidity = h0_rh + (float)(h_t_out - CalibData.H0_T0_OUT) * (h1_rh - h0_rh) / (float)(CalibData.H1_T0_OUT - CalibData.H0_T0_OUT);

  /* Limites */
  if (*humidity < 0.0f) *humidity = 0.0f;
  if (*humidity > 100.0f) *humidity = 100.0f;

  return HAL_OK;
}

/* --- Leitura de Temperatura --- */
HAL_StatusTypeDef HTS221_ReadTemperature(float *temperature)
{
  uint8_t buffer[2];
  int16_t t_out;

  /* Ler valor raw (ADC) */
  if (HTS221_ReadRegMulti(HTS221_TEMP_OUT_L, buffer, 2) != HAL_OK) return HAL_ERROR;
  t_out = (int16_t)((buffer[1] << 8) | buffer[0]);

  /* Interpolação Linear */
  float t0_degc = CalibData.T0_degC_x8 / 8.0f;
  float t1_degc = CalibData.T1_degC_x8 / 8.0f;

  *temperature = t0_degc + (float)(t_out - CalibData.T0_OUT) * (t1_degc - t0_degc) / (float)(CalibData.T1_OUT - CalibData.T0_OUT);

  return HAL_OK;
}
