#include "Services/TelemetrySender.h"
#include <cstring>

#include "Application.h"
extern Application App;


TelemetrySenderService::TelemetrySenderService()
{
	_RtosRes.ThreadAttributes.name = "TelemetrySenderTask";
	_RtosRes.ThreadAttributes.stack_size = 128 * 4,
	_RtosRes.ThreadAttributes.priority = (osPriority_t) osPriorityNormal;

	TelemetryCycle = 0;
}

TelemetrySenderService::~TelemetrySenderService()
{

}


void TelemetrySenderService::Init(UART_HandleTypeDef* UartTmHandle)
{
	_RtosRes.ThreadId = osThreadNew(TelemetrySenderService::TelemetrySenderServiceTaskMain, reinterpret_cast<void*>(this), &_RtosRes.ThreadAttributes);

	_HalRes.UartTmHandle = UartTmHandle;
}

void TelemetrySenderService::Main()
{
	/*
	for(size_t i=0;i<10;i++)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET );
		osDelay(1000);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET );
		osDelay(1000);
	}
	*/

	for(;;)
	{
		OnBoardTime = HAL_GetTick();

		SendGeneralTelemetryReport();
		osDelay(200);
		SendMotorReport();
		osDelay(200);
		SendIMUReport();
		osDelay(200);

		TelemetryCycle+=1;
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

void TelemetrySenderService::SendGeneralTelemetryReport()
{
	// Make report
	_GeneralTelemetry.TelemetryCycle = __builtin_bswap32(TelemetryCycle);
	_GeneralTelemetry.OnBoardTime = __builtin_bswap32(OnBoardTime);
	_GeneralTelemetry.ReceivedPackets = __builtin_bswap32(App.CommandReceiver.ReceivedPackets);


	uint32_t tmp = (App.MotorControlModeFlags << 0) |
			       (App.LedControlState << 2) |
				   (App.IMUStateOk << 3) |
				   (App.LastCommandOpcode << 16)  |
				   (App.LastCommandResult << 24);

	_GeneralTelemetry.GeneralStatus = __builtin_bswap32(tmp);

	EncodeAndSend(reinterpret_cast<const void*>(&_GeneralTelemetry), sizeof(ApplicationGeneralTelemetry));
}



void TelemetrySenderService::SendMotorReport()
{
	_MotorControlTelemetry.TelemetryCycle = __builtin_bswap32(TelemetryCycle);
	_MotorControlTelemetry.OnBoardTime = __builtin_bswap32(OnBoardTime);

	_MotorControlTelemetry.Throttle1 = __builtin_bswap32(reinterpret_cast<const uint32_t&>(App.MotorControl.MotorThrottles[0]));;
	_MotorControlTelemetry.Throttle2 = __builtin_bswap32(reinterpret_cast<const uint32_t&>(App.MotorControl.MotorThrottles[1]));;

	_MotorControlTelemetry.Tachometer1 = __builtin_bswap32(App.Tachometers[0].TickCount);
	_MotorControlTelemetry.Tachometer2 = __builtin_bswap32(App.Tachometers[1].TickCount);
	_MotorControlTelemetry.Tachometer3 = __builtin_bswap32(App.Tachometers[2].TickCount);
	_MotorControlTelemetry.Tachometer4 = __builtin_bswap32(App.Tachometers[3].TickCount);

	_MotorControlTelemetry.MeasuredSpeed1 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MeasuredSpeed[0]));
	_MotorControlTelemetry.MeasuredSpeed2 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MeasuredSpeed[1]));
	_MotorControlTelemetry.MeasuredSpeed3 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MeasuredSpeed[2]));
	_MotorControlTelemetry.MeasuredSpeed4 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MeasuredSpeed[3]));


	_MotorControlTelemetry.SetpointSpeed1 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorSetpointSpeeds[0]));
	_MotorControlTelemetry.SetpointSpeed2 = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.MotorSetpointSpeeds[1]));

	_MotorControlTelemetry.StatusFlags = __builtin_bswap32(App.MotorControlModeFlags);

	EncodeAndSend(reinterpret_cast<const void*>(&_MotorControlTelemetry), sizeof(ApplicationMotorControlTelemetry));
}



void TelemetrySenderService::SendIMUReport()
{
	/*
	App.IMUTelemetry.TelemetryCycle = __builtin_bswap32(App.TelemetryCycle);
	App.IMUTelemetry.OnBoardTime = __builtin_bswap32(App.OnBoardTime);

	App.IMUTelemetry.Pitch = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.IMUAttitude.p));;
	App.IMUTelemetry.Roll = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.IMUAttitude.r));;
	App.IMUTelemetry.Yaw = __builtin_bswap32(reinterpret_cast<uint32_t&>(App.IMUAttitude.y));;
	*/

	EncodeAndSend(reinterpret_cast<const void*>(&_IMUTelemetry), sizeof(ApplicationIMUTelemetry));
}



void TelemetrySenderService::EncodeAndSend(const void* Data, size_t SizeInBytes)
{
	memcpy( _TelemetryPacketEncoder.get_payload_buffer(), Data, SizeInBytes );

	_TelemetryPacketEncoder.calc_crc_and_close_packet( SizeInBytes );

	// FIXME: Check if UART is available to transmit.
	if ( true )
	{
		HAL_UART_Transmit_IT( _HalRes.UartTmHandle,
			_TelemetryPacketEncoder.get_packet_bytes(),
			_TelemetryPacketEncoder.get_total_length()
		);
	}
	else
	{
		// FIXME: Indicate error somewhere
	}


}


void TelemetrySenderService::TelemetrySenderServiceTaskMain(void *argument)
{
	TelemetrySenderService* Me = reinterpret_cast<TelemetrySenderService*>(argument);
    Me->Main();
}
