/**
 *
 */
#pragma once

#include "main.h"
#include "cmsis_os.h"

#include "Telecommands.h"
#include "Protocol/decoder.hpp"

#include <functional>

class CommandReceiverService:
	private protocol::packet_decoder {
public:
	static constexpr const size_t CircularBufferCapacityInMessages = 16;
	static constexpr size_t MaxCommandSizeInBytes = 32;

	CommandReceiverService();
	~CommandReceiverService();

	using CommandHandlerCallback = std::function<void(const uint8_t*)>;
	void Init(UART_HandleTypeDef* UartTcHandle,CommandHandlerCallback CommandHandler );
	void Main();

	// Hardware resources provided by STM32CubeMx HAL.
	struct STM32HALResources {
		STM32HALResources(){};
		UART_HandleTypeDef* UartTcHandle;
	};

	// FreeRTOS resources Provided by
	struct FreeRTOSResources {
		FreeRTOSResources(){};
		osThreadId_t ThreadId;
		osThreadAttr_t ThreadAttributes;

		osMessageQueueId_t QueueHandle;
		uint8_t QueueBuffer[ CircularBufferCapacityInMessages * sizeof( uint8_t ) ];
		StaticQueue_t QueueControlBlock;
		osMessageQueueAttr_t QueueAttributes;
	};

	// Telemetry
	enum LastPacketStatusCode {
		Ok                 = (1 << 0), /**< Packet was dispatched. */
		InvalidLength      = (1 << 1), /**< Invalid length. */
		BadCRC             = (1 << 2), /**< Bad CRC. */
		InvalidOpcode      = (1 << 3)  /**< Opcode number was above last command. */
	};

	uint32_t LastPacketStatus = 0;
	uint32_t ReceivedPackets = 0;

	void UpdateISR(UART_HandleTypeDef* UartHandle);

	CommandHandlerCallback _CommandHandler;

private:
	FreeRTOSResources _RtosRes;
	STM32HALResources _HalRes;

	static void CommandReceiverServiceTaskMain(void *argument);

	uint8_t _CommandMailbox[CircularBufferCapacityInMessages][MaxCommandSizeInBytes];
	uint8_t _CurrentCommandMailboxIdx;

	// BEGIN: Required by decoder Interface
	uint8_t _RxBuf;
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;
};
