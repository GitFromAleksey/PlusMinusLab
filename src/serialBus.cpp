/*
 * serialBus.cpp
 *
 *  Created on: 6 мар. 2021 г.
 *      Author: MSPC
 */

#include "serialBus.h"

void SerialBus__init__(SerialBus *self, USART_TypeDef *instance,
                       DMA_Channel_TypeDef *rxDMA, DMA_Channel_TypeDef *txDMA)
{
#ifdef DEBUG
	cout << "SerialBus__init__()";
#endif

	self->uart = instance;
	self->rx_dma = rxDMA;
	self->tx_dma = txDMA;
	self->dma_transfer_complete = false;
	self->receive_from = SERIALBUS_RECEIVE_FROM_NONE;

#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_setHandler(SerialBus *self, SerialBus_Handler_Type callback, void *context)
{
#ifdef DEBUG
	cout << "SerialBus_setHandler()";
#endif

	self->serial_bus_handler = callback;

#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_setRxBuffer(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
#ifdef DEBUG
	cout << "SerialBus_setRxBuffer(): ";
#endif

	self->buffer = buffer;
	self->buffer = buffer;
	self->rx_data_lenght = 0;
	self->buffer_lenght = bufferLength;
	self->tx_data_lenght = 0;

#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
//#define DEBUG_SerialBus_write
// TODO нужно реализовать согласно заданию
void SerialBus_write(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
#ifdef DEBUG
#ifdef DEBUG_SerialBus_write
	cout << "SerialBus_write(): ";
	cout << "incomming buffer: " << buffer << "; ";
#endif
#endif

	while(bufferLength != 0)
	{
		if(self->tx_data_lenght >= self->buffer_lenght)
			return;

		self->buffer[self->tx_data_lenght] = *buffer;
		++self->tx_data_lenght;

		++buffer;
		--bufferLength;
	}
	self->tx_data_counter = 0;
#ifdef DEBUG
#ifdef DEBUG_SerialBus_write
	cout << " self->buffer: " << self->buffer;
	cout << endl;
#endif
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_process(SerialBus *self)
{
#ifdef DEBUG
//	cout << "SerialBus_process(): ";
#endif

	if(self->rx_data_lenght > 0)
	{
		self->event.rx.buffer = self->buffer;
		self->event.rx.length = self->rx_data_lenght;
		self->event.type = SERIALBUS_EVENT_DATA_RECEIVED;
		self->rx_data_lenght = 0;
		self->serial_bus_handler(NULL, &self->event);
	}

	if(self->tx_data_lenght > 0)
	{
		if(self->receive_from == SERIALBUS_RECEIVE_FROM_UART)
		{
			if(self->uart->SR | USART_SR_TXE)	//!< Transmit Data Register Empty
			{
				if(self->tx_data_counter < self->tx_data_lenght)
				{
					self->uart->DR = self->buffer[self->tx_data_counter++]; // next byte
					self->uart->CR1 |= USART_CR1_TE; //!< Transmitter Enable
				}
				else
				{
					self->tx_data_lenght = 0;
					self->event.type = SERIALBUS_EVENT_TRANSMIT_COMPLETE;
					self->event.rx.buffer = self->buffer;
					self->receive_from = SERIALBUS_RECEIVE_FROM_NONE;
					self->serial_bus_handler(NULL, &self->event);
				}
			}
		}
		else if(self->receive_from == SERIALBUS_RECEIVE_FROM_DMA)
		{
			if(self->dma_transfer_complete)
			{
				if(self->tx_data_counter < self->tx_data_lenght)
				{
					// копирование байта для передачи в DMA
					// DMA = self->buffer[self->tx_data_counter];
					++self->tx_data_counter;
					// разрешение передачи DMA, если нужно
					self->dma_transfer_complete = false;
				}
				else
				{
					self->tx_data_lenght = 0;
					self->event.type = SERIALBUS_EVENT_TRANSMIT_COMPLETE;
					self->event.rx.buffer = self->buffer; // TODO для теста отправляемого сообщения
					self->receive_from = SERIALBUS_RECEIVE_FROM_NONE;
					self->serial_bus_handler(NULL, &self->event);
				}
			}
		}
	}

#ifdef DEBUG
//	cout << "SerialBus_process(): END" << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___uartIRQ(SerialBus *self)
{
#ifdef DEBUG
//	cout << "SerialBus___uartIRQ()";
#endif
	// чтение принятого по UART байта
	if(self->uart->SR & USART_SR_RXNE)
	{
		self->receive_from = SERIALBUS_RECEIVE_FROM_UART;
		if(self->rx_data_lenght < self->buffer_lenght)
		{
			self->buffer[self->rx_data_lenght] = (self->uart->DR & 0xFF);
			++self->rx_data_lenght;
		}
	}

#ifdef DEBUG
//	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___rxDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
//	cout << "SerialBus___rxDmaIRQ()";
#endif

	if(self->rx_data_lenght < self->buffer_lenght)
	{
		// копируем данные из DMA в буфер
		self->buffer[self->rx_data_lenght] = (self->rx_dma->CPAR & 0xFF);
		++self->rx_data_lenght;
		self->receive_from = SERIALBUS_RECEIVE_FROM_DMA;
	}

#ifdef DEBUG
//	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___txDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
//	cout << "SerialBus___txDmaIRQ()";
#endif

	self->dma_transfer_complete = true;

#ifdef DEBUG
//	cout << endl;
#endif
}
// ----------------------------------------------------------------------------

