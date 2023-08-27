#pragma once

#include "main.h"
#include "cmsis_os.h"
#include "Protocol/encoder.hpp"

extern "C"
{
	void TelemetrySenderTaskMain(void *argument);
}

class TelemetrySenderService {
public:
	TelemetrySenderService();
	~TelemetrySenderService();

	void Main(void *argument);
private:
	protocol::packet_encoder TelemetryPacketEncoder;

	void SendGeneralTelemetryReport();
	void SendMotorReport();

	void EncodeAndSend(const void* Data, size_t SizeInBytes);
};

extern TelemetrySenderService TelemetrySender;
