#include "main.h"
#include "protocol/protocol.h"
#include "tctm/cmd_def.h" 		// Definición de telecomandos
#include "tctm/tmy_def.h"		// Definición de variables de telemetría
#include "tctm/report_def.h"	// Definición de reportes

class application:
	public protocol::packet_decoder,
	public protocol::packet_encoder	{
public:
	/**
	 *
	 */
	application( UART_HandleTypeDef* huart_tctm,
				 IRQn_Type uart_tctm_interrupt);

	/**
	 *
	 */
	~application();

	/**
	 *
	 */
	void setup();


	/** Leer telecomandos entrantes (hardcoded a UART). */
	void read_telecommands();


	inline void increment_wheel_ticks(int wheel) { this->wheel_ticks[wheel]++; }

	/**
	 *
	 */
	void handle_received_char();
private:
	uint8_t tmy[TMY_PARAM_LAST]; // __attribute__((aligned (4)));
	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);

    /** Telecomandos */

    /** Flags de un telecomando
     *  Por defecto los telecomandos sólo se reciben y se procesan.
     *  Los flags permiten incrementar el contador de recibidos y/o generar un reporte conteniendo
     *  el estado de ejecución del comando.
     */
	enum opcode_flags {
		default_flags 				   	= 0x00,
		update_execution_counters		= 0x01, /**< Actualizar contador de ejecución. */
		enable_execution_status_report 	= 0x02  /**< Generar un reporte de status. */
	};

    /** Descriptor de un OPCODE. */
	struct opcode_descr {
		opcode_callback fn;
		uint8_t flags;
	};

    /* Tabla de opcodes */
	opcode_descr opcodes[OPCODE_LAST];

    /* Protocolo de comunicación serie */
	UART_HandleTypeDef* huart_tctm;
	IRQn_Type uart_tctm_interrupt;
	uint8_t rx_char;

	/* Tachometers */
	uint32_t wheel_ticks[4];

	/* requeridos por packet_decoder */

    /** Dispatcher de telecomandos recibidos.
      * @param payload bytes del paylaod del telecomando.
      * @param n cantidad de bytes.
      * @warning Tener mucho cuidado de desde donde se llama a esta función.
      *          Si se está ejecutando la FSM en el contexto de interrupciones,
      *          Despachar al contexto de aplicación.
      */
	void handle_packet(const uint8_t* payload, uint8_t n) override;

    /** Establecer código de error de último telecomando procesado.
      * @param error_code código de error (0=éxito)
      */
	void set_error(error_code ec) override;

    /** Implementación de envío de bytes. En este caso se utiliza puerto serie.
     * @param buf buffer con datos a transmitir (inmutable).
     * @param n cantidad de bytes.
     */
	void send_impl(const uint8_t* buf, uint8_t n) override;

    /** Manejar errores de conexión (cuando no se recibe HEARTBEAT).
     */
	void handle_connection_lost() override;


    /* Definición de comandos */

	/* Comandos :: Comandos de sistema */

    /** Pedido de telemetría */
	application::error_code request_tmy(const uint8_t* payload, uint8_t n);

    /** Encendido de led. */
	application::error_code led_on(const uint8_t* payload, uint8_t n);

    /** Apagado de led. */
	application::error_code led_off(const uint8_t* payload, uint8_t n);

	application::error_code request_debug_report(const uint8_t* payload, uint8_t n);

	application::error_code unimplemented_command(const uint8_t* payload, uint8_t n);

	/** Reportes **/

	/**
	 *
	 */
	void send_general_tmy_report();

	void send_motion_control_report();

	/* Datos y métodos específicos de la aplicación */

    /* Bloques funcionales */
};
