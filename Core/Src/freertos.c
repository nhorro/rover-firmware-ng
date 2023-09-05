/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Application/Inc/ApplicationConfig.h"
extern void ApplicationMain(const ApplicationConfig* Config);

#include "tim.h"
#include "usart.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticQueue_t osStaticMessageQDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
ApplicationConfig Config;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for commandReceiver */
osThreadId_t commandReceiverHandle;
const osThreadAttr_t commandReceiver_attributes = {
  .name = "commandReceiver",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for telemetrySender */
osThreadId_t telemetrySenderHandle;
const osThreadAttr_t telemetrySender_attributes = {
  .name = "telemetrySender",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CommandReceiverQueue */
osMessageQueueId_t CommandReceiverQueueHandle;
uint8_t CommandReceiverQueueBuffer[ 16 * sizeof( uint8_t ) ];
osStaticMessageQDef_t CommandReceiverQueueControlBlock;
const osMessageQueueAttr_t CommandReceiverQueue_attributes = {
  .name = "CommandReceiverQueue",
  .cb_mem = &CommandReceiverQueueControlBlock,
  .cb_size = sizeof(CommandReceiverQueueControlBlock),
  .mq_mem = &CommandReceiverQueueBuffer,
  .mq_size = sizeof(CommandReceiverQueueBuffer)
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void DefaultTaskMain(void *argument);
extern void CommandReceiverTaskMain(void *argument);
extern void TelemetrySenderTaskMain(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of CommandReceiverQueue */
  CommandReceiverQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &CommandReceiverQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(DefaultTaskMain, NULL, &defaultTask_attributes);

  /* creation of commandReceiver */
  commandReceiverHandle = osThreadNew(CommandReceiverTaskMain, NULL, &commandReceiver_attributes);

  /* creation of telemetrySender */
  telemetrySenderHandle = osThreadNew(TelemetrySenderTaskMain, NULL, &telemetrySender_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_DefaultTaskMain */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_DefaultTaskMain */
void DefaultTaskMain(void *argument)
{
  /* USER CODE BEGIN DefaultTaskMain */
	Config.UartTcTmHandle = &huart3; // Development mode (USB)
	//Config.UartTcTmHandle = &huart1; // Production mode (Rx/Tx pins)
	Config.PwmTimerHandle = &htim2;
	Config.CommandReceiverQueueHandle = CommandReceiverQueueHandle;
	Config.MPU9250I2CHandle = &hi2c1;
	ApplicationMain(&Config);
  /* USER CODE END DefaultTaskMain */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

