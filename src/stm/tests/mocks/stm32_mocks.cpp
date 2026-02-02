/**
  * Implementação dos mocks para STM32 HAL e ThreadX
  */

#include "main.h"
#include "tx_api.h"
#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Minimal IIS2MDC register addresses used by mocks (avoid pulling project headers here)
  These match the values in src/stm/Core/Inc/iis2mdc.h */
#define IIS2MDC_WHO_AM_I        0x4F
#define IIS2MDC_OUTX_L          0x68
// Minimal ISM330DHCX register addresses used by mocks
#define ISM330DHCX_WHO_AM_I     0x0F
#define ISM330DHCX_OUTX_L_G     0x22
#define ISM330DHCX_OUTX_L_A     0x28
#define IIS2MDC_TEMP_OUT_L      0x6E
#include <stdio.h>
#include <string.h>
#include <vector>

// Estado simulado
static GPIO_PinState mock_gpio_state = GPIO_PIN_RESET;
static uint32_t mock_tick = 0;

// Mock do printf_mutex
TX_MUTEX printf_mutex = {
    .tx_mutex_id = 0x4D555445,
    .tx_mutex_name = (char*)"printf_mutex",
    .tx_mutex_ownership_count = 0,
    .tx_mutex_owner = nullptr,
    .tx_mutex_inherit = 0
};

// Mock I2C state
static HAL_StatusTypeDef mock_i2c_status = HAL_OK;
static std::vector<uint8_t> mock_i2c_last_tx_data;
static uint16_t mock_i2c_last_address = 0;
// Provide a global hi2c2 instance for tests (used by production driver)
I2C_HandleTypeDef hi2c2;
// Additional I2C mem read/write mocks
static HAL_StatusTypeDef mock_i2c_mem_status = HAL_OK;
static uint8_t mock_i2c_whoami = 0x00;
static uint8_t mock_i2c_mag_bytes[6] = {0};
static uint8_t mock_i2c_temp_bytes[2] = {0};
static uint8_t mock_i2c_accel_bytes[6] = {0};
static uint8_t mock_i2c_gyro_bytes[6] = {0};
// VEML6030 (ALS/WHITE) 2-byte values
static uint8_t mock_veml6030_als[2] = {0};
static uint8_t mock_veml6030_white[2] = {0};
// HTS221 register space (0x00..0x3F) to emulate calibration and outputs
static uint8_t mock_hts221_regs[0x40] = {0};

// Mock TIM/PWM state
static HAL_StatusTypeDef mock_tim_pwm_status = HAL_OK;
static uint32_t mock_tim_autoreload = 20000;  // 20ms period @ 1MHz
static uint32_t mock_tim_compare = 0;
static bool mock_tim_pwm_started = false;
static bool mock_tim_moe_enabled = false;

/**
  * @brief  Mock do HAL_GPIO_ReadPin
  */
extern "C" GPIO_PinState HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin)
{
    (void)GPIOx;
    (void)GPIO_Pin;
    return mock_gpio_state;
}

/**
  * @brief  Mock do HAL_GetTick
  */
extern "C" uint32_t HAL_GetTick(void)
{
    return mock_tick;
}

/**
  * @brief  Mock do tx_mutex_get
  */
extern "C" uint32_t tx_mutex_get(TX_MUTEX *mutex_ptr, uint32_t wait_option)
{
    (void)mutex_ptr;
    (void)wait_option;
    return TX_SUCCESS;
}

/**
  * @brief  Mock do tx_mutex_put
  */
extern "C" uint32_t tx_mutex_put(TX_MUTEX *mutex_ptr)
{
    (void)mutex_ptr;
    return TX_SUCCESS;
}

/**
  * @brief  Helper para definir o estado do GPIO nos testes
  */
extern "C" void mock_set_gpio_state(GPIO_PinState state)
{
    mock_gpio_state = state;
}

/**
  * @brief  Helper para definir o tick nos testes
  */
extern "C" void mock_set_tick(uint32_t tick)
{
    mock_tick = tick;
}

/**
  * @brief  Mock do HAL_Delay
  */
extern "C" void HAL_Delay(uint32_t Delay)
{
    (void)Delay;
    // Não faz nada nos testes (delay instantâneo)
}

/**
  * @brief  Mock do HAL_I2C_Master_Transmit
  */
extern "C" HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                                      uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)hi2c;
    (void)Timeout;

    // Guardar dados da última transmissão
    mock_i2c_last_address = DevAddress;
    mock_i2c_last_tx_data.clear();
    for (uint16_t i = 0; i < Size; i++) {
        mock_i2c_last_tx_data.push_back(pData[i]);
    }

    return mock_i2c_status;
}

/**
  * @brief  Mock do HAL_I2C_Mem_Read
  */
extern "C" HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                        uint8_t MemAddress, uint16_t MemAddSize,
                        uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  (void)hi2c; (void)DevAddress; (void)MemAddSize; (void)Timeout;

  if (mock_i2c_mem_status != HAL_OK) return mock_i2c_mem_status;

  if (MemAddress == IIS2MDC_WHO_AM_I && Size == 1) {
    pData[0] = mock_i2c_whoami;
    return HAL_OK;
  }

  if (MemAddress == IIS2MDC_OUTX_L && Size == 6) {
    for (int i = 0; i < 6; ++i) pData[i] = mock_i2c_mag_bytes[i];
    return HAL_OK;
  }

  if (MemAddress == IIS2MDC_TEMP_OUT_L && Size == 2) {
    pData[0] = mock_i2c_temp_bytes[0];
    pData[1] = mock_i2c_temp_bytes[1];
    return HAL_OK;
  }

  /* ISM330DHCX support: WHO_AM_I, gyro and accel multi-byte reads */
  if (MemAddress == ISM330DHCX_WHO_AM_I && Size == 1) {
    pData[0] = mock_i2c_whoami;
    return HAL_OK;
  }

  if (MemAddress == ISM330DHCX_OUTX_L_A && Size == 6) {
    for (int i = 0; i < 6; ++i) pData[i] = mock_i2c_accel_bytes[i];
    return HAL_OK;
  }

  if (MemAddress == ISM330DHCX_OUTX_L_G && Size == 6) {
    for (int i = 0; i < 6; ++i) pData[i] = mock_i2c_gyro_bytes[i];
    return HAL_OK;
  }

  /* VEML6030 support: ALS (0x04) and WHITE (0x05) 16-bit reads (LSB first) */
  if (MemAddress == 0x04 && Size == 2) { /* ALS */
    pData[0] = mock_veml6030_als[0];
    pData[1] = mock_veml6030_als[1];
    return HAL_OK;
  }

  if (MemAddress == 0x05 && Size == 2) { /* WHITE */
    pData[0] = mock_veml6030_white[0];
    pData[1] = mock_veml6030_white[1];
    return HAL_OK;
  }

  /* HTS221 support: support 0x00-0x3F register reads (handle auto-increment with MSB) */
  uint8_t baseAddr = MemAddress & 0x7F; // mask possible auto-increment bit
  if (baseAddr < 0x40) {
    for (int i = 0; i < Size; ++i) {
      uint8_t addr = baseAddr + i;
      if (addr < 0x40) pData[i] = mock_hts221_regs[addr];
      else pData[i] = 0;
    }
    return HAL_OK;
  }

  // Default behavior: zero-fill
  for (int i = 0; i < Size; ++i) pData[i] = 0;
  return HAL_OK;
}

/**
  * @brief  Mock do HAL_I2C_Mem_Write
  */
extern "C" HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                         uint8_t MemAddress, uint16_t MemAddSize,
                         uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  (void)hi2c; (void)DevAddress; (void)MemAddSize; (void)Timeout;
  // store last tx similar to Master_Transmit
  mock_i2c_last_address = DevAddress;
  mock_i2c_last_tx_data.clear();
  for (uint16_t i = 0; i < Size; i++) mock_i2c_last_tx_data.push_back(pData[i]);
  return mock_i2c_status;
}

/**
  * @brief  Mock do HAL_GPIO_WritePin
  */
extern "C" void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    (void)GPIOx;
    (void)GPIO_Pin;
    mock_gpio_state = PinState;
}

/**
  * @brief  Helper para definir o status do I2C nos testes
  */
extern "C" void mock_set_i2c_status(HAL_StatusTypeDef status)
{
    mock_i2c_status = status;
}

/**
  * @brief  Helper para obter os dados da última transmissão I2C
  */
extern "C" void mock_get_i2c_last_tx(uint8_t *buffer, uint16_t *size, uint16_t *address)
{
    *address = mock_i2c_last_address;
    *size = (uint16_t)mock_i2c_last_tx_data.size();
    for (size_t i = 0; i < mock_i2c_last_tx_data.size(); i++) {
        buffer[i] = mock_i2c_last_tx_data[i];
    }
}

/**
  * @brief  Helper para limpar histórico I2C
  */
extern "C" void mock_clear_i2c_history(void)
{
    mock_i2c_last_tx_data.clear();
    mock_i2c_last_address = 0;
}

/** Helpers to configure mem read mocks **/
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status)
{
  mock_i2c_mem_status = status;
}

extern "C" void mock_i2c_set_whoami(uint8_t v)
{
  mock_i2c_whoami = v;
}

extern "C" void mock_i2c_set_mag_bytes(const uint8_t *buf)
{
  for (int i = 0; i < 6; ++i) mock_i2c_mag_bytes[i] = buf[i];
}

extern "C" void mock_i2c_set_temp_bytes(const uint8_t *buf)
{
  for (int i = 0; i < 2; ++i) mock_i2c_temp_bytes[i] = buf[i];
}

extern "C" void mock_i2c_set_accel_bytes(const uint8_t *buf)
{
  for (int i = 0; i < 6; ++i) mock_i2c_accel_bytes[i] = buf[i];
}

extern "C" void mock_i2c_set_gyro_bytes(const uint8_t *buf)
{
  for (int i = 0; i < 6; ++i) mock_i2c_gyro_bytes[i] = buf[i];
}

/* VEML6030 helpers */
extern "C" void mock_veml6030_set_als(const uint8_t *buf)
{
  mock_veml6030_als[0] = buf[0];
  mock_veml6030_als[1] = buf[1];
}

extern "C" void mock_veml6030_set_white(const uint8_t *buf)
{
  mock_veml6030_white[0] = buf[0];
  mock_veml6030_white[1] = buf[1];
}

/* HTS221 helpers */
extern "C" void mock_hts221_set_reg(uint8_t addr, uint8_t val)
{
  if (addr < 0x40) mock_hts221_regs[addr] = val;
}

extern "C" void mock_hts221_set_regs(uint8_t start, const uint8_t *buf, uint8_t len)
{
  for (uint8_t i = 0; i < len; ++i) {
    uint8_t a = start + i;
    if (a < 0x40) mock_hts221_regs[a] = buf[i];
  }
}

extern "C" void mock_hts221_clear(void)
{
  for (int i = 0; i < 0x40; ++i) mock_hts221_regs[i] = 0;
}

/**
  * @brief  Mock do HAL_TIM_PWM_Start
  */
extern "C" HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    (void)htim;
    (void)Channel;
    mock_tim_pwm_started = true;
    return mock_tim_pwm_status;
}

/**
  * @brief  Mock do HAL_TIM_PWM_Stop
  */
extern "C" HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    (void)htim;
    (void)Channel;
    mock_tim_pwm_started = false;
    return mock_tim_pwm_status;
}

/**
  * @brief  Helper para definir status do TIM/PWM
  */
extern "C" void mock_set_tim_pwm_status(HAL_StatusTypeDef status)
{
    mock_tim_pwm_status = status;
}

/**
  * @brief  Helper para obter autoreload (ARR)
  */
extern "C" uint32_t mock_tim_get_autoreload(TIM_HandleTypeDef *htim)
{
    (void)htim;
    return mock_tim_autoreload;
}

/**
  * @brief  Helper para definir autoreload
  */
extern "C" void mock_tim_set_autoreload(uint32_t arr)
{
    mock_tim_autoreload = arr;
}

/**
  * @brief  Helper para definir compare (CCR)
  */
extern "C" void mock_tim_set_compare(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t Compare)
{
    (void)htim;
    (void)Channel;
    mock_tim_compare = Compare;
}

/**
  * @brief  Helper para obter compare (CCR)
  */
extern "C" uint32_t mock_tim_get_compare(void)
{
    return mock_tim_compare;
}

/**
  * @brief  Helper para habilitar MOE
  */
extern "C" void mock_tim_moe_enable(TIM_HandleTypeDef *htim)
{
    (void)htim;
    mock_tim_moe_enabled = true;
}

/**
  * @brief  Helper para verificar se MOE está habilitado
  */
extern "C" bool mock_tim_is_moe_enabled(void)
{
    return mock_tim_moe_enabled;
}

/**
  * @brief  Helper para verificar se PWM está iniciado
  */
extern "C" bool mock_tim_is_pwm_started(void)
{
    return mock_tim_pwm_started;
}

/**
  * @brief  Helper para reset do estado TIM
  */
extern "C" void mock_tim_reset(void)
{
    mock_tim_pwm_status = HAL_OK;
    mock_tim_autoreload = 20000;
    mock_tim_compare = 0;
    mock_tim_pwm_started = false;
    mock_tim_moe_enabled = false;
}
