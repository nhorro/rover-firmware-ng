/**
 *
 */
#pragma once

#include "main.h"
#include "cmsis_os.h"


class MainControlLoopService {
public:
	MainControlLoopService();
	~MainControlLoopService();

	void Init();
	void Main();

	// Hardware resources provided by STM32CubeMx HAL.
	struct STM32HALResources {
		STM32HALResources(){};
	};

	// FreeRTOS resources Provided by
	struct FreeRTOSResources {
		FreeRTOSResources(){};
		osThreadId_t ThreadId;
		osThreadAttr_t ThreadAttributes;
	};

private:
	FreeRTOSResources _RtosRes;
	STM32HALResources _HalRes;

	static void MainControlLoopTaskMain(void *argument);
};
