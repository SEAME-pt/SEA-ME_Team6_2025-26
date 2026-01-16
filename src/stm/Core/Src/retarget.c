/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    retarget.c
  * @brief   Retarget printf to UART (VCP)
  ******************************************************************************
  */
/* USER CODE END Header */

#include <stdio.h>
#include "stm32u5xx_hal.h"

/* External UART handle - USART1 is the VCP (Virtual COM Port) */
extern UART_HandleTypeDef huart1;

/**
  * @brief  Sends a character to USART1 (VCP)
  * @param  ch: character to send
  * @retval character sent
  */
int __io_putchar(int ch)
{
  /* Send character via UART */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/**
  * @brief  Receives a character from USART1 (not implemented)
  * @retval 0
  */
int __io_getchar(void)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
  return ch;
}
