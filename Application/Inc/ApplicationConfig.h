#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#include "main.h"
#include "cmsis_os.h"

typedef struct _ApplicationConfig{
	UART_HandleTypeDef* UartTcTmHandle;
	TIM_HandleTypeDef*  PwmTimerHandle;
	osMessageQueueId_t CommandReceiverQueueHandle;

	// MPU9250
	I2C_HandleTypeDef *MPU9250I2CHandle;
} ApplicationConfig;

#endif // APPLICATION_CONFIG_H
