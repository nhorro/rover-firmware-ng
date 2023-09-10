
#pragma once

class PIDController {
public:
	PIDController();
	~PIDController();

	// Variables del PID
	float previousError = 0.0f;
	float integral = 0.0f;

	void reset() {
		previousError = 0.0f;
		integral = 0.0f;
	}

	// Constantes del PID
	float Kp = 0.7f;
	float Ki = 0.3f;
	float Kd = 0.5f;

	float Process(float SetpointSpeed, float measuredSpeed);
private:

};
