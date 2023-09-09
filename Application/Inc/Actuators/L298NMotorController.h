#pragma once

#include "gpio.h"

class L298NMotorController {
public:
	// Hardware resources provided by STM32CubeMx HAL.
	struct STM32HALResources {
		struct {
			GPIO_TypeDef* GPIOPort;
			uint16_t GPIOPin;
		} ControlPins[4];

		struct {
			TIM_HandleTypeDef* PWMTimerHandle;
			uint32_t Channel;
		} PWMOuts[2];
	};

	L298NMotorController();
	~L298NMotorController();

	enum MotorControlFlags {
		A=1,
		B=2,
		Both = 3
	};

	enum L298Pins {
		IN1 = 0,
		IN2 = 1,
		IN3 = 2,
		IN4 = 3
	};

	void UpdateMotorThrottles(float MotorThrottles[2], MotorControlFlags Flags);

	inline const float* GetMotorThrottles() const { return _MotorThrottles; }

	STM32HALResources& GetHALResourceConfig() { return _HalRes; }

	void Start();
	void Shutdown();
private:
	STM32HALResources _HalRes;
	float _MotorThrottles[2];

	void SetPWM(TIM_HandleTypeDef* timer, uint32_t channel, uint16_t pulse);
};
