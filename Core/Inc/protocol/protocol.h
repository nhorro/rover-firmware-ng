/**
 * @file protocol.cpp Protocolo de comunicación basado en paquetes, inspirado en MAVLINK y otros similares.
 */

#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#include "../main.h"
#include "../config.h"

namespace protocol {

#define HEADER_SIZE (4+1) /* SYNC + LENGTH */
#define PAYLOAD_BUFFER_SIZE 96
#define TRAILER_SIZE (2+1) /* CRC16 + TERMINATOR */

#define MAX_PACKET_SIZE = HEADER_SIZE + PAYLOAD_BUFFER_SIZE + TRAILER_SIZE

const char PACKET_SYNC_0_CHAR = 'P';
const char PACKET_SYNC_1_CHAR = 'K';
const char PACKET_SYNC_2_CHAR = 'T';
const char PACKET_SYNC_3_CHAR = '!';
const char PACKET_TERMINATOR_CHAR = '\n';

/** Codificador de paquetes.
 *  Usado para generar paquetes que envía la aplicación (reportes).
*/
class packet_encoder
{
public:
    /** Constructor por defecto.
    */
	packet_encoder();

    /** Obtener un puntero al buffer interno.
     *  Esta función se usa para completar el contenido de los reportes a generar.
    */
	uint8_t* get_payload_buffer();

    /** Enviar reporte.
     */
	void send(uint8_t length);

	/** Reemplazado por el usuario */
	virtual void send_impl(const uint8_t* buf, uint8_t n) = 0;
private:
    /** Buffer interno utilizado para componer el paquete. */
	uint8_t buffer[HEADER_SIZE+PAYLOAD_BUFFER_SIZE+TRAILER_SIZE] __attribute__((aligned (4)));

protected:
    /** Último paso para completar un paquete. Calcular el CRC y agregar el terminador.
     *   @param length tamaño del paquete.
     */
	void calc_crc_and_close_packet(uint8_t length);

    /** Obtener puntero inmutable al paquete */
	const uint8_t* get_packet() const;
};


/** Decodificador de paquetes.
 *  Usado para recibir paquetes generados por la aplicación de control (telecomandos).
*/
class packet_decoder
{
public:
	const uint32_t cmd_timeout = 1000000;

    /** Códigos de error por defecto.
     */
	enum error_code
	{
        /** Paquete recibido con éxito. */
		success = 0,

        /** Error de sincronización (el caracter recibido no correspondía al estado actual de la FSM. */
		bad_sync = 1,

        /** Tamaño de paquete inválido. */
    	invalid_length = 2,

        /** Error de CRC. */
		bad_crc = 3,

        /** El terminador recibido no es el esperado. */
		bad_terminator = 4,

        /** OPCODE no reconocido por la aplicación. */
		unknown_opcode = 5,

        /** Salida por timeout (agotado tiempo máximo para recibir datos de un paquete). */
		timeout
	};

    /** Constructor por defecto. */
	packet_decoder();

    /** Esta función debe ser llamada por la aplicación cada vez que se recibe un caracter. */
	void feed(uint8_t c);

    /** Esta función debe ser llamada periódicamente por la aplicación para verificar que no se
     *  superó el timeout. */
	void check_timeouts();

    /** Llevar el decodificador a su estado inicial. */
	void reset();

	/* Reemplazados por el usuario */

    /** Callback a implementar por el usuario cada vez que se recibe un paquete válido.
     * @param payload puntero inmutable a los bytes del paquete.
     * @param n tamaño del paquete en bytes.
    */
	virtual void handle_packet(const uint8_t* payload, uint8_t n) = 0;

    /** Establecer un código de error de aplicación luego de procesar el paquete (0=éxito).
    */
	virtual void set_error(error_code ec) = 0;

    /** El protocolo de comunicación establece que debe existir algún mensaje períodico enviado
     *  por la aplicación de control para asegurar que la comunicación está sana.
     *  Este mensaje puede ser por ejemplo un pedido de telemetría general o cualquier otro comando,
     *  al que se califica como HEARTBEAT. Cuando se deja de recibir este mensaje, se invoca esta callback
     *  que la aplicación debe usar para gestionar una reconexión o pasar a un estado seguro (apagar motores, etc.).
     */
	virtual void handle_connection_lost() = 0;
private:
    /** Estados de la FSM para reconocer paquetes */
	enum pkt_state
	{
        /** IDLE (estado inicial). */
		pkt_state_idle=0,

        /** Esperando 'K'. */
		pkt_state_expecting_start_sync1,

        /** Esperando 'T'. */
		pkt_state_expecting_start_sync2,

        /** Esperando '!'. */
		pkt_state_expecting_start_sync3,

        /** Esperando tamaño en bytes. */
		pkt_state_expecting_length,

        /** Esperando bytes de payload hasta completar tamaño. */
		pkt_state_expecting_payload,

        /** Esperando CRC16 (MSB). */
		pkt_state_expecting_crc1,

        /** Esperando CRC16 (LSB). */
		pkt_state_expecting_crc2,

        /** Esperando caracter terminador '!'. */
		pkt_state_expecting_terminator,

		pkt_state_last
	};

    /** Callback para FSM de reconocimiento.
     */
	using packet_state_handler = void(packet_decoder::*)(void);

    /** Tabla con callbacks asociados a cada estado.
    */
	packet_state_handler state_handlers[pkt_state::pkt_state_last];

    /** Timestamp de comienzo de paquete. */
	uint32_t start_of_packet_t0;

    /** Timestamp del último paquete recibido. */
	uint32_t last_received_packet_t0;

    /** Estado actual de la FSM de reconocimiento. */
	pkt_state current_state;

    /** Buffer interno con payload del paquete actual. */
	uint8_t received_payload_buffer[PAYLOAD_BUFFER_SIZE + 1];

    /** Cabezal de escritura en el buffer interno de payload. */
	uint8_t received_payload_index;

    /** CRC16 esperado. */
	uint16_t expected_crc16;

    /** CRC16 del último paquete recibido. */
	uint16_t crc16;

    /** Último caracter recibido. */
	uint8_t last_received_char;

    /** Flag para indicar que hay nuevos datos disponibles. */
	bool new_data_available;

    /** Tamaño del payload. */
	uint8_t payload_length;

    /** Handler de FSM para estado inicial. */
	void handle_pkt_state_idle();

    /** Handler de FSM para recepción de secuencia de sincronismo 1. */
	void handle_pkt_state_expecting_start_sync1();

    /** Handler de FSM para recepción de secuencia de sincronismo 2. */
	void handle_pkt_state_expecting_start_sync2();

    /** Handler de FSM para recepción de secuencia de sincronismo 3. */
	void handle_pkt_state_expecting_start_sync3();

    /** Handler de FSM para recepción de tamaño de paquete. */
	void handle_pkt_state_expecting_length();

    /** Handler de FSM para recepción de payload. */
	void handle_pkt_state_expecting_payload();

    /** Handler de FSM para recepción de CRC (MSB). */
	void handle_pkt_state_expecting_crc1();

    /** Handler de FSM para recepción de CRC (LSB). */
	void handle_pkt_state_expecting_crc2();

    /** Handler de FSM para recepción de carácter terminador. */
	void handle_pkt_state_expecting_terminator();
};

uint16_t calc_crc16(const uint8_t* data_p, uint8_t length);

} // namespace serialprotocol

#endif // PACKET_DECODER_H
