#include "Services/CommandReceiver.h"
#include "Application.h"
//#include <cstring>

extern Application App;

CommandReceiverService CommandReceiver;

CommandReceiverService::CommandReceiverService()
	:
		CommandCallbacks{
			&CommandReceiverService::CmdControlLeds,
			&CommandReceiverService::CmdControlMotorManual,
			&CommandReceiverService::CmdControlMotorAuto,
			&CommandReceiverService::CmdControlMotorMode
		}
{

}

CommandReceiverService::~CommandReceiverService()
{

}


void CommandReceiverService::Main(void *argument)
{
	// FIXME: mecanismo provisorio para evitar que la tarea arranque sin haberse inicializado App.Config
	osDelay(1000);

	for(;;)
	{
		// Read Queue
		uint8_t ReceivedCommandMailboxIdx;
		osStatus result = osMessageQueueGet(
				App.Config->CommandReceiverQueueHandle,
				&ReceivedCommandMailboxIdx,
				0,
				-1);
		App.LasOsResult2 = result;

		if (result== osOK)
		{
			// 1. Parse header
			ApplicationTelecommandHeader Header;
			const uint8_t* TCData = &App.CommandMailbox[ReceivedCommandMailboxIdx][0];
			Header.FromBytes(TCData);

			App.LastCommandOpcode = Header.Opcode;

			// 2. Dispatch command

			if (Header.Opcode < ApplicationTelecommandId::NumberOfTelecommands)
			{
				bool CommandExecutionResult = (this->*CommandCallbacks[Header.Opcode])(
						TCData+sizeof(ApplicationTelecommandHeader)
				);
				App.LastCommandResult = static_cast<uint32_t>(CommandExecutionResult);
			}
			else
			{
				App.LastPacketStatus = LastPacketStatusCode::InvalidOpcode;
			}
		}
		else if (result == osErrorTimeout)
		{
			// TimeOut is not a problem
		}
		else
		{
			// Other code is a problem!
		}

		//osDelay(1000);
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	}
}


void CommandReceiverTaskMain(void *argument)
{
	CommandReceiver.Main(argument);
}


// --------------------------------------------------------------------------------------------------------
// Commands
// --------------------------------------------------------------------------------------------------------

bool CommandReceiverService::CmdControlLeds(const uint8_t* payload)
{
	LedControlCommand cmd;
	cmd.FromBytes(payload);
	App.LedControlState = cmd.LedControlFlags;
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, (cmd.LedControlFlags & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET );
	return true;
}

bool CommandReceiverService::CmdControlMotorManual(const uint8_t* payload)
{
	ControlMotorManualCommand cmd;
	cmd.FromBytes(payload);

	if ( cmd.MotorControlFlags & 1 )
	{
		App.MotorThrottles[0] = cmd.MotorAThrottle;
		// TODO: Update motors
	}

	if ( cmd.MotorControlFlags & 2 )
	{
		App.MotorThrottles[1] = cmd.MotorBThrottle;
		// TODO: Update motors
	}

	return true;
}

bool CommandReceiverService::CmdControlMotorAuto(const uint8_t* payload)
{
	ControlMotorAutoCommand cmd;
	cmd.FromBytes(payload);

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

bool CommandReceiverService::CmdControlMotorMode(const uint8_t* payload)
{
	SetMotorControlModeCommand cmd;
	cmd.FromBytes(payload);

	App.MotorControlModeFlags = cmd.MotorControlModeFlags;

	return true;
}


