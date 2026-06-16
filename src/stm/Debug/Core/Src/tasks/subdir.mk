################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/tasks/task_aeb.c \
../Core/Src/tasks/task_battery.c \
../Core/Src/tasks/task_can_rx.c \
../Core/Src/tasks/task_cruise_control.c \
../Core/Src/tasks/task_environment.c \
../Core/Src/tasks/task_heartbeat.c \
../Core/Src/tasks/task_imu.c \
../Core/Src/tasks/task_indicator.c \
../Core/Src/tasks/task_speed.c \
../Core/Src/tasks/task_srf08.c \
../Core/Src/tasks/task_tof.c 

OBJS += \
./Core/Src/tasks/task_aeb.o \
./Core/Src/tasks/task_battery.o \
./Core/Src/tasks/task_can_rx.o \
./Core/Src/tasks/task_cruise_control.o \
./Core/Src/tasks/task_environment.o \
./Core/Src/tasks/task_heartbeat.o \
./Core/Src/tasks/task_imu.o \
./Core/Src/tasks/task_indicator.o \
./Core/Src/tasks/task_speed.o \
./Core/Src/tasks/task_srf08.o \
./Core/Src/tasks/task_tof.o 

C_DEPS += \
./Core/Src/tasks/task_aeb.d \
./Core/Src/tasks/task_battery.d \
./Core/Src/tasks/task_can_rx.d \
./Core/Src/tasks/task_cruise_control.d \
./Core/Src/tasks/task_environment.d \
./Core/Src/tasks/task_heartbeat.d \
./Core/Src/tasks/task_imu.d \
./Core/Src/tasks/task_indicator.d \
./Core/Src/tasks/task_speed.d \
./Core/Src/tasks/task_srf08.d \
./Core/Src/tasks/task_tof.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/tasks/%.o Core/Src/tasks/%.su Core/Src/tasks/%.cyclo: ../Core/Src/tasks/%.c Core/Src/tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32U585xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Middlewares/ST/threadx/utility -I../Drivers/VL53L5CX/inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-tasks

clean-Core-2f-Src-2f-tasks:
	-$(RM) ./Core/Src/tasks/task_aeb.cyclo ./Core/Src/tasks/task_aeb.d ./Core/Src/tasks/task_aeb.o ./Core/Src/tasks/task_aeb.su ./Core/Src/tasks/task_battery.cyclo ./Core/Src/tasks/task_battery.d ./Core/Src/tasks/task_battery.o ./Core/Src/tasks/task_battery.su ./Core/Src/tasks/task_can_rx.cyclo ./Core/Src/tasks/task_can_rx.d ./Core/Src/tasks/task_can_rx.o ./Core/Src/tasks/task_can_rx.su ./Core/Src/tasks/task_cruise_control.cyclo ./Core/Src/tasks/task_cruise_control.d ./Core/Src/tasks/task_cruise_control.o ./Core/Src/tasks/task_cruise_control.su ./Core/Src/tasks/task_environment.cyclo ./Core/Src/tasks/task_environment.d ./Core/Src/tasks/task_environment.o ./Core/Src/tasks/task_environment.su ./Core/Src/tasks/task_heartbeat.cyclo ./Core/Src/tasks/task_heartbeat.d ./Core/Src/tasks/task_heartbeat.o ./Core/Src/tasks/task_heartbeat.su ./Core/Src/tasks/task_imu.cyclo ./Core/Src/tasks/task_imu.d ./Core/Src/tasks/task_imu.o ./Core/Src/tasks/task_imu.su ./Core/Src/tasks/task_indicator.cyclo ./Core/Src/tasks/task_indicator.d ./Core/Src/tasks/task_indicator.o ./Core/Src/tasks/task_indicator.su ./Core/Src/tasks/task_speed.cyclo ./Core/Src/tasks/task_speed.d ./Core/Src/tasks/task_speed.o ./Core/Src/tasks/task_speed.su ./Core/Src/tasks/task_srf08.cyclo ./Core/Src/tasks/task_srf08.d ./Core/Src/tasks/task_srf08.o ./Core/Src/tasks/task_srf08.su ./Core/Src/tasks/task_tof.cyclo ./Core/Src/tasks/task_tof.d ./Core/Src/tasks/task_tof.o ./Core/Src/tasks/task_tof.su

.PHONY: clean-Core-2f-Src-2f-tasks

