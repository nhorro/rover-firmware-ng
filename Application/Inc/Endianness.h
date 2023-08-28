#pragma once

static inline float SwapFloat(const uint8_t* data)
{
	union {
		uint8_t b[4];
		float f;
	} converter;

	converter.b[0] = data[3];
	converter.b[1] = data[2];
	converter.b[2] = data[1];
	converter.b[3] = data[0];

	return converter.f;
}
