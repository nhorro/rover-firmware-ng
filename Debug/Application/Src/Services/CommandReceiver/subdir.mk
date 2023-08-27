################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/Src/Services/CommandReceiver/CommandReceiver.cpp 

OBJS += \
./Application/Src/Services/CommandReceiver/CommandReceiver.o 

CPP_DEPS += \
./Application/Src/Services/CommandReceiver/CommandReceiver.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Src/Services/CommandReceiver/%.o Application/Src/Services/CommandReceiver/%.su: ../Application/Src/Services/CommandReceiver/%.cpp Application/Src/Services/CommandReceiver/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Application/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-Src-2f-Services-2f-CommandReceiver

clean-Application-2f-Src-2f-Services-2f-CommandReceiver:
	-$(RM) ./Application/Src/Services/CommandReceiver/CommandReceiver.d ./Application/Src/Services/CommandReceiver/CommandReceiver.o ./Application/Src/Services/CommandReceiver/CommandReceiver.su

.PHONY: clean-Application-2f-Src-2f-Services-2f-CommandReceiver

