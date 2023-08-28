#ifndef TELECOMMANDS_H
#define TELECOMMANDS_H

#include "main.h"
#include "cmsis_os.h"
#include <cstring>

#include "Endianness.h"

enum ApplicationTelecommandId {
	ControlLeds = 0,
	ControlMotorManual,
	ControlMotorAuto,
	SetMotorControlMode,
	NumberOfTelecommands
};

/* Command definitions */

#pragma pack(1)
struct ApplicationTelecommandHeader {
	uint8_t Opcode;

	void FromBytes(const uint8_t* payload)
	{
		Opcode = *payload;
	}
};
#pragma pack(0)



#pragma pack(1)
struct LedControlCommand
{
	uint32_t LedControlFlags;

	void FromBytes(const uint8_t* payload)
	{
		LedControlFlags = __builtin_bswap32(*((uint32_t*)&payload[0]));
	}
};
#pragma pack(0)




#pragma pack(1)
struct ControlMotorManualCommand
{
	uint32_t MotorControlFlags;
	float MotorAThrottle; // FIXME move to float
	float MotorBThrottle; // FIXME move to float

	void FromBytes(const uint8_t* payload)
	{
		MotorControlFlags = __builtin_bswap32(*(uint32_t*)&payload[0]);
		MotorAThrottle = SwapFloat(&payload[4]);
		MotorBThrottle = SwapFloat(&payload[8]);
	}
};
#pragma pack(0)



#pragma pack(1)
struct ControlMotorAutoCommand
{
	uint32_t MotorControlFlags;
	float MotorASpeed;
	float MotorBSpeed;

	void FromBytes(const uint8_t* payload)
	{
		MotorControlFlags = __builtin_bswap32(*((uint32_t*)&payload[0]));
		MotorASpeed = SwapFloat(&payload[4]);
		MotorBSpeed = SwapFloat(&payload[8]);
	}
};
#pragma pack(0)



#pragma pack(1)
struct SetMotorControlModeCommand
{
	uint32_t MotorControlModeFlags;

	void FromBytes(const uint8_t* payload)
	{
		MotorControlModeFlags = __builtin_bswap32(*((uint32_t*)&payload[0]));
	}
};
#pragma pack(0)

#endif
