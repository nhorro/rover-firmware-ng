################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Modules/CommandReceiverTask.cpp \
../Core/Src/Modules/TelemetrySenderTask.cpp 

OBJS += \
./Core/Src/Modules/CommandReceiverTask.o \
./Core/Src/Modules/TelemetrySenderTask.o 

CPP_DEPS += \
./Core/Src/Modules/CommandReceiverTask.d \
./Core/Src/Modules/TelemetrySenderTask.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Modules/%.o Core/Src/Modules/%.su: ../Core/Src/Modules/%.cpp Core/Src/Modules/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Modules

clean-Core-2f-Src-2f-Modules:
	-$(RM) ./Core/Src/Modules/CommandReceiverTask.d ./Core/Src/Modules/CommandReceiverTask.o ./Core/Src/Modules/CommandReceiverTask.su ./Core/Src/Modules/TelemetrySenderTask.d ./Core/Src/Modules/TelemetrySenderTask.o ./Core/Src/Modules/TelemetrySenderTask.su

.PHONY: clean-Core-2f-Src-2f-Modules

