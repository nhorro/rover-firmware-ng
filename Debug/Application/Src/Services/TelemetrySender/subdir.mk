################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/Src/Services/TelemetrySender/TelemetrySender.cpp 

OBJS += \
./Application/Src/Services/TelemetrySender/TelemetrySender.o 

CPP_DEPS += \
./Application/Src/Services/TelemetrySender/TelemetrySender.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Src/Services/TelemetrySender/%.o Application/Src/Services/TelemetrySender/%.su: ../Application/Src/Services/TelemetrySender/%.cpp Application/Src/Services/TelemetrySender/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Application/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-Src-2f-Services-2f-TelemetrySender

clean-Application-2f-Src-2f-Services-2f-TelemetrySender:
	-$(RM) ./Application/Src/Services/TelemetrySender/TelemetrySender.d ./Application/Src/Services/TelemetrySender/TelemetrySender.o ./Application/Src/Services/TelemetrySender/TelemetrySender.su

.PHONY: clean-Application-2f-Src-2f-Services-2f-TelemetrySender

