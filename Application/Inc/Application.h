#pragma once

#include "main.h"

#include "ApplicationConfig.h"

#include "Telecommands.h"
#include "Telemetry.h"

#include "Actuators/L298NMotorController.h"
#include "Sensors/LM393Tachometer.h"

#include "Services/CommandReceiver.h"
#include "Services/MainControlLoop.h"
#include "Services/TelemetrySender.h"

extern "C" {
	void ApplicationSetup(const ApplicationConfig* Config);
	void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin);
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle);
}


class Application {
public:
	Application();
	~Application();
	void Setup(const ApplicationConfig* Config);

	// Telemetry
	uint32_t LastCommandOpcode = 0;
	uint32_t LastCommandResult = 0;
    uint32_t LedControlState = 0;
	uint32_t IMUStateOk = 0;
	uint32_t MotorControlModeFlags = ControlModeFlags::Disarmed;

	// Actuators
	enum ControlModeFlags {
			Disarmed=0,
			ArmedManual=1,
			ArmedPID=2
	};
	L298NMotorController MotorControl;

	// Sensors
	LM393Tachometer Tachometers[4];

	// Services
	CommandReceiverService CommandReceiver;
	MainControlLoopService MainControlLoop;
	TelemetrySenderService TelemetrySender;


	float MotorSetpointSpeeds[2] = {0,0};

private:


	void HandleTelecommand(const uint8_t* TCData);


	// Friends
	friend void ApplicationSetup(const ApplicationConfig* Config);
	friend void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin);
	friend void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle);

	// Commands
	using CommandCallback = bool (Application::*)(const uint8_t*);
	CommandCallback _CommandCallbacks[ApplicationTelecommandId::NumberOfTelecommands];

	// Commands
	bool CmdControlLeds(const uint8_t*);
	bool CmdControlMotorManual(const uint8_t* payload);
	bool CmdControlMotorAuto(const uint8_t* payload);
	bool CmdControlMotorMode(const uint8_t* payload);
	bool CmdSetPIDParameters(const uint8_t* payload);
    // More commands here
    //bool CmdMyNewCommand(const uint8_t* payload);
};




