/**
 * @file cmd_def.h Definición de códigos de telecomandos (OPCODES).
 */

#ifndef CMD_DEF_H
#define CMD_DEF_H

/** Definición de OPCODES (códigos de operación de los telecomandos). */
enum opcode_index_e {
    /** Pedido de telemetría. Si la aplicación de control lo realiza periódicamente, puede usarse como HEARBEAT.
     */
	OPCODE_REQUEST_TMY						= 0x00,

    /** Encender led de prueba.
     *  Comando para ensayos de puesta en marcha (PEM).
     */
	OPCODE_LED_ON							= 0x01,

    /** Apagar led de prueba.
     *  Comando para ensayos de puesta en marcha (PEM).
     */
	OPCODE_LED_OFF							= 0x02,

	// BEGIN Opcodes específicos de la aplicación

    /** Establecer potencia de motores (modo manual, sin PID)
     */
	OPCODE_UPDATE_MOTOR_THROTTLES			= 0x03,

    /** Establecer velocidad de motores (RPM)
     */
	OPCODE_UPDATE_MOTOR_SPEED_SETPOINTS		= 0x04,

    /** Reiniciar controlador PID
     */
	OPCODE_RESET_PID_CONTROLLER = 0x05,


	OPCODE_REQUEST_DEBUG_REPORT = 0x06,

	// END Opcodes específicos de la aplicación
	OPCODE_LAST
};

#endif
