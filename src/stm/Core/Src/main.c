/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"
#include "main.h"
#include "mdf.h"
#include "i2c.h"
#include "icache.h"
#include "octospi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "ucpd.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lps22hh.h"
#include <stdio.h>
#include "stm32_can_benchmark.h"

#include "motor_control.h"
#include "srf08.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


#define SERVO_MIN_PULSE  500   // 0.5ms - posição mínima (~0°)
#define SERVO_MID_PULSE  1500  // 1.5ms - posição central (~90°)
#define SERVO_MAX_PULSE  2500  // 2.5ms - posição máxima (~180°)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;



uint8_t matrix_buf[2][8] = {0};  // buffer para cada matriz




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


#define MATRIX_ADDR_1  0x71
#define MATRIX_ADDR_2  0x74

void matrix_init(I2C_HandleTypeDef *hi2c, uint8_t addr) {
    uint8_t cmd;

    cmd = 0x21;
    HAL_I2C_Master_Transmit(hi2c, addr << 1, &cmd, 1, 100);
    HAL_Delay(10);

    cmd = 0x81;
    HAL_I2C_Master_Transmit(hi2c, addr << 1, &cmd, 1, 100);
    HAL_Delay(10);

    cmd = 0xEF;
    HAL_I2C_Master_Transmit(hi2c, addr << 1, &cmd, 1, 100);
    HAL_Delay(10);
}

void matrix_all_on(I2C_HandleTypeDef *hi2c, uint8_t addr) {
    uint8_t packet[17] = {0};
    packet[0] = 0x00;  // endereço RAM
    // o HT16K33 usa 16 bytes de RAM, cada linha ocupa 2 bytes
    // byte par = colunas 0-7, byte ímpar = não usado (0x00)
    for (int i = 0; i < 8; i++) {
        packet[1 + (i * 2)] = 0xFF;   // byte da linha
        packet[1 + (i * 2) + 1] = 0x00; // byte reservado
    }
    HAL_I2C_Master_Transmit(hi2c, addr << 1, packet, 17, 100);
}


void matrix_clear(I2C_HandleTypeDef *hi2c, uint8_t addr) {
    uint8_t packet[17] = {0};
    packet[0] = 0x00;
    HAL_I2C_Master_Transmit(hi2c, addr << 1, packet, 17, 100);
}

void matrix_write_column(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t col) {
    uint8_t packet[17] = {0};
    packet[0] = 0x00;
    for (int row = 0; row < 8; row++) {
        packet[1 + (row * 2)] = (1 << col);  // acende toda a coluna
    }
    HAL_I2C_Master_Transmit(hi2c, addr << 1, packet, 17, 100);
}





// Function para mover servo suavemente
void Test_Servo_SetAngle(uint16_t angle)
{
    // angle: 0 a 180 graus
    // Fórmula: pulse = 500 + (angle * 2000 / 180)
    uint16_t pulse = 500 + (angle * 2000 / 180);

    if (pulse > SERVO_MAX_PULSE) pulse = SERVO_MAX_PULSE;
    if (pulse < SERVO_MIN_PULSE) pulse = SERVO_MIN_PULSE;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADF1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_ICACHE_Init();
  MX_OCTOSPI1_Init();
  MX_OCTOSPI2_Init();
  MX_SPI2_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_UCPD1_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(100);  // pequena pausa a seguir ao boot

  // I2C1 Scanner
  uint8_t i2c_scan_result[128] = {0};
  uint8_t found_count = 0;

  printf("=== I2C1 Scanner ===\r\n");
  printf("Scanning addresses 0x01 to 0x7F...\r\n\r\n");

  for (uint8_t addr = 1; addr < 128; addr++)
  {
      HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2, 10);

      if (result == HAL_OK)
      {
          printf("  [FOUND] 0x%02X\r\n", addr);
          i2c_scan_result[found_count++] = addr;
      }

      /* Recover I2C bus if stuck after NACK/error */
      if (hi2c1.State != HAL_I2C_STATE_READY)
      {
          HAL_I2C_Init(&hi2c1);
      }
      __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_AF);
  }

  if (found_count == 0)
  {
      printf("No I2C devices found!\r\n");
  }
  else
  {
      printf("\r\nTotal devices found: %d\r\n", found_count);
  }
  printf("=== Scan complete ===\r\n\r\n");



  HAL_Delay(100);  // pequena pausa a seguir ao boot

  matrix_init(&hi2c1, MATRIX_ADDR_1);
  matrix_init(&hi2c1, MATRIX_ADDR_2);

  matrix_all_on(&hi2c1, MATRIX_ADDR_1);
  matrix_all_on(&hi2c1, MATRIX_ADDR_2);


  // Inicia o PWM no TIM1_CH1 (PA8)
  printf("Iniciando PWM para servo motor em PA8 (TIM1_CH1)...\r\n");
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  // Posiciona servo na posição central no início
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_MID_PULSE);
  printf("Servo posicionado na posição central (90°).\r\n\r\n");

  HAL_Delay(500);

  /* USER CODE END 2 */
  printf("RACALLAJJSADSADSADPSAPDSA\r\n asdsadsad \r\n \r\n\r\n");

  MX_ThreadX_Init();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  // Matriz 1: varre esquerda -> direita
	      // Matriz 2: varre direita -> esquerda
	      for (int i = 0; i < 8; i++)
	      {
	          matrix_write_column(&hi2c1, MATRIX_ADDR_1, i);
	          matrix_write_column(&hi2c1, MATRIX_ADDR_2, 7 - i);
	          HAL_Delay(80);
	      }

	      // Volta: Matriz 1 direita -> esquerda, Matriz 2 esquerda -> direita
	      for (int i = 7; i >= 0; i--)
	      {
	          matrix_write_column(&hi2c1, MATRIX_ADDR_1, i);
	          matrix_write_column(&hi2c1, MATRIX_ADDR_2, 7 - i);
	          HAL_Delay(80);
	      }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{
  HAL_PWREx_EnableVddIO2();

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
