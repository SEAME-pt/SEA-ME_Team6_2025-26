################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/threadx/utility/tx_execution_profile.c 

OBJS += \
./Middlewares/ST/threadx/utility/tx_execution_profile.o 

C_DEPS += \
./Middlewares/ST/threadx/utility/tx_execution_profile.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/threadx/utility/%.o Middlewares/ST/threadx/utility/%.su Middlewares/ST/threadx/utility/%.cyclo: ../Middlewares/ST/threadx/utility/%.c Middlewares/ST/threadx/utility/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32U585xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Middlewares/ST/threadx/utility -I../Drivers/VL53L5CX/inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-threadx-2f-utility

clean-Middlewares-2f-ST-2f-threadx-2f-utility:
	-$(RM) ./Middlewares/ST/threadx/utility/tx_execution_profile.cyclo ./Middlewares/ST/threadx/utility/tx_execution_profile.d ./Middlewares/ST/threadx/utility/tx_execution_profile.o ./Middlewares/ST/threadx/utility/tx_execution_profile.su

.PHONY: clean-Middlewares-2f-ST-2f-threadx-2f-utility

