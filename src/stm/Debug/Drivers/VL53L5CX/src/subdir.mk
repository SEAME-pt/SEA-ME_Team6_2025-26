################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/VL53L5CX/src/platform.c \
../Drivers/VL53L5CX/src/vl53l5cx_api.c \
../Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.c 

OBJS += \
./Drivers/VL53L5CX/src/platform.o \
./Drivers/VL53L5CX/src/vl53l5cx_api.o \
./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.o 

C_DEPS += \
./Drivers/VL53L5CX/src/platform.d \
./Drivers/VL53L5CX/src/vl53l5cx_api.d \
./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/VL53L5CX/src/%.o Drivers/VL53L5CX/src/%.su Drivers/VL53L5CX/src/%.cyclo: ../Drivers/VL53L5CX/src/%.c Drivers/VL53L5CX/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32U585xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Middlewares/ST/threadx/utility -I../Drivers/VL53L5CX/inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-VL53L5CX-2f-src

clean-Drivers-2f-VL53L5CX-2f-src:
	-$(RM) ./Drivers/VL53L5CX/src/platform.cyclo ./Drivers/VL53L5CX/src/platform.d ./Drivers/VL53L5CX/src/platform.o ./Drivers/VL53L5CX/src/platform.su ./Drivers/VL53L5CX/src/vl53l5cx_api.cyclo ./Drivers/VL53L5CX/src/vl53l5cx_api.d ./Drivers/VL53L5CX/src/vl53l5cx_api.o ./Drivers/VL53L5CX/src/vl53l5cx_api.su ./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.cyclo ./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.d ./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.o ./Drivers/VL53L5CX/src/vl53l5cx_plugin_motion_indicator.su

.PHONY: clean-Drivers-2f-VL53L5CX-2f-src

