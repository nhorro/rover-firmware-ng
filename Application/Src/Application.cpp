#include "Application.h"

Application App;



static uint32_t TachoLastTs[4] = { 0, 0, 0, 0 };	// Last timestamp in ms
static uint32_t TachoDT[4] = { 0, 0, 0, 0 }; 		// Deltatime with respect to last timestamp
static float AppliedThrottle[2] = { 0, 0 };			// Applied throttle

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin)
{
	uint32_t CurrTS = HAL_GetTick();
	switch(GPIO_Pin)
	{
		case TACHO1_Pin: { 	TachoDT[0] =  CurrTS - TachoLastTs[0]; TachoLastTs[0] = CurrTS; } break;
		case TACHO2_Pin: {  TachoDT[1] =  CurrTS - TachoLastTs[1]; TachoLastTs[1] = CurrTS; } break;
		case TACHO3_Pin: {  TachoDT[2] =  CurrTS - TachoLastTs[2]; TachoLastTs[2] = CurrTS; } break;
		case TACHO4_Pin: {  TachoDT[3] =  CurrTS - TachoLastTs[3]; TachoLastTs[3] = CurrTS; } break;
		default: { __NOP(); }
	};
	//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle)
{
	App.CommandReceiver.UpdateISR(UartHandle);
}

void ApplicationSetup(const ApplicationConfig* Config)
{
	App.Setup(Config);
}



Application::Application() :
		_CommandCallbacks{
			&Application::CmdControlLeds,
			&Application::CmdControlMotorManual,
			&Application::CmdControlMotorAuto,
			&Application::CmdControlMotorMode,
			&Application::CmdSetPIDParameters,
			// More commands here
		}
{

}

Application::~Application() { }

void Application::Setup(const ApplicationConfig* Config)
{
	L298NMotorController::STM32HALResources& MotorConfig = MotorControl.GetHALResourceConfig();

	// This should come from Config but we are too lazy now.
	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN1].GPIOPort = L298N_IN1_GPIO_Port;
	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN1].GPIOPin = L298N_IN1_Pin;

	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN2].GPIOPort = L298N_IN2_GPIO_Port;
	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN2].GPIOPin = L298N_IN2_Pin;

	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN3].GPIOPort = L298N_IN3_GPIO_Port;
	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN3].GPIOPin = L298N_IN3_Pin;

	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN4].GPIOPort = L298N_IN4_GPIO_Port;
	MotorConfig.ControlPins[L298NMotorController::L298Pins::IN4].GPIOPin = L298N_IN4_Pin;

	MotorConfig.PWMOuts[0].PWMTimerHandle = Config->PwmTimerHandle;
	MotorConfig.PWMOuts[0].Channel = TIM_CHANNEL_3;
	MotorConfig.PWMOuts[1].PWMTimerHandle = Config->PwmTimerHandle;
	MotorConfig.PWMOuts[1].Channel = TIM_CHANNEL_4;

	CommandReceiver.Init(Config->UartTcTmHandle, std::bind(&Application::HandleTelecommand, this, std::placeholders::_1) );

	MainControlLoop.Init();

	TelemetrySender.Init(Config->UartTcTmHandle);


	MotorControl.Start();
}



void Application::HandleTelecommand(const uint8_t* TCData)
{
	// 1. Parse header
	ApplicationTelecommandHeader Header;
	Header.FromBytes(TCData);

	LastCommandOpcode = Header.Opcode;

	// 2. Dispatch command
	if (Header.Opcode < ApplicationTelecommandId::NumberOfTelecommands)
	{
		bool CommandExecutionResult = (this->*_CommandCallbacks[Header.Opcode])(
				TCData+sizeof(ApplicationTelecommandHeader)
		);
		LastCommandResult = static_cast<uint32_t>(CommandExecutionResult);
	}
	else
	{
		// Indicate invalid opcode.
		//LastPacketStatus = LastPacketStatusCode::InvalidOpcode;
	}

}




// --------------------------------------------------------------------------------------------------------
// Commands
// --------------------------------------------------------------------------------------------------------

bool Application::CmdControlLeds(const uint8_t* payload)
{
	LedControlCommand cmd;
	cmd.FromBytes(payload);

	App.LedControlState = cmd.LedControlFlags;
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, (cmd.LedControlFlags & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET );

	return true;
}


bool Application::CmdControlMotorManual(const uint8_t* payload)
{
	ControlMotorManualCommand cmd;
	cmd.FromBytes(payload);

	// Debug
	AppliedThrottle[0] = cmd.MotorAThrottle;

	if (MotorControlModeFlags & Application::ControlModeFlags::ArmedManual)
	{
		if(cmd.MotorControlFlags&L298NMotorController::MotorControlFlags::A)
		{
			MotorControl.MotorThrottles[0] = cmd.MotorAThrottle;
		}

		if(cmd.MotorControlFlags&L298NMotorController::MotorControlFlags::B)
		{
			MotorControl.MotorThrottles[1] = cmd.MotorBThrottle;
		}

		MotorControl.UpdateMotorThrottles(static_cast<L298NMotorController::MotorControlFlags>(cmd.MotorControlFlags));
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool Application::CmdControlMotorAuto(const uint8_t* payload)
{
	ControlMotorAutoCommand cmd;
	cmd.FromBytes(payload);

	if (App.MotorControlModeFlags & Application::ControlModeFlags::ArmedPID)
	{
		if (cmd.MotorControlFlags & 1)
		{
			App.MotorSetpointSpeeds[0] = cmd.MotorASpeed;
		}

		if (cmd.MotorControlFlags & 2)
		{
			App.MotorSetpointSpeeds[1] = cmd.MotorBSpeed;
		}

		return true;
	}
	else
	{
		return false;
	}

	return true;
}

bool Application::CmdControlMotorMode(const uint8_t* payload)
{
	SetMotorControlModeCommand cmd;
	cmd.FromBytes(payload);

	switch(cmd.MotorControlModeFlags)
	{
		case Application::ControlModeFlags::ArmedManual: {

		} break;

		case Application::ControlModeFlags::ArmedPID: {

		} break;

		case Application::ControlModeFlags::Disarmed: {

		} break;
	}

	// In either case, shutdown motors when changing mode.
	MotorControlModeFlags = cmd.MotorControlModeFlags;
	MotorControl.Shutdown();

	return true;
}

bool Application::CmdSetPIDParameters(const uint8_t* payload)
{
	SetPIDParametersCommand cmd;
	cmd.FromBytes(payload);

	App.PID[0].reset();
	App.PID[0].Kp = cmd.Kp;
	App.PID[0].Ki = cmd.Ki;
	App.PID[0].Kd = cmd.Kd;
	App.PID[1].reset();
	App.PID[1].Kp = cmd.Kp;
	App.PID[1].Ki = cmd.Ki;
	App.PID[1].Kd = cmd.Kd;


	return true;
}


