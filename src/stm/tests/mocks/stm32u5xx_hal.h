/**
  * Mock do stm32u5xx_hal.h para testes
  */

#ifndef __STM32U5xx_HAL_H
#define __STM32U5xx_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"  // Já contém HAL_StatusTypeDef e GPIO_PinState

// I2C Handle Structure (simplified mock)
typedef struct
{
  void *Instance;
  uint32_t Init;
} I2C_HandleTypeDef;

// TIM Handle Structure (simplified mock)
typedef struct
{
  void *Instance;
  uint32_t Init;
  uint32_t Channel;
} TIM_HandleTypeDef;

// TIM Channels
#define TIM_CHANNEL_1  0x00000000U
#define TIM_CHANNEL_2  0x00000004U
#define TIM_CHANNEL_3  0x00000008U
#define TIM_CHANNEL_4  0x0000000CU

// Function prototypes (não duplicar do main.h)
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                          uint8_t *pData, uint16_t Size, uint32_t Timeout);
void HAL_Delay(uint32_t Delay);

// TIM/PWM Functions
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

// TIM Macros (converted to functions for testing)
#define __HAL_TIM_GET_AUTORELOAD(htim) mock_tim_get_autoreload(htim)
#define __HAL_TIM_SET_COMPARE(htim, channel, value) mock_tim_set_compare(htim, channel, value)
#define __HAL_TIM_MOE_ENABLE(htim) mock_tim_moe_enable(htim)

// Test helpers - I2C
void mock_set_i2c_status(HAL_StatusTypeDef status);
void mock_get_i2c_last_tx(uint8_t *buffer, uint16_t *size, uint16_t *address);
void mock_clear_i2c_history(void);

// Test helpers - TIM/PWM
void mock_set_tim_pwm_status(HAL_StatusTypeDef status);
uint32_t mock_tim_get_autoreload(TIM_HandleTypeDef *htim);
void mock_tim_set_autoreload(uint32_t arr);
void mock_tim_set_compare(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t Compare);
uint32_t mock_tim_get_compare(void);
void mock_tim_moe_enable(TIM_HandleTypeDef *htim);
bool mock_tim_is_moe_enabled(void);
bool mock_tim_is_pwm_started(void);
void mock_tim_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32U5xx_HAL_H */
