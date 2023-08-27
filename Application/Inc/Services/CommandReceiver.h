#pragma once

#include "main.h"
#include "cmsis_os.h"

#include "Telecommands.h"


class CommandReceiverService {
public:
	CommandReceiverService();
	~CommandReceiverService();

	void Main(void *argument);
private:
	using CommandCallback = bool (CommandReceiverService::*)(const uint8_t*);

	CommandCallback CommandCallbacks[ApplicationTelecommandId::NumberOfTelecommands];

	// Commands
	bool CmdControlLeds(const uint8_t*);
	bool CmdControlMotorManual(const uint8_t* payload);
	bool CmdControlMotorAuto(const uint8_t* payload);
	bool CmdControlMotorMode(const uint8_t* payload);
};


extern "C"
{
	void CommandReceiverTaskMain(void *argument);
}
