---
id: EVID_L0_18
header: 'Evidence: CAN Bus Communication with ThreadX'
text: 'Evidence demonstrating CAN bus communication integrated with ThreadX RTOS on
  STM32. Includes CAN message traces, ThreadX thread scheduling logs, and integration
  test results.

  '
level: '1.18'
normative: true
references:
- type: file
  path: docs/guides/ThreadXGuide.md
- type: file
  path: docs/guides/CAN-test-guide.md
- type: file
  path: docs/guides/AGL_and_ThreadX_benefits_Guide.md
- type: file
  path: docs/guides/CAN-explanation.md
- type: file
  path: docs/guides/ThreadX_Installation_Guide.md
- type: file
  path: docs/guides/CAN-overview.md
- type: file
  path: docs/guides/CAN_test/CAN_Test.c
- type: file
  path: docs/guides/CAN_test/CAN-test.gif
- type: file
  path: docs/guides/CAN_test/app_threadx.c
- type: file
  path: docs/guides/CAN_test/app_threadx.h
- type: file
  path: docs/images/CANFrame.png
- type: file
  path: docs/images/CANsignal.png
- type: file
  path: docs/images/CANtopology2.png
- type: file
  path: docs/images/CanFeederArchitecture.jpg
- type: file
  path: docs/images/CANtopology.png
- type: file
  path: docs/presentations/threadX/2025.11 Eclipse ThreadX_ A recipe for success with
    four simple ingredients.pdf
- type: file
  path: src/ota/scripts/canary-check.sh
- type: file
  path: src/kuksa/kuksa_RPi5/src/is_stm_connected.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/src/can_to_kuksa_publisher.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/src/can_decode.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/src/handlers/heartbeat_stm.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/can_decode.hpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/can_encode.hpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/can_to_kuksa_publisher.hpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/is_stm_connected.hpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/can_id.h
- type: file
  path: src/kuksa/kuksa_RPi4_display/inc/can_id.h
- type: file
  path: src/hmi/assets/icons/car-inactive.svg
- type: file
  path: src/hmi/assets/icons/car-active.svg
- type: file
  path: src/tests/unit/mocks/stm32_mocks.cpp
- type: file
  path: src/tests/unit/mocks/stm32u5xx_hal.h
- type: file
  path: src/tests/unit/cpp/can_decode.hpp
- type: file
  path: src/tests/unit/cpp/can_frame_builder.hpp
- type: file
  path: src/tests/unit/cpp/is_stm_connected_test.cpp
- type: file
  path: src/tests/unit/cpp/can_decode_test.cpp
- type: file
  path: src/tests/unit/cpp/heartbeat_stm_test.cpp
- type: file
  path: src/shared/backend/kuksa/can_id.h
- type: file
  path: src/shared/backend/providers/vehicleprovider.hpp
- type: file
  path: src/shared/backend/providers/vehicleprovider.cpp
- type: file
  path: src/CAN/CAN_test/CAN_Test.c
- type: file
  path: src/CAN/CAN_test/CAN-test.gif
- type: file
  path: src/CAN/CAN_test/app_threadx.c
- type: file
  path: src/CAN/CAN_test/app_threadx.h
- type: file
  path: src/stm/Core/Inc/vehicle_state.h
- type: file
  path: src/stm/Core/Inc/vl53l5cx_platform_stm32.h
- type: file
  path: src/stm/Core/Inc/stm32_can_benchmark.h
- type: file
  path: src/stm/Core/Inc/stm32u5xx_hal_conf.h
- type: file
  path: src/stm/Core/Inc/stm32_assert.h
- type: file
  path: src/stm/Core/Inc/stm32u5xx_it.h
- type: file
  path: src/stm/Core/Inc/app_threadx.h
- type: file
  path: src/stm/Core/Inc/can_tx.h
- type: file
  path: src/stm/Core/Inc/can_id.h
- type: file
  path: src/stm/Core/Inc/tasks/task_can_rx.h
- type: file
  path: src/stm/Core/Src/can_tx.c
- type: file
  path: src/stm/Core/Src/stm32_can_benchmark.c
- type: file
  path: src/stm/Core/Src/vl53l5cx_platform_stm32.c
- type: file
  path: src/stm/Core/Src/app_threadx.c
- type: file
  path: src/stm/Core/Src/system_stm32u5xx.c
- type: file
  path: src/stm/Core/Src/stm32u5xx_hal_timebase_tim.c
- type: file
  path: src/stm/Core/Src/stm32u5xx_it.c
- type: file
  path: src/stm/Core/Src/stm32u5xx_hal_msp.c
- type: file
  path: src/stm/Core/Src/tasks/task_can_rx.c
- type: file
  path: src/stm/AZURE_RTOS/App/app_azure_rtos_config.h
- type: file
  path: src/stm/AZURE_RTOS/App/app_azure_rtos.h
- type: file
  path: src/stm/AZURE_RTOS/App/app_azure_rtos.c
- type: file
  path: src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/system_stm32u5xx.h
- type: file
  path: src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u585xx.h
- type: file
  path: src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u5xx.h
- type: file
  path: src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpgpio.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gtzc.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_icache.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_tim.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_icache.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_i2c.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_spi.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_bus.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_mdf.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usb.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usart.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_pwr.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_system.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_rcc.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dlyb.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_utils.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpuart.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_ucpd.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dma.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_exti.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_def.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_cortex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_gpio.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_cortex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_ospi.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_exti.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_crs.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_gpio.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_ucpd.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_exti.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mdf.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_utils.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpgpio.c
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.


