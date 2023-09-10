#include "Services/MainControlLoop.h"
//#include "Application.h"
//#include <cstring>

#include "Sensors/LM393Tachometer.h"

#include "Application.h"
extern Application App;




MainControlLoopService::MainControlLoopService()
{
	_RtosRes.ThreadAttributes.name = "MainControlTask";
	_RtosRes.ThreadAttributes.stack_size = 128 * 4,
	_RtosRes.ThreadAttributes.priority = (osPriority_t) osPriorityHigh;
}



MainControlLoopService::~MainControlLoopService()
{

}


void MainControlLoopService::Init()
{
	//_RtosRes.SensorReadingsAvailableEventGroup = osEventFlagsNew(nullptr);
	_RtosRes.ThreadId = osThreadNew(MainControlLoopService::MainControlLoopTaskMain, reinterpret_cast<void*>(this), &_RtosRes.ThreadAttributes);
}



void MainControlLoopService::Main()
{
	static constexpr const uint32_t UpdateFreqInHz = 200;
	for(;;)
	{
		// Compute speeds
		for(size_t TachoIdx = 0; TachoIdx<4;TachoIdx++)
		{
			App.Tachometers[TachoIdx].DetectInactivity();

			if (App.Tachometers[TachoIdx].HasValidMeasure)
			{
				App.MeasuredSpeed[TachoIdx] = App.MedianFilters[TachoIdx].Process(
						LM393Tachometer::TachometerDeltaAt1RPMInMs / App.Tachometers[TachoIdx].AverageDeltaTimeBetweenTicks );
			}
			else
			{
				App.MeasuredSpeed[TachoIdx] = 0;
			}
		}

		// PID mode
		if (App.MotorControlModeFlags & Application::ControlModeFlags::ArmedPID)
		{
			App.MotorControl.MotorThrottles[0] = App.PID[0].Process(
					App.MotorSetpointSpeeds[0],
					(App.MeasuredSpeed[0]+App.MeasuredSpeed[1])/2
			);

			App.MotorControl.MotorThrottles[1] = App.PID[1].Process(
					App.MotorSetpointSpeeds[1],
					(App.MeasuredSpeed[2]+App.MeasuredSpeed[3])/2
			);

			App.MotorControl.UpdateMotorThrottles(L298NMotorController::MotorControlFlags::Both);

			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		}


		osDelay(1000/UpdateFreqInHz);

	}
}



void MainControlLoopService::MainControlLoopTaskMain(void *argument)
{
    MainControlLoopService* Me = reinterpret_cast<MainControlLoopService*>(argument);
    Me->Main();
}
