---
id: EVID-L0-26
header: End-to-End Startup Time Evidence
text: 'Evidence demonstrating that the complete system provides end-to-end availability
  of safety-relevant data and a usable instrument cluster UI within the specified
  time bounds.


  **Verification Method:** End-to-end integration test. External time measurement
  (video recording from power-on to UI display). System logs from STM32, AGL, and
  Qt. CAN traffic correlation with UI updates.


  **Expected Artifacts:**

  - Video recording from power-on to full system availability

  - STM32 startup and CAN transmission logs (showing data within 100 ms)

  - Qt UI startup timing logs (showing UI within 2.0 s)

  - AGL/KUKSA service startup logs (showing operational within 10 s)

  - CAN traffic captures correlated with UI updates

  - End-to-end timing correlation analysis

  '
level: '1.26'
normative: true
references:
- type: file
  path: docs/demos/Cluster-display-sprint4.jpeg
- type: file
  path: docs/demos/Cluster-display-sprint3.jpeg
- type: file
  path: docs/demos/Cluster-state-sprint3.jpeg
- type: file
  path: docs/demos/car_architecture_new_2 - sprint6.jpg
- type: file
  path: docs/demos/car_architecture_old - sprint6.jpeg
- type: file
  path: docs/demos/mockup2.jpeg
- type: file
  path: docs/demos/first_qt_app.jpeg
- type: file
  path: docs/demos/CAN Scheme - New update.png
- type: file
  path: docs/demos/mockup5.jpeg
- type: file
  path: docs/demos/Cluster-study.jpeg
- type: file
  path: docs/demos/mockup6.jpeg
- type: file
  path: docs/demos/crosscompiling_scheme-sprint6.jpeg
- type: file
  path: docs/demos/car_architecture_new_1 - sprint6.jpg
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint0.jpeg
- type: file
  path: docs/demos/mockup4.jpeg
- type: file
  path: docs/demos/LCOV_coverage_report_QT - sprint 6.jpeg
- type: file
  path: docs/demos/mockup1.jpeg
- type: file
  path: docs/demos/system-architecture-cross.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint3.jpeg
- type: file
  path: docs/demos/mockup3.jpeg
- type: file
  path: docs/demos/ThreadX_Running.gif
- type: file
  path: docs/demos/CAN-layout-sprint3.jpeg
- type: file
  path: docs/demos/stm_can_transmission - sprint6.jpg
- type: file
  path: docs/demos/STM_Ilustra.jpg
- type: file
  path: docs/demos/Cluster-mockup-sprint3.jpeg
- type: file
  path: docs/demos/eletric-scheme.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint2.jpeg
- type: file
  path: docs/demos/CANbus Scheme Updated.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint1.jpeg
- type: file
  path: docs/guides/MQTT_spike.md
- type: file
  path: docs/guides/QML-guide.md
- type: file
  path: docs/guides/AGL_and_ThreadX_benefits_Guide.md
- type: file
  path: docs/guides/AGL_Installation_Guide.md
- type: file
  path: docs/guides/AGL_ Joystick_installation_and_test.md
- type: file
  path: docs/guides/ThreadX_Installation_Guide.md
- type: file
  path: docs/guides/ThreadXGuide.md
- type: file
  path: docs/guides/AGL_Configuration_Guide.md
- type: file
  path: docs/guides/CAN_test/app_threadx.c
- type: file
  path: docs/guides/CAN_test/app_threadx.h
- type: file
  path: docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
- type: file
  path: docs/images/CanFeederArchitecture.jpg
- type: file
  path: docs/presentations/threadX/2025.11 Eclipse ThreadX_ A recipe for success with
    four simple ingredients.pdf
- type: file
  path: src/setup/scripts/r5-qt6-build.sh
- type: file
  path: src/setup/scripts/qt6-build.sh
- type: file
  path: src/setup/toolchains/agl-toolchain.cmake
- type: file
  path: src/setup/toolchains/r5-agl-toolchain.cmake
- type: file
  path: src/ota/rauc/bootloader-custom-backend.sh
- type: file
  path: src/ota/rauc/post-reboot-verify.sh
- type: file
  path: src/kuksa/kuksa_RPi5/src/is_stm_connected.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/src/handlers/heartbeat_stm.cpp
- type: file
  path: src/kuksa/kuksa_RPi5/inc/is_stm_connected.hpp
- type: file
  path: src/tests/unit/mocks/stm32_mocks.cpp
- type: file
  path: src/tests/unit/mocks/stm32u5xx_hal.h
- type: file
  path: src/tests/unit/cpp/is_stm_connected_test.cpp
- type: file
  path: src/tests/unit/cpp/heartbeat_stm_test.cpp
- type: file
  path: src/CAN/CAN_test/app_threadx.c
- type: file
  path: src/CAN/CAN_test/app_threadx.h
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
review_status: pending
---
This evidence item collects end-to-end integration test results, video recordings, and system logs that demonstrate all startup time requirements are met.

**Evidence Status:** Pending - end-to-end integration test to be performed during final system integration phase.
