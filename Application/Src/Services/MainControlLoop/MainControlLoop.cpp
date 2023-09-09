#include "Services/MainControlLoop.h"
//#include "Application.h"
//#include <cstring>

//extern Application App;



MainControlLoopService::MainControlLoopService()
{
	_RtosRes.ThreadAttributes.name = "MainControlTask";
	_RtosRes.ThreadAttributes.stack_size = 128 * 4,
	_RtosRes.ThreadAttributes.priority = (osPriority_t) osPriorityNormal;
}



MainControlLoopService::~MainControlLoopService()
{

}


void MainControlLoopService::Init()
{
	_RtosRes.ThreadId = osThreadNew(MainControlLoopService::MainControlLoopTaskMain, reinterpret_cast<void*>(this), &_RtosRes.ThreadAttributes);
}



void MainControlLoopService::Main()
{
	for(;;)
	{

	}
}



void MainControlLoopService::MainControlLoopTaskMain(void *argument)
{
    MainControlLoopService* Me = reinterpret_cast<MainControlLoopService*>(argument);
    Me->Main();
}
