#pragma once

#include "main.h"
#include "cmsis_os.h"

#include "ApplicationConfig.h"
#include "Protocol/decoder.hpp"

#include "Services/TelemetrySender.h"

#include "Telemetry.h"

extern "C" {
	void ApplicationMain(const ApplicationConfig* Config);
}

enum LastPacketStatusCode {
	Ok                 = (1 << 0), /**< Packet was dispatched. */
	InvalidLength      = (1 << 1), /**< Invalid length. */
	BadCRC             = (1 << 2), /**< Bad CRC. */
	InvalidOpcode      = (1 << 3)  /**< Opcode number was above last command. */
};


class Application :
	private protocol::packet_decoder {
public:
	Application();
	~Application();

	// TC&TM

	// Telemetry
	ApplicationGeneralTelemetry GeneralTelemetry;
	ApplicationMotorControlTelemetry MotorControlTelemetry;

	inline uint32_t GetReceivedPackets() const { return ReceivedPackets; }
	inline uint32_t GetLedControlState() const { return LedControlState; }

	const ApplicationConfig* Config;


	// Telemetry is now public, then refactor
	uint32_t TelemetryCycle = 0;
	uint32_t ReceivedPackets = 0;
	uint32_t LedControlState = 0;
	uint32_t LastCommandOpcode = 0;
	uint32_t LastCommandResult = 0;
	uint32_t LasOsResult1 = 0;
	uint32_t LasOsResult2 = 0;
	uint32_t LastPacketStatus = 0;

	// Motors
	uint32_t MotorControlModeFlags = 0;
	float MotorThrottles[2] = { 0, 0 };
	float MotorSetpointSpeeds[2] = { 0, 0 };
	float TachometerMeasuredSpeed[4] = { 0, 0, 0, 0 };

	static constexpr size_t CommandMailboxSize = 16; // Match queue size?
	static constexpr size_t MaxCommandSizeInBytes = 32;


	uint8_t CommandMailbox[CommandMailboxSize][MaxCommandSizeInBytes];
	uint8_t CurrentCommandMailboxIdx = 0;


private:
	// TC&TM

	// Telecommands
	uint8_t RxBuf;
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;

	/**
	 *
	 */
	friend void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle);

	/**
	 *
	 */
	friend void ApplicationMain(const ApplicationConfig* Config);

};



