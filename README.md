Rover firmware for STM32 (Next Generation)
==========================================

This repository contains the firmware to control a 4WD rover. The software and pinout has been is customized to the [STM32 F767ZITX microcontroller](https://www.st.com/en/microcontrollers-microprocessors/stm32f767zi.html), but it can be easily ported to other STM32 targets that support CUBEMx HAL.

Code organization
-----------------

Finding a good workflow for using HAL and FreeRTOS in STM32CubeIde is still under a stage of learning, trial, and error. 

Trying to get the best of both worlds, the code organization attempts to:

1. Benefit from STM32CubeMx code generation for HAL and FreeRTOS, which involves regenerating periodically the `main.c` file and other files in `Core`. Note that at the time of this writing, STM32CubeMx does not support C++ so some workarounds are required to call C++ methods from C boilerplate. Also, FreeRTOS API is in C.
2. Separate application logic from HAL/FreeRTOS resource creation.

~~~
Core
├── Inc
│   ├── FreeRTOSConfig.h
│   ├── main.h
│   ├── stm32f7xx_hal_conf.h
│   └── stm32f7xx_it.h
├── Src
│   ├── freertos.c
│   ├── main.c
│   ├── stm32f7xx_hal_msp.c
│   ├── stm32f7xx_hal_timebase_tim.c
│   ├── stm32f7xx_it.c
│   ├── syscalls.c
│   ├── sysmem.c
│   └── system_stm32f7xx.c
└── Startup
    └── startup_stm32f767zitx.s
Application/
├── Inc
│   ├── ApplicationConfig.h
│   ├── Application.h
│   ├── Protocol
│   │   ├── decoder.hpp
│   │   ├── encoder.hpp
│   │   └── protocol.hpp
│   ├── Services
│   │   ├── CommandReceiver.h
│   │   └── TelemetrySender.h
│   ├── Telecommands.h
│   └── Telemetry.h
└── Src
    ├── Application.cpp
    ├── Protocol
    │   └── protocol.cpp
    └── Services
        ├── CommandReceiver
        │   └── CommandReceiver.cpp
        └── TelemetrySender
            └── TelemetrySender.cpp
~~~

Notebooks for development
-------------------------

[Python code and notebooks](./Python/) are provided to interact with the Rover.