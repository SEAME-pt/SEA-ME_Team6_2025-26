################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_threadx.c \
../Core/Src/gpio.c \
../Core/Src/hts221.c \
../Core/Src/i2c.c \
../Core/Src/i2c_scanner.c \
../Core/Src/icache.c \
../Core/Src/iis2mdc.c \
../Core/Src/ism330dhcx.c \
../Core/Src/lcd1602.c \
../Core/Src/lps22hh.c \
../Core/Src/main.c \
../Core/Src/mcp2515.c \
../Core/Src/mdf.c \
../Core/Src/motor_control.c \
../Core/Src/octospi.c \
../Core/Src/retarget.c \
../Core/Src/servo.c \
../Core/Src/speedometer.c \
../Core/Src/spi.c \
../Core/Src/stm32_can_benchmark.c \
../Core/Src/stm32u5xx_hal_msp.c \
../Core/Src/stm32u5xx_hal_timebase_tim.c \
../Core/Src/stm32u5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u5xx.c \
../Core/Src/tim.c \
../Core/Src/ucpd.c \
../Core/Src/usart.c \
../Core/Src/usb_otg.c \
../Core/Src/veml6030.c \
../Core/Src/vl53l5cx_driver.c \
../Core/Src/vl53l5cx_platform_stm32.c \
../Core/Src/vl53l5cx_simple.c 

S_UPPER_SRCS += \
../Core/Src/tx_initialize_low_level.S 

OBJS += \
./Core/Src/app_threadx.o \
./Core/Src/gpio.o \
./Core/Src/hts221.o \
./Core/Src/i2c.o \
./Core/Src/i2c_scanner.o \
./Core/Src/icache.o \
./Core/Src/iis2mdc.o \
./Core/Src/ism330dhcx.o \
./Core/Src/lcd1602.o \
./Core/Src/lps22hh.o \
./Core/Src/main.o \
./Core/Src/mcp2515.o \
./Core/Src/mdf.o \
./Core/Src/motor_control.o \
./Core/Src/octospi.o \
./Core/Src/retarget.o \
./Core/Src/servo.o \
./Core/Src/speedometer.o \
./Core/Src/spi.o \
./Core/Src/stm32_can_benchmark.o \
./Core/Src/stm32u5xx_hal_msp.o \
./Core/Src/stm32u5xx_hal_timebase_tim.o \
./Core/Src/stm32u5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u5xx.o \
./Core/Src/tim.o \
./Core/Src/tx_initialize_low_level.o \
./Core/Src/ucpd.o \
./Core/Src/usart.o \
./Core/Src/usb_otg.o \
./Core/Src/veml6030.o \
./Core/Src/vl53l5cx_driver.o \
./Core/Src/vl53l5cx_platform_stm32.o \
./Core/Src/vl53l5cx_simple.o 

S_UPPER_DEPS += \
./Core/Src/tx_initialize_low_level.d 

C_DEPS += \
./Core/Src/app_threadx.d \
./Core/Src/gpio.d \
./Core/Src/hts221.d \
./Core/Src/i2c.d \
./Core/Src/i2c_scanner.d \
./Core/Src/icache.d \
./Core/Src/iis2mdc.d \
./Core/Src/ism330dhcx.d \
./Core/Src/lcd1602.d \
./Core/Src/lps22hh.d \
./Core/Src/main.d \
./Core/Src/mcp2515.d \
./Core/Src/mdf.d \
./Core/Src/motor_control.d \
./Core/Src/octospi.d \
./Core/Src/retarget.d \
./Core/Src/servo.d \
./Core/Src/speedometer.d \
./Core/Src/spi.d \
./Core/Src/stm32_can_benchmark.d \
./Core/Src/stm32u5xx_hal_msp.d \
./Core/Src/stm32u5xx_hal_timebase_tim.d \
./Core/Src/stm32u5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u5xx.d \
./Core/Src/tim.d \
./Core/Src/ucpd.d \
./Core/Src/usart.d \
./Core/Src/usb_otg.d \
./Core/Src/veml6030.d \
./Core/Src/vl53l5cx_driver.d \
./Core/Src/vl53l5cx_platform_stm32.d \
./Core/Src/vl53l5cx_simple.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32U585xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Drivers/VL53L5CX/inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o: ../Core/Src/%.S Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -DDEBUG -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../AZURE_RTOS/App -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Drivers/CMSIS/Include -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_threadx.cyclo ./Core/Src/app_threadx.d ./Core/Src/app_threadx.o ./Core/Src/app_threadx.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/hts221.cyclo ./Core/Src/hts221.d ./Core/Src/hts221.o ./Core/Src/hts221.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/i2c_scanner.cyclo ./Core/Src/i2c_scanner.d ./Core/Src/i2c_scanner.o ./Core/Src/i2c_scanner.su ./Core/Src/icache.cyclo ./Core/Src/icache.d ./Core/Src/icache.o ./Core/Src/icache.su ./Core/Src/iis2mdc.cyclo ./Core/Src/iis2mdc.d ./Core/Src/iis2mdc.o ./Core/Src/iis2mdc.su ./Core/Src/ism330dhcx.cyclo ./Core/Src/ism330dhcx.d ./Core/Src/ism330dhcx.o ./Core/Src/ism330dhcx.su ./Core/Src/lcd1602.cyclo ./Core/Src/lcd1602.d ./Core/Src/lcd1602.o ./Core/Src/lcd1602.su ./Core/Src/lps22hh.cyclo ./Core/Src/lps22hh.d ./Core/Src/lps22hh.o ./Core/Src/lps22hh.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mcp2515.cyclo ./Core/Src/mcp2515.d ./Core/Src/mcp2515.o ./Core/Src/mcp2515.su ./Core/Src/mdf.cyclo ./Core/Src/mdf.d ./Core/Src/mdf.o ./Core/Src/mdf.su ./Core/Src/motor_control.cyclo ./Core/Src/motor_control.d ./Core/Src/motor_control.o ./Core/Src/motor_control.su ./Core/Src/octospi.cyclo ./Core/Src/octospi.d ./Core/Src/octospi.o ./Core/Src/octospi.su ./Core/Src/retarget.cyclo ./Core/Src/retarget.d ./Core/Src/retarget.o ./Core/Src/retarget.su ./Core/Src/servo.cyclo ./Core/Src/servo.d ./Core/Src/servo.o ./Core/Src/servo.su ./Core/Src/speedometer.cyclo ./Core/Src/speedometer.d ./Core/Src/speedometer.o ./Core/Src/speedometer.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32_can_benchmark.cyclo ./Core/Src/stm32_can_benchmark.d ./Core/Src/stm32_can_benchmark.o ./Core/Src/stm32_can_benchmark.su ./Core/Src/stm32u5xx_hal_msp.cyclo ./Core/Src/stm32u5xx_hal_msp.d ./Core/Src/stm32u5xx_hal_msp.o ./Core/Src/stm32u5xx_hal_msp.su ./Core/Src/stm32u5xx_hal_timebase_tim.cyclo ./Core/Src/stm32u5xx_hal_timebase_tim.d ./Core/Src/stm32u5xx_hal_timebase_tim.o ./Core/Src/stm32u5xx_hal_timebase_tim.su ./Core/Src/stm32u5xx_it.cyclo ./Core/Src/stm32u5xx_it.d ./Core/Src/stm32u5xx_it.o ./Core/Src/stm32u5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u5xx.cyclo ./Core/Src/system_stm32u5xx.d ./Core/Src/system_stm32u5xx.o ./Core/Src/system_stm32u5xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/tx_initialize_low_level.d ./Core/Src/tx_initialize_low_level.o ./Core/Src/ucpd.cyclo ./Core/Src/ucpd.d ./Core/Src/ucpd.o ./Core/Src/ucpd.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/usb_otg.cyclo ./Core/Src/usb_otg.d ./Core/Src/usb_otg.o ./Core/Src/usb_otg.su ./Core/Src/veml6030.cyclo ./Core/Src/veml6030.d ./Core/Src/veml6030.o ./Core/Src/veml6030.su ./Core/Src/vl53l5cx_driver.cyclo ./Core/Src/vl53l5cx_driver.d ./Core/Src/vl53l5cx_driver.o ./Core/Src/vl53l5cx_driver.su ./Core/Src/vl53l5cx_platform_stm32.cyclo ./Core/Src/vl53l5cx_platform_stm32.d ./Core/Src/vl53l5cx_platform_stm32.o ./Core/Src/vl53l5cx_platform_stm32.su ./Core/Src/vl53l5cx_simple.cyclo ./Core/Src/vl53l5cx_simple.d ./Core/Src/vl53l5cx_simple.o ./Core/Src/vl53l5cx_simple.su

.PHONY: clean-Core-2f-Src

