#include "Application.h"
#include "main.h"
#include "cmsis_os.h"

#include <cstring>
#include <cmath>

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

//	GPIO_PinState in1 = (App.MotorControlModeFlags >> 31) & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
//	GPIO_PinState in2 = (App.MotorControlModeFlags >> 30) & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
//	GPIO_PinState in3 = (App.MotorControlModeFlags >> 29) & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
//	GPIO_PinState in4 = (App.MotorControlModeFlags >> 28) & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
//
//	HAL_GPIO_WritePin(L298N_IN1_GPIO_Port, L298N_IN1_Pin,in1); // GPIO_PIN_RESET GPIO_PIN_SET
//	HAL_GPIO_WritePin(L298N_IN2_GPIO_Port, L298N_IN2_Pin,in2);
//	HAL_GPIO_WritePin(L298N_IN3_GPIO_Port, L298N_IN3_Pin,in3);
//	HAL_GPIO_WritePin(L298N_IN4_GPIO_Port, L298N_IN4_Pin,in4);

	if (flags & 1)
	{
		if ( MotorThrottles[0] >= 0.0)
		{
			HAL_GPIO_WritePin(L298N_IN1_GPIO_Port, L298N_IN1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(L298N_IN2_GPIO_Port, L298N_IN2_Pin,GPIO_PIN_SET);

			setPWM(Config->PwmTimerHandle,TIM_CHANNEL_3,MotorThrottles[0]*MaxThrottlePulseWidth);
		}
		else
		{
			HAL_GPIO_WritePin(L298N_IN1_GPIO_Port, L298N_IN1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(L298N_IN2_GPIO_Port, L298N_IN2_Pin,GPIO_PIN_RESET);

			setPWM(Config->PwmTimerHandle,TIM_CHANNEL_3,-MotorThrottles[0]*MaxThrottlePulseWidth);
		}

	}

	if (flags & 2)
	{
		if ( MotorThrottles[1] >= 0.0)
		{
			HAL_GPIO_WritePin(L298N_IN3_GPIO_Port, L298N_IN3_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(L298N_IN4_GPIO_Port, L298N_IN4_Pin,GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(L298N_IN3_GPIO_Port, L298N_IN3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(L298N_IN4_GPIO_Port, L298N_IN4_Pin,GPIO_PIN_RESET);
		}

		setPWM(Config->PwmTimerHandle,TIM_CHANNEL_4,fabs(MotorThrottles[1])*MaxThrottlePulseWidth);
	}
}



static constexpr uint32_t MaximumTachometerDeltaInMs = 250;

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin)
{
	static constexpr const uint32_t TicksPerRevolution = 20;
	static constexpr const float DeltaTimePerRPM = 60.0*1000.0 / TicksPerRevolution;


	switch(GPIO_Pin)
	{
		case TACHO1_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			uint32_t CurrentTimeInMs = HAL_GetTick();
			uint32_t TachometerDeltaTimeSinceLastTickInMs = CurrentTimeInMs - App.TachometerLastUpdateInMs[0];
			App.TachometerLastUpdateInMs[0] = CurrentTimeInMs;
			if (TachometerDeltaTimeSinceLastTickInMs < MaximumTachometerDeltaInMs )
			{
				App.TachometerMeasuredSpeed[0] = DeltaTimePerRPM / TachometerDeltaTimeSinceLastTickInMs;
			}
			App.TachometerTicks[0]++;
		} break;

		case TACHO2_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			uint32_t CurrentTimeInMs = HAL_GetTick();
			uint32_t TachometerDeltaTimeSinceLastTickInMs = CurrentTimeInMs - App.TachometerLastUpdateInMs[1];
			App.TachometerLastUpdateInMs[1] = CurrentTimeInMs;
			if (TachometerDeltaTimeSinceLastTickInMs < MaximumTachometerDeltaInMs )
			{
				App.TachometerMeasuredSpeed[1] = DeltaTimePerRPM / TachometerDeltaTimeSinceLastTickInMs;
			}
			App.TachometerTicks[1]++;
		} break;

		case TACHO3_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			uint32_t CurrentTimeInMs = HAL_GetTick();
			uint32_t TachometerDeltaTimeSinceLastTickInMs = CurrentTimeInMs - App.TachometerLastUpdateInMs[2];
			App.TachometerLastUpdateInMs[2] = CurrentTimeInMs;
			if (TachometerDeltaTimeSinceLastTickInMs < MaximumTachometerDeltaInMs )
			{
				App.TachometerMeasuredSpeed[2] = DeltaTimePerRPM / TachometerDeltaTimeSinceLastTickInMs;
			}
			App.TachometerTicks[2]++;
		} break;

		case TACHO4_Pin: {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			uint32_t CurrentTimeInMs = HAL_GetTick();
			uint32_t TachometerDeltaTimeSinceLastTickInMs = CurrentTimeInMs - App.TachometerLastUpdateInMs[3];
			App.TachometerLastUpdateInMs[3] = CurrentTimeInMs;
			if (TachometerDeltaTimeSinceLastTickInMs < MaximumTachometerDeltaInMs )
			{
				App.TachometerMeasuredSpeed[3] = DeltaTimePerRPM / TachometerDeltaTimeSinceLastTickInMs;
			}
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


void ApplicationMain(const ApplicationConfig* Config)
{
	// FIXME: Cuidado con esto. Las tares no deberìan arrancar hasta que no estè la configuraciòn.
	App.Config = Config;

	// Inicio
	HAL_UART_Receive_IT(App.Config->UartTcTmHandle, &App.RxBuf, 1);

	// Shut down motors.
	App.UpdateMotorThrottle(3);
	for(;;)
	{
		// Main loop
		App.OnBoardTime = HAL_GetTick();


		// Detect if tachometers stopped moving.
		for(size_t i =0;i<4; i++)
		{
			if ( (App.OnBoardTime-App.TachometerLastUpdateInMs[i]) >= MaximumTachometerDeltaInMs )
			{
				App.TachometerMeasuredSpeed[i] = 0;
			}
		}


		// PID mode
		if (App.MotorControlModeFlags & Application::ControlModeFlags::ArmedPID)
		{

			float AverageMeasuredSpeed = (App.TachometerMeasuredSpeed[0]+App.TachometerMeasuredSpeed[1])/2;
			App.MotorThrottles[0] = App.PID[0].Process(App.MotorSetpointSpeeds[0], AverageMeasuredSpeed);

			AverageMeasuredSpeed = (App.TachometerMeasuredSpeed[2]+App.TachometerMeasuredSpeed[3])/2;
			App.MotorThrottles[1] = App.PID[1].Process(App.MotorSetpointSpeeds[1], AverageMeasuredSpeed);

			App.UpdateMotorThrottle(3);
		}


		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(1000/50.0); //App.MainControlLoopFrequency

	}
}
