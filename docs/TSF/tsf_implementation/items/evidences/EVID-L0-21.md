---
id: EVID-L0-21
header: 'Evidence: Complete TSF Workflow Execution'
text: 'Evidence demonstrating successful execution of the complete TSF automation
  workflow. Includes execution logs of open_check_sync_update_validate_run_publish_tsfrequirements.py
  with --all flag, showing all three phases (check, sync, validate) completing successfully.

  '
level: '1.21'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py
- type: file
  path: docs/demos/coverage-report-sprint7.png
- type: file
  path: docs/demos/OTA-Action.png
- type: file
  path: docs/demos/CI-CD-tests-sprint7.png
- type: file
  path: docs/demos/LCOV_coverage_report_QT - sprint 6.jpeg
- type: file
  path: docs/demos/coverage-report-srint8.png
- type: file
  path: docs/demos/rpi5-thermal-stress-test-.png
- type: file
  path: docs/demos/CAN-test.gif
- type: file
  path: docs/demos/first-code-coverage.png
- type: file
  path: docs/demos/LCOV_coverage_report - sprint6.jpeg
- type: file
  path: docs/guides/CAN-test-guide.md
- type: file
  path: docs/guides/github-actions-guide.md
- type: file
  path: docs/guides/AGL_ Joystick_installation_and_test.md
- type: file
  path: docs/guides/Testing-Framework-Decision.md
- type: file
  path: docs/guides/Software-Tests-guide.md
- type: file
  path: docs/guides/CAN_test/CAN_Test.c
- type: file
  path: docs/guides/CAN_test/CAN-test.gif
- type: file
  path: docs/guides/OTA/OTA_Comparison_Tests.md
- type: file
  path: docs/guides/Car_Architecture/Circuit Diagram.png
- type: file
  path: docs/guides/Car_Architecture/Circuit Documentation.md
- type: file
  path: docs/presentations/threadX/2025.11 Eclipse ThreadX_ A recipe for success with
    four simple ingredients.pdf
- type: file
  path: src/ota/scripts/smoke-test.sh
- type: file
  path: src/tests/utils/scripts/run_python_coverage.sh
- type: file
  path: src/tests/unit/python/test_joystick_control.py
- type: file
  path: src/tests/unit/python/test_calculator.py
- type: file
  path: src/tests/unit/cpp/temperatureprovider_test.cpp
- type: file
  path: src/tests/unit/cpp/battery_test.cpp
- type: file
  path: src/tests/unit/cpp/servo_testable.h
- type: file
  path: src/tests/unit/cpp/test_lps22hh.cpp
- type: file
  path: src/tests/unit/cpp/motor_control_testable.c
- type: file
  path: src/tests/unit/cpp/is_stm_connected_test.cpp
- type: file
  path: src/tests/unit/cpp/emergency_stop_test.cpp
- type: file
  path: src/tests/unit/cpp/test_veml6030.cpp
- type: file
  path: src/tests/unit/cpp/servo_testable.c
- type: file
  path: src/tests/unit/cpp/test_integration.cpp
- type: file
  path: src/tests/unit/cpp/speedprovider_test.cpp
- type: file
  path: src/tests/unit/cpp/imu_accel_test.cpp
- type: file
  path: src/tests/unit/cpp/tof_testable.h
- type: file
  path: src/tests/unit/cpp/speedometer_testable.c
- type: file
  path: src/tests/unit/cpp/imu_gyro_test.cpp
- type: file
  path: src/tests/unit/cpp/emergency_stop_testable.h
- type: file
  path: src/tests/unit/cpp/emergency_stop_testable.c
- type: file
  path: src/tests/unit/cpp/speedometer_testable.h
- type: file
  path: src/tests/unit/cpp/test_servo.cpp
- type: file
  path: src/tests/unit/cpp/tof_distance_test.cpp
- type: file
  path: src/tests/unit/cpp/speed_test.cpp
- type: file
  path: src/tests/unit/cpp/environment_test.cpp
- type: file
  path: src/tests/unit/cpp/test_emergency_stop.cpp
- type: file
  path: src/tests/unit/cpp/imu_magn_test.cpp
- type: file
  path: src/tests/unit/cpp/test_motor_control.cpp
- type: file
  path: src/tests/unit/cpp/dispatcher_test.cpp
- type: file
  path: src/tests/unit/cpp/test_ism330dhcx.cpp
- type: file
  path: src/tests/unit/cpp/tof_testable.c
- type: file
  path: src/tests/unit/cpp/test_speedometer.cpp
- type: file
  path: src/tests/unit/cpp/can_decode_test.cpp
- type: file
  path: src/tests/unit/cpp/test_hts221.cpp
- type: file
  path: src/tests/unit/cpp/test_tof.cpp
- type: file
  path: src/tests/unit/cpp/motor_control_testable.h
- type: file
  path: src/tests/unit/cpp/joystick_test.cpp
- type: file
  path: src/tests/unit/cpp/test_iis2mdc.cpp
- type: file
  path: src/tests/unit/cpp/heartbeat_stm_test.cpp
- type: file
  path: src/tests/unit/cpp/timeprovider_test.cpp
- type: file
  path: src/tests/scripts/run_tests_docker.sh
- type: file
  path: src/tests/scripts/coverage.sh
- type: file
  path: src/tests/cmake/CoveragePython.cmake
- type: file
  path: src/tests/cmake/CoverageIndex.cmake
- type: file
  path: src/tests/cmake/CoverageCpp.cmake
- type: file
  path: src/CAN/CAN_test/CAN_Test.c
- type: file
  path: src/CAN/CAN_test/CAN-test.gif
- type: file
  path: src/stm/Combined_test.md
- type: file
  path: src/stm/Core/Inc/lcd1602.h
- type: file
  path: src/stm/Core/Src/lcd1602.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.


