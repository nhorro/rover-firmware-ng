
#include "Controllers/PID.h"

PIDController::PIDController()
{
}

PIDController::~PIDController()
{

}

float PIDController::Process(float SetpointSpeed, float measuredSpeed)
{
	// Calcula el error
	float error = SetpointSpeed - measuredSpeed;

	// Calcula la parte proporcional
	float P = Kp * error;

	// Calcula la parte integral
	integral += error;
	float I = Ki * integral;

	// Calcula la parte derivativa
	float D = Kd * (error - previousError);
	previousError = error;

	// Calcula la señal de control resultante
	float controlSignal = P + I + D;

	// Limita la señal de control entre 0.0 y 1.0
	if (controlSignal < 0.0f) {
		controlSignal = 0.0f;
	} else if (controlSignal > 1.0f) {
		controlSignal = 1.0f;
	}

	// According to the IEEE standard, NaN values have the odd property that
	// comparisons involving them are always false.
	// That is, for a float f, f != f will be true only if f is NaN.
	if ( controlSignal != controlSignal)
	{
		controlSignal = 0.0f;
	}


	return controlSignal;

}

