#pragma once

#include "main.h"

#include <cstdio>

#include <Filters/MovingAverage.hpp>

class LM393Tachometer {
public:
	static constexpr const uint32_t TicksPerRevolution = 20;
	static constexpr const uint32_t TachometerDeltaAt1RPMInMs= 60*1000/TicksPerRevolution;
	static constexpr const uint32_t TachometerDeltaAtMinRPMInMs = TachometerDeltaAt1RPMInMs/30; // 30 RPM
	static constexpr const uint32_t TachometerDeltaAtMaxRPMInMs = TachometerDeltaAtMinRPMInMs / 400; // 400 RPM

	LM393Tachometer();
	~LM393Tachometer();

	inline bool UpdateISR()
	{
		uint32_t CurrentTimeInMs = HAL_GetTick();
		TimeSinceLastUpdateInMs = CurrentTimeInMs - LastUpdateTimeStampInMs;
		TickCount++;
		HasValidMeasure = true;
		LastUpdateTimeStampInMs = CurrentTimeInMs;
		if (TimeSinceLastUpdateInMs > TachometerDeltaAtMaxRPMInMs )
		{
			AverageDeltaTimeBetweenTicks = Filter.process(TimeSinceLastUpdateInMs);
		}
		else
		{
			// FIXME: too close IRQs might be an error that would be useful to detect.
			AverageDeltaTimeBetweenTicks = Filter.process(TachometerDeltaAtMaxRPMInMs);
		}

		return HasValidMeasure;
	}

	inline void DetectInactivity() {
		uint32_t CurrentTimeInMs = HAL_GetTick();
		if ((CurrentTimeInMs - LastUpdateTimeStampInMs) > TachometerDeltaAtMinRPMInMs)
		{
			HasValidMeasure = false;
		}
	}

	uint32_t LastUpdateTimeStampInMs;
	uint32_t AverageDeltaTimeBetweenTicks;
	uint32_t TimeSinceLastUpdateInMs;
	uint32_t TickCount;
	bool HasValidMeasure;

	MovingAverageFilter<8> Filter;
};
