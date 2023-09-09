Rover Firmware Software Design 
==============================

This section describes the firmware software architecture.

Static architecture
-------------------

### STM32 HAL and FreeRTOS integration

STM32CubeMx automatically regenerates boilerplate code for HAL and FreeRTOS/CMSISv2 each time the `.ioc` configuration file is updated. The main point of superposition between user and boilerplate code are `main.c` and `freertos.c` files. To keep this files as less modified ass possible:

1. User application code is maintained in `Application.cpp/h` with external sources grouped by category (this is described in the next section).
2. FreeRTOS objects are created programatically, withouth the IDE. DefaultTask is automatically created by STM32CubeMx and apparently there is no avoid it being regenerated each time that the `.ioc` configuration file is updated.


Application receives the pointers to HAL initialized descriptors in the structure declared in `ApplicationConfig.h`, which is passed to the C++ `Setup()` member function using a free function with C linkage: `ApplicationSetup()`.

**File: main.c**

~~~c
/* USER CODE BEGIN Includes */
#include "ApplicationConfig.h"
extern void ApplicationSetup(const ApplicationConfig* Config);
/* USER CODE END Includes */

/* ... */

  /* USER CODE BEGIN 2 */
  ApplicationConfig Config;
  Config.UartTcTmHandle = &huart3; // Development mode (USB)
  //Config.UartTcTmHandle = &huart1; // Production mode (Rx/Tx pins)
  Config.PwmTimerHandle = &htim2;
  ApplicationSetup(&Config);
  /* USER CODE END 2 */
~~~

**File: ApplicationConfig.h**

~~~c
#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H

#include "main.h"       // Provides STM32CubeMx API and defines.
#include "cmsis_os.h"   // Provides FreeRTOS API

typedef struct _ApplicationConfig{
	UART_HandleTypeDef* UartTcTmHandle;
	TIM_HandleTypeDef*  PwmTimerHandle;

	// MPU9250
	I2C_HandleTypeDef *MPU9250I2CHandle;
} ApplicationConfig;

#endif // APPLICATION_CONFIG_H
~~~

### Component organization

FIXME.

### Services

FIXME.

### Sensors

FIXME.

### Actuators

FIXME.

### Controllers

FIXME.

### Filters

FIXME.

#### Madgwick filter

FIXME.

#### Lowpass filters

- Butterworth
- Chebyshev
- Elliptical


#### Interrupts

~~~c++
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin); // Handle tachometers
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle); // Handle telecommands from UART.
~~~


Dynamic architecture
--------------------


### Startup sequence

FIXME.

### Services

FIXME.

### Command reception

FIXME.

### Telemetry publication

FIXME.

### Control cycle

FIXME.


Command and telemetry reference
-------------------------------


### Commands

FIXME.


### Telemetry

FIXME.


Customization and extension guide
---------------------------------

### Adding new features

- Implementing a new telecommand.
- Adding telemetry.
- Implementing a new service.

### Adding a new telecommand

1. Add the new command Id to the `ApplicationTelecommandId` enum in `Telecommands.h`:

~~~c++
enum ApplicationTelecommandId {
	ControlLeds = 0,
	ControlMotorManual,
	ControlMotorAuto,
	SetMotorControlMode,
	CmdSetPIDParameters,
	// New commands here
    ExampleCommand,
	NumberOfTelecommands
};
~~~

2. Define the command structure and implement `FromBytes()` deserializer :

~~~c++
#pragma pack(1)
struct ExampleCommand
{
	uint32_t AnIntegerParameter;
	float AFloatParameter;

	void FromBytes(const uint8_t* payload)
	{
		AnIntegerParameter = __builtin_bswap32(*(uint32_t*)&payload[0]);
		AFloatParameter = SwapFloat(&payload[4]);
	}
};
#pragma pack(0)
~~~


3. Add the command handler. 


Edit `Application.h` and add `bool CmdExample(const uint8_t* payload);` in the "Commmands" section:

~~~c++
    // Commands
	bool CmdControlLeds(const uint8_t*);
	bool CmdControlMotorManual(const uint8_t* payload);
	bool CmdControlMotorAuto(const uint8_t* payload);
	bool CmdControlMotorMode(const uint8_t* payload);
	bool CmdExample(const uint8_t* payload);	
    // More commands here    
~~~

And define the code for the telecommand in `Application.cpp`:

~~~c++
bool Application::CmdExample(const uint8_t* payload)
{
	ExampleCommand cmd;
	cmd.FromBytes(payload);

    // Do something with cmd.AFloatParameter, AnIntegerParameter.
    return true; // True if command was succesful Ok.
}
~~~

4. **Optional**: add command method in Python client.

### Adding telemetry

FIXME.

### Adding a new service

FIXME.