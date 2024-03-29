#include "Actuators/L298NMotorController.h"
#include <cmath>

L298NMotorController::L298NMotorController()
	:
		MotorThrottles{0.0,0.0}
{


}

L298NMotorController::~L298NMotorController()
{

}


void L298NMotorController::Start()
{
	HAL_TIM_Base_Start(_HalRes.PWMOuts[0].PWMTimerHandle);
	// If each PWM has an independent timer, start the second one.
	if (_HalRes.PWMOuts[1].PWMTimerHandle != _HalRes.PWMOuts[0].PWMTimerHandle)
	{
		HAL_TIM_Base_Start(_HalRes.PWMOuts[1].PWMTimerHandle);
	}

}


void L298NMotorController::Shutdown()
{
	MotorThrottles[0] = 0;
	MotorThrottles[1] = 0;
	UpdateMotorThrottles(MotorControlFlags::Both);
}



void L298NMotorController::UpdateMotorThrottles(MotorControlFlags Flags)
{
	static constexpr const uint32_t MaxThrottlePulseWidth = 1000;

	for(size_t MotorIdx=0;MotorIdx<2;MotorIdx++)
	{
		if (Flags & (1<<MotorIdx))
		{
			// FIXME: Reject changes that are to similar to las value to prevent jitter
			//MotorThrottles[MotorIdx] = MotorThrottles[MotorIdx];

			// Clamp values
			if(MotorThrottles[MotorIdx]< -1.0)
				MotorThrottles[MotorIdx] = -1.0;
			else if(MotorThrottles[MotorIdx] > 1.0)
				MotorThrottles[MotorIdx] = 1.0;

			if ( MotorThrottles[MotorIdx] >= 0.0 )
			{
				HAL_GPIO_WritePin(
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN1 ].GPIOPort,
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN1 ].GPIOPin,
						GPIO_PIN_RESET
				);

				HAL_GPIO_WritePin(
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN2 ].GPIOPort,
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN2 ].GPIOPin,
						GPIO_PIN_SET
				);

			}
			else
			{
				HAL_GPIO_WritePin(
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN1 ].GPIOPort,
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN1 ].GPIOPin,
						GPIO_PIN_SET
				);

				HAL_GPIO_WritePin(
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN2 ].GPIOPort,
						_HalRes.ControlPins[ (MotorIdx<<1) + L298Pins::IN2 ].GPIOPin,
						GPIO_PIN_RESET
				);
			}

			SetPWM(
				_HalRes.PWMOuts[MotorIdx].PWMTimerHandle,
				_HalRes.PWMOuts[MotorIdx].Channel,
				fabs( MotorThrottles[MotorIdx]*MaxThrottlePulseWidth )
			);
		}
	}
}


void L298NMotorController::SetPWM(TIM_HandleTypeDef* timer, uint32_t channel, uint16_t pulse)
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

