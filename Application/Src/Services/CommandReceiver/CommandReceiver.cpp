#include "Services/CommandReceiver.h"
//#include "Application.h"
//#include <cstring>


CommandReceiverService::CommandReceiverService()
{
	 _RtosRes.QueueAttributes.name = "CommandReceiverQueue";
	 _RtosRes.QueueAttributes.cb_mem = &_RtosRes.QueueControlBlock;
	 _RtosRes.QueueAttributes.cb_size = sizeof(_RtosRes.QueueControlBlock);
	 _RtosRes.QueueAttributes.mq_mem = &_RtosRes.QueueBuffer;
	 _RtosRes.QueueAttributes.mq_size = sizeof(_RtosRes.QueueBuffer);


	 _RtosRes.ThreadAttributes.name = "CommandReceiverTask";
	 _RtosRes.ThreadAttributes.stack_size = 128 * 4;
	 _RtosRes.ThreadAttributes.priority = (osPriority_t) osPriorityNormal;


	 _CurrentCommandMailboxIdx = 0;
}

CommandReceiverService::~CommandReceiverService()
{

}

void CommandReceiverService::Init(UART_HandleTypeDef* UartTcHandle, CommandHandlerCallback CommandHandler)
{
	_HalRes.UartTcHandle =  UartTcHandle;

	_RtosRes.QueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &_RtosRes.QueueAttributes);
	_RtosRes.ThreadId = osThreadNew(CommandReceiverService::CommandReceiverServiceTaskMain, reinterpret_cast<void*>(this), &_RtosRes.ThreadAttributes);

	_CommandHandler = CommandHandler;
}

void CommandReceiverService::CommandReceiverServiceTaskMain(void *argument)
{
	CommandReceiverService* Me = reinterpret_cast<CommandReceiverService*>(argument);
    Me->Main();
}


void CommandReceiverService::Main()
{
	// Start receiving
	HAL_UART_Receive_IT(_HalRes.UartTcHandle, &_RxBuf, 1);

	for(;;)
	{
		// Read Queue
		uint8_t ReceivedCommandMailboxIdx;
		osStatus result = osMessageQueueGet(
				_RtosRes.QueueHandle,
				&ReceivedCommandMailboxIdx,
				0, // Priority (ignored)
				-1 // Infinite wait
		);

		if (result== osOK)
		{
			const uint8_t* TCData = &_CommandMailbox[ReceivedCommandMailboxIdx][0];
			_CommandHandler(TCData);
		}
		else if (result == osErrorTimeout)
		{
			// TimeOut is not a problem
		}
		else
		{
			// Other code is a problem!
		}

		//osDelay(1000);
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	}
}



void CommandReceiverService::UpdateISR(UART_HandleTypeDef* UartHandle)
{
	if(UartHandle == _HalRes.UartTcHandle)
	{
		protocol::packet_decoder::feed(_RxBuf);
		HAL_UART_Receive_IT(UartHandle, &_RxBuf, 1);
	}
}



void CommandReceiverService::handle_packet(const uint8_t* payload, uint8_t n)
{
	ReceivedPackets++;

	// FIXME: check for available mailboxes
	if ( true )
	{
		LastPacketStatus = LastPacketStatusCode::Ok;

		std::memcpy(&_CommandMailbox[_CurrentCommandMailboxIdx], payload, n);

		osStatus_t result = osMessageQueuePut(
				_RtosRes.QueueHandle,
				&_CurrentCommandMailboxIdx,
				0, // priority ignored.
				0 // 0 Timeout is required because running in interrupt
		);

		if ( osStatus_t::osOK == result )
		{
			// Next mailbox
			_CurrentCommandMailboxIdx++;
			if (_CurrentCommandMailboxIdx == CircularBufferCapacityInMessages)
			{
				_CurrentCommandMailboxIdx = 0;
			}
		}
	}
}

void CommandReceiverService::set_error(error_code ec)
{
	if (ec == invalid_length )
	{
		LastPacketStatus = LastPacketStatusCode::InvalidLength;
	}
	else if(ec == bad_crc)
	{
		LastPacketStatus = LastPacketStatusCode::BadCRC;
	}
}
