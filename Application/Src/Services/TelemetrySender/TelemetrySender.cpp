#include "Services/TelemetrySender.h"
#include "Application.h"
#include <cstring>


extern Application App;

TelemetrySenderService TelemetrySender;


TelemetrySenderService::TelemetrySenderService()
{

}

TelemetrySenderService::~TelemetrySenderService()
{

}

void TelemetrySenderService::Main(void *argument)
{
	// FIXME: mecanismo provisorio para evitar que la tarea arranque sin haberse inicializado App.Config
	osDelay(1000);

	for(;;)
	{
		SendGeneralTelemetryReport();
		osDelay(200);
		SendMotorReport();
		osDelay(200);
		SendMotorReport();
		osDelay(200);
		//HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
	}
}

void TelemetrySenderService::SendGeneralTelemetryReport()
{
	App.TelemetryCycle+=1;

	// Make report
	App.GeneralTelemetry.TelemetryCycle = __builtin_bswap32(App.TelemetryCycle);
	App.GeneralTelemetry.OnBoardTime = __builtin_bswap32(App.OnBoardTime);
	App.GeneralTelemetry.ReceivedPackets = __builtin_bswap32(App.ReceivedPackets);

	uint32_t tmp = (App.MotorControlModeFlags << 0) |
			       (App.LedControlState << 2) |
				   (App.LastCommandOpcode << 16)  |
				   (App.LastCommandResult << 24);

	App.GeneralTelemetry.GeneralStatus = __builtin_bswap32(tmp);

	App.GeneralTelemetry.Debug1 = __builtin_bswap32(App.LasOsResult1);
	App.GeneralTelemetry.Debug2 = __builtin_bswap32(App.LasOsResult2);

	EncodeAndSend(reinterpret_cast<const void*>(&App.GeneralTelemetry), sizeof(ApplicationGeneralTelemetry));
}



void TelemetrySenderService::SendMotorReport()
{
	App.MotorControlTelemetry.TelemetryCycle = __builtin_bswap32(App.TelemetryCycle);
	App.MotorControlTelemetry.OnBoardTime = __builtin_bswap32(App.OnBoardTime);

	App.MotorControlTelemetry.Throttle1 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorThrottles[0]));;
	App.MotorControlTelemetry.Throttle2 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorThrottles[1]));;

	App.MotorControlTelemetry.Tachometer1 = __builtin_bswap32(App.TachometerTicks[0]);
	App.MotorControlTelemetry.Tachometer2 = __builtin_bswap32(App.TachometerTicks[1]);
	App.MotorControlTelemetry.Tachometer3 = __builtin_bswap32(App.TachometerTicks[2]);
	App.MotorControlTelemetry.Tachometer4 = __builtin_bswap32(App.TachometerTicks[3]);

	App.MotorControlTelemetry.MeasuredSpeed1 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.TachometerMeasuredSpeed[0]));
	App.MotorControlTelemetry.MeasuredSpeed2 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.TachometerMeasuredSpeed[1]));
	App.MotorControlTelemetry.MeasuredSpeed3 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.TachometerMeasuredSpeed[2]));
	App.MotorControlTelemetry.MeasuredSpeed4 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.TachometerMeasuredSpeed[3]));

	App.MotorControlTelemetry.SetpointSpeed1 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorSetpointSpeeds[0]));
	App.MotorControlTelemetry.SetpointSpeed2 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorSetpointSpeeds[1]));

	App.MotorControlTelemetry.StatusFlags = __builtin_bswap32(App.MotorControlModeFlags);

	EncodeAndSend(reinterpret_cast<const void*>(&App.MotorControlTelemetry), sizeof(ApplicationMotorControlTelemetry));
}


void TelemetrySenderService::EncodeAndSend(const void* Data, size_t SizeInBytes)
{
	memcpy( TelemetryPacketEncoder.get_payload_buffer(), Data, SizeInBytes );

	TelemetryPacketEncoder.calc_crc_and_close_packet( SizeInBytes );

	// FIXME: Check if UART is available to transmit.
	if ( true )
	{
		HAL_UART_Transmit_IT( App.Config->UartTcTmHandle,
			TelemetryPacketEncoder.get_packet_bytes(),
			TelemetryPacketEncoder.get_total_length()
		);
	}
	else
	{
		// FIXME: Indicate error somewhere
	}


}

void TelemetrySenderTaskMain(void *argument)
{
	TelemetrySender.Main(argument);
}
