#pragma once
#include "protocol.hpp"

namespace protocol {

/**
 * @brief 
 * 
 */
class packet_encoder
{
public:
    /** Constructor por defecto.
    */
	packet_encoder()
    {
        this->buffer[0] = PACKET_SYNC_0_CHAR;
	    this->buffer[1] = PACKET_SYNC_1_CHAR;	
    }

    /** Obtener un puntero al buffer interno.
     *  Esta función se usa para completar el contenido de los reportes a generar.
    */
	inline void* get_payload_buffer() { return this->buffer + HEADER_SIZE; }

    /** Último paso para completar un paquete. Calcular el CRC y agregar el terminador.
     *   @param length tamaño del paquete.
     */
	void calc_crc_and_close_packet(uint8_t length)
    {
       	this->payload_length = length;
        buffer[HEADER_SIZE-1] = length;
        uint16_t crc = calc_crc8(reinterpret_cast<uint8_t*>(this->get_payload_buffer()), length);
        buffer[HEADER_SIZE + length] = crc & 0xFF;	        
        buffer[HEADER_SIZE + length + 1] = PACKET_TERMINATOR_CHAR;
    }

    /** Obtener puntero inmutable al paquete */
	inline const uint8_t* get_packet_bytes() const { return this->buffer; }

    inline uint32_t get_payload_length() const { return this->payload_length; }
    
    inline uint32_t get_total_length() const { return  HEADER_SIZE + this->payload_length + TRAILER_SIZE; }

private:
    uint8_t payload_length;

    /** Buffer interno utilizado para componer el paquete. */
	uint8_t buffer[HEADER_SIZE+PAYLOAD_BUFFER_SIZE+TRAILER_SIZE] __attribute__((aligned (4)));    
};

}