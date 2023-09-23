#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#include "main.h"       // Provides STM32CubeMx API and defines.
#include "cmsis_os.h"   // Provides FreeRTOS API

typedef struct _ApplicationConfig{
	UART_HandleTypeDef* UartTcTmHandle;
	TIM_HandleTypeDef*  PwmTimerHandle;
	TIM_HandleTypeDef* TachoTimerHandlers[4];

	// MPU9250
	//I2C_HandleTypeDef *MPU9250I2CHandle;

	// More HAL descriptors here...
} ApplicationConfig;

#endif // APPLICATION_CONFIG_H
