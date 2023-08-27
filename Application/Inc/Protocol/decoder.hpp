
#pragma once
#include "protocol.hpp"

namespace protocol {

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
        
        /** Salida por timeout (agotado tiempo máximo para recibir datos de un paquete). */
		timeout
	};

    /** Constructor por defecto. */
	packet_decoder()	
        :
		    state_handlers {
			    &packet_decoder::handle_pkt_state_idle,
                &packet_decoder::handle_pkt_state_expecting_start_sync1,			
                &packet_decoder::handle_pkt_state_expecting_length,
                &packet_decoder::handle_pkt_state_expecting_payload,
                &packet_decoder::handle_pkt_state_expecting_crc,
                &packet_decoder::handle_pkt_state_expecting_terminator
		    }
    {   
	    this->reset();
    }

    /** Esta función debe ser llamada por la aplicación cada vez que se recibe un caracter. */
	void feed(uint8_t c)
    {
        this->last_received_char = c;
        (this->*(state_handlers[static_cast<int>(this->current_state)]))();
    }    

    /** Esta función debe ser llamada periódicamente por la aplicación para verificar que no se 
     *  superó el timeout. */
	void check_timeouts()
    {
        // 1. Obtener tiempo actual.
        uint32_t t1 = get_current_time_ms();

        // Verificar tiempo que transcurrió desde que se comenzo el procesamiento del paquete.
        // Si se excede, reiniciar FSM.
        uint32_t dt = this->start_of_packet_t0 > t1 ? 
            1 + this->start_of_packet_t0 + ~t1 : t1 - this->start_of_packet_t0;
        if(dt>=PACKET_TIMEOUT_IN_MS)
        {
            this->reset();
        }

        // FIXME: Verificar tiempo desde que se recibió un heartbeat.
        /*
        dt = this->last_received_packet_t0 > t1 ? 
            1 + this->last_received_packet_t0 + ~t1 : t1 - this->last_received_packet_t0;
        if(dt>HEARTBEAT_TIMEOUT_IN_MS)
        {
            this->handle_connection_lost();
        }
        */
    }    

    /** Llevar el decodificador a su estado inicial. */
	void reset()
    {    
        this->current_state = pkt_state::pkt_state_idle;
        this->received_payload_index = 0;
        this->start_of_packet_t0 = get_current_time_ms();
        this->crc8 = 0;
    }    

	/* Reemplazados por el usuario */

    /** Callback a implementar por el usuario cada vez que se recibe un paquete válido. 
     * @param payload puntero inmutable a los bytes del paquete.
     * @param n tamaño del paquete en bytes.
    */
	virtual void handle_packet(const uint8_t* payload, uint8_t n) = 0;

private:
    /** Estados de la FSM para reconocer paquetes */
	enum pkt_state
	{
        /** IDLE (estado inicial). */
		pkt_state_idle=0,

        /** Esperando 'K'. */
		pkt_state_expecting_start_sync1,

        /** Esperando tamaño en bytes. */
		pkt_state_expecting_length,

        /** Esperando bytes de payload hasta completar tamaño. */
		pkt_state_expecting_payload,

        /** Esperando CRC8. */
		pkt_state_expecting_crc,

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
	uint16_t expected_crc8;

    /** CRC16 del último paquete recibido. */
	uint8_t crc8;

    /** Último caracter recibido. */
	uint8_t last_received_char;

    /** Flag para indicar que hay nuevos datos disponibles. */
	bool new_data_available;

    /** Tamaño del payload. */
	uint8_t payload_length;

    /** Handler de FSM para estado inicial. */
	void handle_pkt_state_idle()
    {
        if (PACKET_SYNC_0_CHAR == this->last_received_char)
        {
            this->current_state =
                    pkt_state::pkt_state_expecting_start_sync1;
        }
    }

    /** Handler de FSM para recepción de secuencia de sincronismo 1. */
	void handle_pkt_state_expecting_start_sync1()
    {
        if (PACKET_SYNC_1_CHAR == this->last_received_char)
        {
            this->current_state = pkt_state::pkt_state_expecting_length;
        }
        else
        {
            this->set_error(error_code::bad_sync);
            this->reset();
        }
    }    

    /** Handler de FSM para recepción de tamaño de paquete. */
	void handle_pkt_state_expecting_length()
    {
        this->payload_length = this->last_received_char;
        if (this->payload_length && (this->payload_length <= PAYLOAD_BUFFER_SIZE))
        {
            this->current_state = pkt_state::pkt_state_expecting_payload;
        }
        else
        {
            this->set_error(error_code::invalid_length);
            this->reset();
        }
    }

    /** Handler de FSM para recepción de payload. */
	void handle_pkt_state_expecting_payload()
    {
        this->received_payload_buffer[this->received_payload_index] = this->last_received_char;
        if (++this->received_payload_index == this->payload_length)
        {        
            this->expected_crc8 = calc_crc8( this->received_payload_buffer, this->received_payload_index);
            this->received_payload_buffer[this->received_payload_index] = '\0';
            this->current_state = pkt_state::pkt_state_expecting_crc;
        }
    }    

    /** Handler de FSM para recepción de CRC. */
	void handle_pkt_state_expecting_crc()
    {
        this->crc8 = this->last_received_char;
        if(this->crc8 == this->expected_crc8)
        {
            this->current_state = pkt_state::pkt_state_expecting_terminator;
        }
        else
        {
            this->set_error(error_code::bad_crc);
            this->reset();
        }
        
    }    

    /** Handler de FSM para recepción de carácter terminador. */
	void handle_pkt_state_expecting_terminator()
    {
        if (PACKET_TERMINATOR_CHAR == this->last_received_char)
        {
            this->last_received_packet_t0 = get_current_time_ms();
            this->handle_packet(this->received_payload_buffer, this->received_payload_index);
        }
        else
        {
            this->set_error(error_code::bad_terminator);
        }
        this->reset();	
    }

    virtual void set_error(error_code ec) = 0;	
};

}