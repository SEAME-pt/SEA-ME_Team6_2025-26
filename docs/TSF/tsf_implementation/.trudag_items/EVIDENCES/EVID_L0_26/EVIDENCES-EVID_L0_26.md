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
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint4.jpeg
  description: 'Evidence from docs/demos/Cluster-display-sprint4.jpeg: Cluster display
    sprint4'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint3.jpeg
  description: 'Evidence from docs/demos/Cluster-display-sprint3.jpeg: Cluster display
    sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-state-sprint3.jpeg
  description: 'Evidence from docs/demos/Cluster-state-sprint3.jpeg: Cluster state
    sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_2
    - sprint6.jpg
  description: 'Evidence from docs/demos/car_architecture_new_2 - sprint6.jpg: car
    architecture new 2   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_old
    - sprint6.jpeg
  description: 'Evidence from docs/demos/car_architecture_old - sprint6.jpeg: car
    architecture old   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup2.jpeg
  description: 'Evidence from docs/demos/mockup2.jpeg: mockup2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg
  description: 'Evidence from docs/demos/first_qt_app.jpeg: first qt app'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN Scheme
    - New update.png
  description: 'Evidence from docs/demos/CAN Scheme - New update.png: CAN Scheme   New
    update'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup5.jpeg
  description: 'Evidence from docs/demos/mockup5.jpeg: mockup5'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-study.jpeg
  description: 'Evidence from docs/demos/Cluster-study.jpeg: Cluster study'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup6.jpeg
  description: 'Evidence from docs/demos/mockup6.jpeg: mockup6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/crosscompiling_scheme-sprint6.jpeg
  description: 'Evidence from docs/demos/crosscompiling_scheme-sprint6.jpeg: crosscompiling
    scheme sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_1
    - sprint6.jpg
  description: 'Evidence from docs/demos/car_architecture_new_1 - sprint6.jpg: car
    architecture new 1   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint0.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint0.jpeg:
    Validation of energy layout sprint0'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup4.jpeg
  description: 'Evidence from docs/demos/mockup4.jpeg: mockup4'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/LCOV_coverage_report_QT
    - sprint 6.jpeg
  description: 'Evidence from docs/demos/LCOV_coverage_report_QT - sprint 6.jpeg:
    LCOV coverage report QT   sprint 6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup1.jpeg
  description: 'Evidence from docs/demos/mockup1.jpeg: mockup1'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/system-architecture-cross.png
  description: 'Evidence from docs/demos/system-architecture-cross.png: system architecture
    cross'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint3.jpeg:
    Validation of energy layout sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup3.jpeg
  description: 'Evidence from docs/demos/mockup3.jpeg: mockup3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/ThreadX_Running.gif
  description: 'Evidence from docs/demos/ThreadX_Running.gif: ThreadX Running'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-layout-sprint3.jpeg
  description: 'Evidence from docs/demos/CAN-layout-sprint3.jpeg: CAN layout sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/stm_can_transmission
    - sprint6.jpg
  description: 'Evidence from docs/demos/stm_can_transmission - sprint6.jpg: stm can
    transmission   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/STM_Ilustra.jpg
  description: 'Evidence from docs/demos/STM_Ilustra.jpg: STM Ilustra'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-mockup-sprint3.jpeg
  description: 'Evidence from docs/demos/Cluster-mockup-sprint3.jpeg: Cluster mockup
    sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png
  description: 'Evidence from docs/demos/eletric-scheme.png: eletric scheme'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint2.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint2.jpeg:
    Validation of energy layout sprint2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CANbus
    Scheme Updated.png
  description: 'Evidence from docs/demos/CANbus Scheme Updated.png: CANbus Scheme
    Updated'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint1.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint1.jpeg:
    Validation of energy layout sprint1'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/MQTT_spike.md
  description: 'Evidence from docs/guides/MQTT_spike.md: MQTT spike'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/QML-guide.md
  description: 'Evidence from docs/guides/QML-guide.md: QML guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_and_ThreadX_benefits_Guide.md
  description: 'Evidence from docs/guides/AGL_and_ThreadX_benefits_Guide.md: AGL and
    ThreadX benefits Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md
  description: 'Evidence from docs/guides/AGL_Installation_Guide.md: AGL Installation
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_
    Joystick_installation_and_test.md
  description: 'Evidence from docs/guides/AGL_ Joystick_installation_and_test.md:
    AGL  Joystick installation and test'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadX_Installation_Guide.md
  description: 'Evidence from docs/guides/ThreadX_Installation_Guide.md: ThreadX Installation
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md
  description: 'Evidence from docs/guides/ThreadXGuide.md: ThreadXGuide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Configuration_Guide.md
  description: 'Evidence from docs/guides/AGL_Configuration_Guide.md: AGL Configuration
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN_test/app_threadx.c
  description: 'Evidence from docs/guides/CAN_test/app_threadx.c: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN_test/app_threadx.h
  description: 'Evidence from docs/guides/CAN_test/app_threadx.h: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
  description: 'Evidence from docs/guides/Car_Architecture/Circuit_Connection_Diagram.md:
    Circuit Connection Diagram'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CanFeederArchitecture.jpg
  description: 'Evidence from docs/images/CanFeederArchitecture.jpg: CanFeederArchitecture'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/presentations/threadX/2025.11
    Eclipse ThreadX_ A recipe for success with four simple ingredients.pdf
  description: 'Evidence from docs/presentations/threadX/2025.11 Eclipse ThreadX_
    A recipe for success with four simple ingredients.pdf: 2025.11 Eclipse ThreadX  A
    recipe for success with four simple ingredients'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/scripts/r5-qt6-build.sh
  description: 'Evidence from src/setup/scripts/r5-qt6-build.sh: r5 qt6 build'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/scripts/qt6-build.sh
  description: 'Evidence from src/setup/scripts/qt6-build.sh: qt6 build'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/agl-toolchain.cmake: agl toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/r5-agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/r5-agl-toolchain.cmake: r5 agl
    toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/bootloader-custom-backend.sh
  description: 'Evidence from src/ota/rauc/bootloader-custom-backend.sh: bootloader
    custom backend'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/post-reboot-verify.sh
  description: 'Evidence from src/ota/rauc/post-reboot-verify.sh: post reboot verify'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/kuksa/kuksa_RPi5/src/is_stm_connected.cpp
  description: 'Evidence from src/kuksa/kuksa_RPi5/src/is_stm_connected.cpp: is stm
    connected'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/kuksa/kuksa_RPi5/src/handlers/heartbeat_stm.cpp
  description: 'Evidence from src/kuksa/kuksa_RPi5/src/handlers/heartbeat_stm.cpp:
    heartbeat stm'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/kuksa/kuksa_RPi5/inc/is_stm_connected.hpp
  description: 'Evidence from src/kuksa/kuksa_RPi5/inc/is_stm_connected.hpp: is stm
    connected'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/tests/unit/mocks/stm32_mocks.cpp
  description: 'Evidence from src/tests/unit/mocks/stm32_mocks.cpp: stm32 mocks'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/tests/unit/mocks/stm32u5xx_hal.h
  description: 'Evidence from src/tests/unit/mocks/stm32u5xx_hal.h: stm32u5xx hal'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/tests/unit/cpp/is_stm_connected_test.cpp
  description: 'Evidence from src/tests/unit/cpp/is_stm_connected_test.cpp: is stm
    connected test'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/tests/unit/cpp/heartbeat_stm_test.cpp
  description: 'Evidence from src/tests/unit/cpp/heartbeat_stm_test.cpp: heartbeat
    stm test'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/CAN/CAN_test/app_threadx.c
  description: 'Evidence from src/CAN/CAN_test/app_threadx.c: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/CAN/CAN_test/app_threadx.h
  description: 'Evidence from src/CAN/CAN_test/app_threadx.h: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/vl53l5cx_platform_stm32.h
  description: 'Evidence from src/stm/Core/Inc/vl53l5cx_platform_stm32.h: vl53l5cx
    platform stm32'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/stm32_can_benchmark.h
  description: 'Evidence from src/stm/Core/Inc/stm32_can_benchmark.h: stm32 can benchmark'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/stm32u5xx_hal_conf.h
  description: 'Evidence from src/stm/Core/Inc/stm32u5xx_hal_conf.h: stm32u5xx hal
    conf'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/stm32_assert.h
  description: 'Evidence from src/stm/Core/Inc/stm32_assert.h: stm32 assert'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/stm32u5xx_it.h
  description: 'Evidence from src/stm/Core/Inc/stm32u5xx_it.h: stm32u5xx it'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/app_threadx.h
  description: 'Evidence from src/stm/Core/Inc/app_threadx.h: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/stm32_can_benchmark.c
  description: 'Evidence from src/stm/Core/Src/stm32_can_benchmark.c: stm32 can benchmark'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/vl53l5cx_platform_stm32.c
  description: 'Evidence from src/stm/Core/Src/vl53l5cx_platform_stm32.c: vl53l5cx
    platform stm32'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/app_threadx.c
  description: 'Evidence from src/stm/Core/Src/app_threadx.c: app threadx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/system_stm32u5xx.c
  description: 'Evidence from src/stm/Core/Src/system_stm32u5xx.c: system stm32u5xx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/stm32u5xx_hal_timebase_tim.c
  description: 'Evidence from src/stm/Core/Src/stm32u5xx_hal_timebase_tim.c: stm32u5xx
    hal timebase tim'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/stm32u5xx_it.c
  description: 'Evidence from src/stm/Core/Src/stm32u5xx_it.c: stm32u5xx it'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/stm32u5xx_hal_msp.c
  description: 'Evidence from src/stm/Core/Src/stm32u5xx_hal_msp.c: stm32u5xx hal
    msp'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/AZURE_RTOS/App/app_azure_rtos_config.h
  description: 'Evidence from src/stm/AZURE_RTOS/App/app_azure_rtos_config.h: app
    azure rtos config'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/AZURE_RTOS/App/app_azure_rtos.h
  description: 'Evidence from src/stm/AZURE_RTOS/App/app_azure_rtos.h: app azure rtos'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/AZURE_RTOS/App/app_azure_rtos.c
  description: 'Evidence from src/stm/AZURE_RTOS/App/app_azure_rtos.c: app azure rtos'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/system_stm32u5xx.h
  description: 'Evidence from src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/system_stm32u5xx.h:
    system stm32u5xx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u585xx.h
  description: 'Evidence from src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u585xx.h:
    stm32u585xx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u5xx.h
  description: 'Evidence from src/stm/Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u5xx.h:
    stm32u5xx'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpgpio.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpgpio.h:
    stm32u5xx ll lpgpio'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim.h:
    stm32u5xx hal tim'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gtzc.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gtzc.h:
    stm32u5xx hal gtzc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h:
    stm32u5xx hal pcd ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_icache.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_icache.h:
    stm32u5xx ll icache'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_tim.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_tim.h:
    stm32u5xx ll tim'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_icache.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_icache.h:
    stm32u5xx hal icache'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_i2c.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_i2c.h:
    stm32u5xx ll i2c'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_spi.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_spi.h:
    stm32u5xx ll spi'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_bus.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_bus.h:
    stm32u5xx ll bus'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio_ex.h:
    stm32u5xx hal gpio ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart_ex.h:
    stm32u5xx hal uart ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr_ex.h:
    stm32u5xx hal pwr ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_uart.h:
    stm32u5xx hal uart'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c_ex.h:
    stm32u5xx hal i2c ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_i2c.h:
    stm32u5xx hal i2c'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_mdf.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_mdf.h:
    stm32u5xx hal mdf'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usb.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usb.h:
    stm32u5xx ll usb'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usart.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_usart.h:
    stm32u5xx ll usart'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_pwr.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_pwr.h:
    stm32u5xx ll pwr'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_system.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_system.h:
    stm32u5xx ll system'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_tim_ex.h:
    stm32u5xx hal tim ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_rcc.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_rcc.h:
    stm32u5xx ll rcc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dlyb.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dlyb.h:
    stm32u5xx ll dlyb'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_utils.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_utils.h:
    stm32u5xx ll utils'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpuart.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_lpuart.h:
    stm32u5xx ll lpuart'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc_ex.h:
    stm32u5xx hal rcc ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_ucpd.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_ucpd.h:
    stm32u5xx ll ucpd'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma_ex.h:
    stm32u5xx hal dma ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h:
    stm32u5xx hal pcd'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash.h:
    stm32u5xx hal flash'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dma.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_dma.h:
    stm32u5xx ll dma'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_exti.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_exti.h:
    stm32u5xx ll exti'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_def.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_def.h:
    stm32u5xx hal def'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_dma.h:
    stm32u5xx hal dma'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_cortex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_cortex.h:
    stm32u5xx hal cortex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal.h:
    stm32u5xx hal'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_gpio.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_gpio.h:
    stm32u5xx ll gpio'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi_ex.h:
    stm32u5xx hal spi ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pwr.h:
    stm32u5xx hal pwr'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_flash_ex.h:
    stm32u5xx hal flash ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_spi.h:
    stm32u5xx hal spi'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_cortex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_cortex.h:
    stm32u5xx ll cortex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_ospi.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_ospi.h:
    stm32u5xx hal ospi'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_gpio.h:
    stm32u5xx hal gpio'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_exti.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_exti.h:
    stm32u5xx hal exti'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_rcc.h:
    stm32u5xx hal rcc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_crs.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_ll_crs.h:
    stm32u5xx ll crs'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h:
    stm32 hal legacy'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c:
    stm32u5xx hal i2c ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c:
    stm32u5xx hal exti'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c:
    stm32u5xx hal pwr ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_gpio.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_gpio.c:
    stm32u5xx ll gpio'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c:
    stm32u5xx hal spi ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c:
    stm32u5xx hal tim'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c:
    stm32u5xx hal pcd ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c:
    stm32u5xx hal uart'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c:
    stm32u5xx hal dma'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c:
    stm32u5xx hal uart ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c:
    stm32u5xx hal icache'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c:
    stm32u5xx hal tim ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_ucpd.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_ucpd.c:
    stm32u5xx ll ucpd'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c:
    stm32u5xx hal cortex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_exti.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_exti.c:
    stm32u5xx ll exti'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c:
    stm32u5xx hal i2c'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c:
    stm32u5xx hal spi'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c:
    stm32u5xx hal pcd'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma.c:
    stm32u5xx ll dma'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c:
    stm32u5xx hal gtzc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c:
    stm32u5xx ll usb'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c:
    stm32u5xx hal'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c:
    stm32u5xx hal rcc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c:
    stm32u5xx hal pwr'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mdf.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_mdf.c:
    stm32u5xx hal mdf'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c:
    stm32u5xx hal gpio'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c:
    stm32u5xx hal flash ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c:
    stm32u5xx hal dma ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c:
    stm32u5xx hal flash'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c:
    stm32u5xx hal ospi'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_utils.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_utils.c:
    stm32u5xx ll utils'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c:
    stm32u5xx ll dlyb'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c:
    stm32u5xx hal rcc ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpgpio.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpgpio.c:
    stm32u5xx ll lpgpio'
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects end-to-end integration test results, video recordings, and system logs that demonstrate all startup time requirements are met.

**Evidence Status:** Pending - end-to-end integration test to be performed during final system integration phase.
