/**
  * Mock do main.h para testes
  * Simula as definições do STM32
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// GPIO Pin States
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
} GPIO_PinState;

// GPIO Definitions
#define GPIOB ((void*)0x48000400)
#define GPIO_PIN_6 ((uint16_t)0x0040)

// HAL Status
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// Mock functions
GPIO_PinState HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin);
uint32_t HAL_GetTick(void);

// Test helper functions
void mock_set_gpio_state(GPIO_PinState state);
void mock_set_tick(uint32_t tick);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
