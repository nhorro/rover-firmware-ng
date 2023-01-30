/**
 * @file config.h Configuración de la aplicación.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Puerto serie */
#define APP_SERIAL_IF_BAUDRATE	9600
#define APP_SERIAL_IF_BUFSIZE	32

/* Tamaño de mensajes y timeout del protocolo serie */
#define PACKET_TIMEOUT_IN_MS 	1000
#define HEARTBEAT_TIMEOUT_IN_MS 1000


/* Configuración de períodos de tareas
1. Leer entradas y sensores.
    1.1 read_telecommands
    1.2 read_imu
    1.3 read_tachometers
    1.4 read_gps
2. Aplicar filtros / estimar.
    2.1 N/A
3. Actualizar valores de actuadores.
    3.1 write_motors
4. Publicar telemetrías.
    4.1 send_general_tmy_report
    4.2 send_motion_control_report
    4.3 send_imu_report
    4.4 send_gps_report
*/

/* Frecuencia del ciclo de control (Hz) */
#define CONTROL_CYCLE_FREQ          100.0f

/* 1.1 Frecuencia para lectura de telecomandos (Hz) */
#define READ_TELECOMMANDS_FREQ	 	 100.0f

/* 1.2 Frecuencia para lectura de IMU (Hz) */
#define READ_IMU_FREQ	 	 10.0f

/* 1.3 Frecuencia para lectura de tacómetros (Hz) */
#define READ_TACHOMETERS_FREQ	 	 50.0f

/* 1.4 Frecuencia para lectura de GPS (Hz) */
#define READ_GPS_FREQ	 	 2.0f

/* 3.1 Frecuencia de actualización de motores (Hz) */
#define WRITE_MOTORS_FREQ	 	 50.0f

/* 4.1 Frecuencia de publicación de reportes de telemetría general (Hz) */
#define GENERAL_TMY_REPORT_FREQ	 	 3.0f

/* 4.2 Frecuencia de publicación de reportes de motores y tacómetros (Hz) */
#define MOTION_REPORT_FREQ 		 	 4.0f

/* 4.3 Frecuencia de publicación de reportes de IMU (Hz) */
#define IMU_REPORT_FREQ 		 	 1.0f

/* 4.4 Frecuencia de publicación de reportes de GPS (Hz) */
#define GPS_REPORT_FREQ 		 	 1.0f

/* Configuración de dispositivos */

#define WHEEL_ENCODER_N_TICKS 20


/* Definiciones de harness */

/* L298N */

/* IMU - MPU9250 */

/* GPS */

/* Tacómetros */
#define DEFAULT_TACHO1_PIN PE_7
#define DEFAULT_TACHO2_PIN PE_8
#define DEFAULT_TACHO3_PIN PG_9
#define DEFAULT_TACHO4_PIN PG_14

/* Valor máximo de RPM que se considera válido.
   Valores superiores o negativos se consideran errores de lectura o de cálculo de RPM. */
#define MAX_TACHO_VALID_RPM_READING 150.0f


/* Valor máximo de la variable de salida del controlador PID de motores */
#define DEFAULT_PID_MAX 1.0f
#define DEFAULT_PID_MIN -1.0f

#define DEFAULT_PID_KP 0.01f
#define DEFAULT_PID_KD 0.0001f

#define DEFAULT_PID_KI 0.0001f

#endif
