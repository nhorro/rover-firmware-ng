
#pragma once

class PIDController {
public:
	PIDController();
	~PIDController();

	// Variables del PID
	float previousError = 0.0f;
	float integral = 0.0f;

	// Constantes del PID
	const float Kp = 0.3f;
	const float Ki = 0.1f;
	const float Kd = 0.125f;

	float Process(float SetpointSpeed, float measuredSpeed);
private:

};
