#pragma once

#include "main.h"

class LM393Tachometer {
public:
	static constexpr const uint32_t TicksPerRevolution = 20;
	static constexpr const uint32_t TachometerDeltaAt1RPMInMs = 60*1000/TicksPerRevolution; // 1 RPM
	static constexpr const uint32_t TachometerDeltaAtMaxRPMInMs = TachometerDeltaAt1RPMInMs / 3000; // 3000 RPM
	LM393Tachometer();
	~LM393Tachometer();

	inline bool UpdateISR()
	{
		uint32_t CurrentTimeInMs = HAL_GetTick();
		TimeSinceLastUpdateInMs = CurrentTimeInMs - LastUpdateTimeStampInMs;
		if (TimeSinceLastUpdateInMs > TachometerDeltaAtMaxRPMInMs )
		{
			HasValidMeasure = true;
			LastUpdateTimeStampInMs = CurrentTimeInMs;
		}
		else
		{
			// FIXME: too close IRQs might be an error that would be useful to detect.
		}

		return HasValidMeasure;
	}

	inline void DetectInactivity() {
		if (LastUpdateTimeStampInMs > TachometerDeltaAt1RPMInMs)
		{
			HasValidMeasure = false;
		}
	}

	uint32_t LastUpdateTimeStampInMs;
	uint32_t TimeSinceLastUpdateInMs;
	uint32_t TickCount;
	bool HasValidMeasure;
};
