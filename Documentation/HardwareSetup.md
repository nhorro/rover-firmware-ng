Hardware Setup
==============


Nucleo Pinout Reference (from OS Mbed site)
-------------------------------------------

### Arduino-compatible headers

#### CN8 and CN9

![image](https://os.mbed.com/media/uploads/jeromecoutant/nucleo_f767zi_zio_left_2020_3_30.png)

#### CN7 and CN10

![image](https://os.mbed.com/media/uploads/jeromecoutant/nucleo_f767zi_zio_right_2020_3_30.png)

### CN11 CN12 headers

![image](https://os.mbed.com/media/uploads/jeromecoutant/nucleo_f767zi_morpho_left_2020_3_30.png)

![image](https://os.mbed.com/media/uploads/jeromecoutant/nucleo_f767zi_morpho_right_2020_3_30.png)

Pin assignment
--------------

Note: with vehicle facing forward: LF=Left/Front, RB=Right/Back,etc.

### L298N - Nucleo

| L298N pin | Nucleo pin   | Wire color |
| --------- | ------------ | ---------- |
| ENA       | PB_11 (CN10) |            |
| IN1       | PE_10 (CN10) |            |
| IN2       | PE_12 (CN10) |            |
| IN3       | PE_14 (CN10) |            |
| IN4       | PE_15 (CN10) |            |
| ENB       | PB_10 (CN10) |            |

### L298N - Nucleo

| L298N pin | Motor      | Wire color     |
| --------- | ---------- | -------------- |
| OUT1      | LF, LB (-) | Black          |
| OUT2      | LF, LB (+) | Red            |
| OUT3      | RF,RB(+)   | Red            |
| OUT4      | RF,RB(-)   | Black          |

### Tachometers

| Nucleo pin    | LM393 pin            | Wire color |
| ------------- | -------------------- | ---------- |
| GND           | GND                  |            |
| 3.3V          | VCC                  |            |
| PF_10 (CN9)   | DO (Tacho 1, LF)     | Brown      |
| PC_2  (CN11)  | DO (Tacho 2, RF)     | White      |
| PG_9  (CN11)  | DO (Tacho  3, LB)    | Blue       |
| PG14  (CN10)  | DO (Tacho 4, LF)     | Green      |

### MPU9250

To be completed.

### GPS

To be completed.