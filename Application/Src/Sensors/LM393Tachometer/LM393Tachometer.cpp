#include "Sensors/LM393Tachometer.h"

LM393Tachometer::LM393Tachometer()
	:
		LastUpdateTimeStampInMs(0),
		AverageDeltaTimeBetweenTicks(0),
		TimeSinceLastUpdateInMs(0),
		TickCount(0),
		HasValidMeasure(false)
{

}

LM393Tachometer::~LM393Tachometer()
{

}
