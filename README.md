Rover firmware for STM32 (Next Generation)
==========================================

This repository contains the STM32CubeIde proect with the firmware to control a 4WD rover. The software and pinout has been customized to the [STM32 F767ZITX microcontroller](https://www.st.com/en/microcontrollers-microprocessors/stm32f767zi.html), but it can be easily ported to other STM32 targets that support CUBEMx HAL.

**History - Why 'Next Generation'?**

This version is an improvement over previous implementations:

- v1. An initial implementation with [Arduino Due / Raspberry Pi](https://github.com/nhorro/rover-firmware). This included the ROS software running in a Raspberry PI and a x86 for GroundControl.
- v2. A migration to [Nucleo-f767zi with MBedOs](https://github.com/nhorro/ceai2020/tree/master/intro_se/workspace/rover_firmware) with a lightweight [GroundControl](https://github.com/nhorro/ceai2020/tree/master/intro_se/workspace/rover_groundcontrol).
- v3. This version:
   -  Better task/code modularization with FreeRTOS.
   -  Benefit from STM32CubeHal to choose the best peripheral configuration (hardware PWM and tick count, etc.).

Instructions
------------

**Requirements**

- Hardware:
    - [Nucleo-f767zi](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html).
    - Sensors:
      - [4x tachmoeters that use optical encoders LM393](https://candy-ho.com/producto/sensor-optico-horquilla-velocidad-tacometro-lm393-arduino/).
      - [1x MPU9250](https://articulo.mercadolibre.com.ar/MLA-618704275-mpu-9250-mpu9250-acelerometro-magnetometro-giroscopo-arduino-_JM#position=2&search_layout=grid&type=item&tracking_id=5be31c5c-a195-44a5-9254-faf2f07b738c).
      - [1x GPS Neo6M](https://articulo.mercadolibre.com.ar/MLA-684956175-neo6m-gy-gps6mv2-gps-apm25-neo-6m-modulo-antena-a0129-_JM#position=2&search_layout=grid&type=item&tracking_id=110a4426-ee0b-4a96-8a97-0db8e81b4a8f).
    - Actuators:
      - [L298N motor controller module](https://candy-ho.com/producto/doble-puente-h-driver-l298n-motor-dc-arduino-arm-avr-l298/).    
    - 4WD Chassis kit:
        - 4x DC Motors wtih gear reduction + wheels.
        - 1x Battery holder x2 18650 (8.4v).
        - 2x 18650 batteries.
- Software:
    - [STM32CubeIde](https://www.st.com/en/development-tools/stm32cubeide.html)
    - Python 3+ (tested in Conda base enviroment).

**Procedure**

1. Import project in [STM32CubeIde](https://www.st.com/en/development-tools/stm32cubeide.html) and download to Nucleo.
2. Communicate with the firmware with the [Python notebooks](./Python/).

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
