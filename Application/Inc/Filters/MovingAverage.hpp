#pragma once

template <size_t N>
class MovingAverageFilter {
public:
	MovingAverageFilter() {}
	~MovingAverageFilter() {}

	float process(float value)
	{
		// Desplaza las mediciones en el buffer
	    for (size_t i = N - 1; i > 0; i--)
	    {
	        buffer[i] = buffer[i - 1];
	    }

	    // Almacena la nueva medición en el buffer
	    buffer[0] = value;


	    // Calcula el promedio de las últimas N mediciones
	    float suma = 0;
	    for (size_t i = 0; i < N; i++) {
	        suma += buffer[i];
	    }

	    return suma / N;
	}
private:
	float buffer[N];
};
