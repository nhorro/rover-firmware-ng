#include "Application.h"
#include "main.h"
#include "cmsis_os.h"

#include <cstring>

Application App;

Application::Application()
{

}

Application::~Application()
{

}

void Application::handle_packet(const uint8_t* payload, uint8_t n)
{
	ReceivedPackets++;

	// FIXME: check for available mailboxes
	if ( true )
	{
		LastPacketStatus = LastPacketStatusCode::Ok;

		std::memcpy(&CommandMailbox[CurrentCommandMailboxIdx], payload, n);

		App.LasOsResult1 = osMessageQueuePut(
				App.Config->CommandReceiverQueueHandle,
				&CurrentCommandMailboxIdx,
				0, // priority ignored.
				0 // Required because running in interrupt
		);

		if ( osStatus_t::osOK == App.LasOsResult1 )
		{
			// Next mailbox
			CurrentCommandMailboxIdx++;
			if (CurrentCommandMailboxIdx == CommandMailboxSize)
			{
				CurrentCommandMailboxIdx = 0;
			}
		}
	}
}

void Application::set_error(error_code ec)
{
	if (ec == invalid_length )
	{
		LastPacketStatus = LastPacketStatusCode::InvalidLength;
	}
	else if(ec == bad_crc)
	{
		LastPacketStatus = LastPacketStatusCode::BadCRC;
	}
}


void setPWM(TIM_HandleTypeDef* timer, uint32_t channel, uint16_t pulse)
{
	HAL_TIM_PWM_Stop(timer, channel); // stop generation of pwm
	TIM_OC_InitTypeDef sConfigOC;
	//timer->Init.Period = period; // set the period duration
	timer->Init.Period = 1000;
	HAL_TIM_PWM_Init(timer); // reinititialise with new period value
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse; // set the pulse duration
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, channel);
	HAL_TIM_PWM_Start(timer, channel); // start pwm generation
}

void Application::UpdateMotorThrottle(uint32_t flags)
{
	static constexpr const uint32_t MaxThrottlePulseWidth = 1000.0;

	if (flags & 1)
	{
		setPWM(Config->PwmTimerHandle,TIM_CHANNEL_3,MotorThrottles[0]*MaxThrottlePulseWidth);
	}

	if (flags & 2)
	{
		setPWM(Config->PwmTimerHandle,TIM_CHANNEL_4,MotorThrottles[1]*MaxThrottlePulseWidth);
	}
}





void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case TACHO1_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			App.TachometerTicks[0]++;
			//app.increment_wheel_ticks(0);
		} break;

		case TACHO2_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			App.TachometerTicks[1]++;
			//app.increment_wheel_ticks(1);
		} break;

		case TACHO3_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			App.TachometerTicks[2]++;
			//app.increment_wheel_ticks(2);
		} break;

		case TACHO4_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			App.TachometerTicks[3]++;
			//app.increment_wheel_ticks(3);
		} break;

		default: {
			__NOP();
		}
	};
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle)
{
	if(UartHandle == App.Config->UartTcTmHandle)
	{
		HAL_UART_Receive_IT(UartHandle, &App.RxBuf, 1);
		App.feed(App.RxBuf);
		//HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		//HAL_UART_Transmit_IT(UartHandle,reinterpret_cast<const uint8_t*>(MESSAGE_STR), 6);
	}

}



// Función de control PID
float PID_Controller(float measuredSpeed) {

}



void ApplicationMain(const ApplicationConfig* Config)
{
	// FIXME: Cuidado con esto. Las tares no deberìan arrancar hasta que no estè la configuraciòn.
	App.Config = Config;

	static const uint32_t SpeedUpdateScaler = 10;
	uint32_t SpeedUpdateCounter = 0;

	// Inicio
	HAL_UART_Receive_IT(App.Config->UartTcTmHandle, &App.RxBuf, 1);
	for(;;)
	{
		// Main loop
		if (++SpeedUpdateCounter==SpeedUpdateScaler)
		{
			uint32_t CurrentTimeInMs = HAL_GetTick();
			uint32_t Dt = CurrentTimeInMs - App.OnBoardTime;
			App.OnBoardTime = CurrentTimeInMs;

			uint32_t DeltaTachometerTicks[4] = {
				App.TachometerTicks[0] - App.TachometerTicksPreviousCycle[0],
				App.TachometerTicks[1] - App.TachometerTicksPreviousCycle[1],
				App.TachometerTicks[2] - App.TachometerTicksPreviousCycle[2],
				App.TachometerTicks[3] - App.TachometerTicksPreviousCycle[3]
			};
			App.TachometerTicksPreviousCycle[0]=App.TachometerTicks[0];
			App.TachometerTicksPreviousCycle[1]=App.TachometerTicks[1];
			App.TachometerTicksPreviousCycle[2]=App.TachometerTicks[2];
			App.TachometerTicksPreviousCycle[3]=App.TachometerTicks[3];

			constexpr const uint32_t TicksPerRevolution = 20;
			App.TachometerMeasuredSpeed[0] = DeltaTachometerTicks[0];
			App.TachometerMeasuredSpeed[1] = DeltaTachometerTicks[1];
			App.TachometerMeasuredSpeed[2] = DeltaTachometerTicks[2];
			App.TachometerMeasuredSpeed[3] = DeltaTachometerTicks[3];
			SpeedUpdateCounter = 0;
		}

		// PID mode
		if (App.MotorSetpointSpeeds[0] > 0.0)
		{
			float AverageMeasuredSpeed = (App.TachometerMeasuredSpeed[0]+App.TachometerMeasuredSpeed[1])/2;
			App.MotorThrottles[0] = App.PID[0].Process(App.MotorSetpointSpeeds[0], AverageMeasuredSpeed);
			App.UpdateMotorThrottle(1);
		}

		if (App.MotorSetpointSpeeds[1] > 0.0)
		{
			float AverageMeasuredSpeed = (App.TachometerMeasuredSpeed[2]+App.TachometerMeasuredSpeed[3])/2;
			App.MotorThrottles[1] = App.PID[1].Process(App.MotorSetpointSpeeds[1], AverageMeasuredSpeed);
			App.UpdateMotorThrottle(2);
		}



		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(1000/100);
	}
}
