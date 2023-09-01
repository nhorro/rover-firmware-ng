#pragma once

#include "main.h"
#include "cmsis_os.h"

#define APPLICATION_FRAME_ID_GENERAL_TELEMETRY 0
#define APPLICATION_FRAME_ID_MOTOR_CONTROL_TELEMETRY 1


#pragma pack(1)
struct ApplicationGeneralTelemetry
{
	uint8_t FrameType = APPLICATION_FRAME_ID_GENERAL_TELEMETRY;
	uint32_t TelemetryCycle = 0;
	uint32_t OnBoardTime = 0;
	uint32_t ReceivedPackets = 0;
	uint32_t LedControlState = 0;
	uint32_t LastCommandOpcode = 0;
	uint32_t LastCommandResult = 0;
	uint32_t LasOsResult1 = 0;
	uint32_t LasOsResult2 = 0;

};
#pragma pack(0)



#pragma pack(1)
struct ApplicationMotorControlTelemetry{
	uint8_t FrameType = APPLICATION_FRAME_ID_MOTOR_CONTROL_TELEMETRY;
	uint32_t Throttle1;
	uint32_t Throttle2;

	uint32_t Tachometer1;
	uint32_t Tachometer2;
	uint32_t Tachometer3;
	uint32_t Tachometer4;

	uint32_t MeasuredSpeed1;
	uint32_t MeasuredSpeed2;
	uint32_t MeasuredSpeed3;
	uint32_t MeasuredSpeed4;

	uint32_t SetpointSpeed1;
	uint32_t SetpointSpeed2;

	uint32_t StatusFlags;
} ;
#pragma pack(0)
