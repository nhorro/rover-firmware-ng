#include "application.h"

#include <cstring>

application::application(UART_HandleTypeDef* huart_tctm, IRQn_Type uart_tctm_interrupt) :
	opcodes
	{
		// OPCODE_REQUEST_TMY						= 0x00,
		{ &application::request_tmy, opcode_flags::default_flags },

		// OPCODE_LED_ON							= 0x01,
		{ &application::led_on, opcode_flags::enable_execution_status_report },

		// OPCODE_LED_OFF							= 0x02,
		{ &application::led_off, opcode_flags::enable_execution_status_report },

		// OPCODE_UPDATE_MOTOR_THROTTLES			= 0x03,
		{ &application::unimplemented_command, opcode_flags::enable_execution_status_report },

		// OPCODE_UPDATE_MOTOR_SPEED_SETPOINTS		= 0x04,
		{ &application::unimplemented_command, opcode_flags::enable_execution_status_report },

		// OPCODE_RESET_PID_CONTROLLER = 0x05,
		{ &application::unimplemented_command, opcode_flags::enable_execution_status_report },

		// OPCODE_REQUEST_DEBUG_REPORT = 0x06,
		{ &application::request_debug_report, opcode_flags::enable_execution_status_report },

		// BEGIN Comandos específicos de la aplicación
		// END Comandos específicos de la aplicación
	},
	huart_tctm(huart_tctm),
	uart_tctm_interrupt(uart_tctm_interrupt),

	wheel_ticks{0,0,0,0}
{

}



application::~application()
{

}



void application::setup()
{
	// Enable UART IRQ
	HAL_NVIC_SetPriority(this->uart_tctm_interrupt, 0, 0);
	HAL_NVIC_EnableIRQ(this->uart_tctm_interrupt);

	//this->read_telecommands();
}

void application::read_telecommands()
{
	HAL_UART_Receive_IT (this->huart_tctm, &this->rx_char, 1);
}

void application::handle_received_char()
{
	this->feed(this->rx_char);
}

void application::handle_packet(const uint8_t* payload, uint8_t n)
{
    // El byte 0 es el código de opcode, el resto el payload. */
	uint8_t opcode = payload[0];
	if (OPCODE_REQUEST_TMY == opcode)
	{
        /* Un pedido de general de telemetría es un caso especial.
           No queremos que los contadores y status reaccionen a este opcode. */
		(this->*(opcodes[opcode].fn))(payload + 1, n - 1);
	}
	else
	{
        // Incrementar contador de paquetes aceptados y cambiar estado de led.
		this->tmy[TMY_PARAM_ACCEPTED_PACKETS]++;
        //this->leds[0].write(!this->leds[0].read());

        // Ejecutar comando y actualizar status y código de último comando
		this->tmy[TMY_PARAM_LAST_ERROR] =
				(opcode < OPCODE_LAST) ?
						(this->*(opcodes[opcode].fn))(payload + 1,
								n - 1) :
						error_code::unknown_opcode;
		this->tmy[TMY_PARAM_LAST_OPCODE] = static_cast<uint8_t>(opcode);

		// Si está el flag habilitado, generar reporte de ejecución
		if ( this->opcodes->flags & opcode_flags::enable_execution_status_report )
		{
			this->get_payload_buffer()[0] = REPORT_COMMAND_EXECUTION_STATUS;
			this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_LAST_OPCODE];
			this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_LAST_ERROR];
			this->send(3);
		}
	}
}

void application::set_error(error_code ec)
{
	if (packet_decoder::error_code::timeout != ec)
	{
		this->tmy[TMY_PARAM_REJECTED_PACKETS]++;
		this->tmy[TMY_PARAM_LAST_ERROR] = static_cast<uint8_t>(ec);
	}
}

void application::send_impl(const uint8_t* buf, uint8_t n)
{
	HAL_UART_Transmit_IT(this->huart_tctm,buf, n);
}

void application::handle_connection_lost()
{
    // Si se perdió la conexión apagar el vehículo inmediatamente.

	//REQUEST_EXTERNAL_RESET

	//apagar motores
    /*
	this->motor_ctl.set_motor_throttles(this->throttles,
    		l298_motor_control::motor_control_flags::motor_a|
	   	    l298_motor_control::motor_control_flags::motor_b );
    */

}

/* Opcodes */

application::error_code application::request_tmy(const uint8_t* payload, uint8_t n)
{
	this->send_general_tmy_report();
	return error_code::success;
}

application::error_code application::led_on(const uint8_t* payload, uint8_t n)
{
	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_SET);
	return error_code::success;
}

application::error_code application::led_off(const uint8_t* payload, uint8_t n)
{
	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_RESET);
	return error_code::success;
}

application::error_code application::request_debug_report(const uint8_t* payload, uint8_t n)
{
	this->send_motion_control_report();
	return error_code::success;
}

application::error_code application::unimplemented_command(const uint8_t* payload, uint8_t n)
{
	return error_code::success;
}

/* Reportes */

void application::send_general_tmy_report()
{
    //printf("%03d send_general_tmy_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_GENERAL_TELEMETRY;
	this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_ACCEPTED_PACKETS];
	this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_REJECTED_PACKETS];
	this->get_payload_buffer()[3] = this->tmy[TMY_PARAM_LAST_OPCODE];
	this->get_payload_buffer()[4] = this->tmy[TMY_PARAM_LAST_ERROR];
	this->get_payload_buffer()[5] = this->tmy[TMY_PARAM_STATUS];
	this->send(1+5);
}

void application::send_motion_control_report()
{
	std::memcpy(&this->tmy[TMY_PARAM_TACHO1_COUNT_B0],this->wheel_ticks,sizeof(uint32_t)*4);

	this->get_payload_buffer()[0] = REPORT_MOTION_CONTROL_STATE;
	this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare
	std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_TACHO1_SPEED_B0], 12*4);
	this->send(4 + (12*4) );
}
