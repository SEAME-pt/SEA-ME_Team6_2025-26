#ifndef TEST_HTS221_STUB_H
#define TEST_HTS221_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"


// Endereços I2C e registos importantes do sensor HTS221
#define HTS221_I2C_ADDR      0xBE
#define HTS221_WHO_AM_I      0x0F
#define HTS221_CTRL_REG1     0x20
#define HTS221_STATUS_REG    0x27
#define HTS221_HUMIDITY_OUT_L 0x28
#define HTS221_TEMP_OUT_L    0x2A

// Registos usados para calibração e conversão dos valores brutos
#define HTS221_H0_RH_X2      0x30
#define HTS221_H1_RH_X2      0x31
#define HTS221_T0_DEGC_X8    0x32
#define HTS221_T1_DEGC_X8    0x33
#define HTS221_T1_T0_MSB     0x35
#define HTS221_H0_T0_OUT     0x36
#define HTS221_H1_T0_OUT     0x3A
#define HTS221_T0_OUT        0x3C
#define HTS221_T1_OUT        0x3E

// Valor esperado no registo WHO_AM_I para identificação do sensor
#define HTS221_ID            0xBC

// Estrutura simplificada que representa os parâmetros de calibração
// lidos do sensor necessários para converter os valores brutos em
// humidade (%) e temperatura (°C).
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

// Handle I2C usado pelos testes
extern I2C_HandleTypeDef hi2c2;

// - HTS221_Init: inicializa o sensor e verifica WHO_AM_I
// - HTS221_ReadHumidity: lê e converte a humidade para percentagem
// - HTS221_ReadTemperature: lê e converte a temperatura para °C
HAL_StatusTypeDef HTS221_Init(void);
HAL_StatusTypeDef HTS221_ReadHumidity(float *humidity);
HAL_StatusTypeDef HTS221_ReadTemperature(float *temperature);

#ifdef __cplusplus
}
#endif

#endif
