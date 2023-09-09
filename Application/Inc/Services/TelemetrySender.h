/**
 *
 */
#pragma once

#include "main.h"
#include "cmsis_os.h"
#include "Protocol/encoder.hpp"

#include "Telemetry.h"

class TelemetrySenderService {
public:
	TelemetrySenderService();
	~TelemetrySenderService();

	void Init(UART_HandleTypeDef* UartTcTmHandle);
	void Main();

	// Hardware resources provided by STM32CubeMx HAL.
	struct STM32HALResources {
		STM32HALResources(){};
		UART_HandleTypeDef* UartTmHandle;
	};

	// FreeRTOS resources Provided by
	struct FreeRTOSResources {
		FreeRTOSResources(){};
		osThreadId_t ThreadId;
		osThreadAttr_t ThreadAttributes;
	};

	// Telemetry
	uint32_t TelemetryCycle = 0;
	uint32_t OnBoardTime = 0; // Onboard time in usecs

private:
	FreeRTOSResources _RtosRes;
	STM32HALResources _HalRes;

	protocol::packet_encoder _TelemetryPacketEncoder;

	void SendGeneralTelemetryReport();
	void SendMotorReport();
	void SendIMUReport();

	void EncodeAndSend(const void* Data, size_t SizeInBytes);

	// Telemetry
	ApplicationGeneralTelemetry _GeneralTelemetry;
	ApplicationMotorControlTelemetry _MotorControlTelemetry;
	ApplicationIMUTelemetry _IMUTelemetry;

	static void TelemetrySenderServiceTaskMain(void *argument);
};
