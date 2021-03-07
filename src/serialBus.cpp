/*
 * serialBus.cpp
 *
 *  Created on: 6 мар. 2021 г.
 *      Author: MSPC
 */

#include "serialBus.h"

void SerialBus__init__(SerialBus *self,USART_TypeDef *instance,
                       DMA_Channel_TypeDef *rxDMA, DMA_Channel_TypeDef *txDMA)
{
#ifdef DEBUG
	cout << "SerialBus__init__()";
#endif

	self->uart = instance;
	self->rx_dma = rxDMA;
	self->tx_dma = txDMA;

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
	cout << "SerialBus_setRxBuffer()";
#endif

	self->buffer = buffer;
	self->buffer_lenght = bufferLength;
	self->buffer_index = 0;

#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus_write(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
#ifdef DEBUG
	cout << "SerialBus_write(): ";
	cout << "incomming buffer: " << buffer;
#endif

	while(bufferLength != 0)
	{
		*self->buffer = *buffer;
		++self->buffer;
		++buffer;
		--bufferLength;

		if(++self->buffer_index >= self->buffer_lenght)
			break;
	}

#ifdef DEBUG
	cout << "; self->buffer: " << self->buffer;
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_process(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus_process()";
#endif
	SerialBus_Event event;
	// это основной поток UART
	// проверка буфера на наличие входящих данных
	if(self->buffer_lenght > 0)
	{
		event.type = SERIALBUS_EVENT_DATA_RECEIVED;
		event.rx.buffer = self->buffer;
		event.rx.length = self->buffer_index;
		self->serial_bus_handler(NULL, &event);
		self->buffer_index = 0;
	}

#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___uartIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___uartIRQ()";
#endif
	// чтение принятого по UART байта
	if(self->uart->SR & USART_SR_RXNE)
	{
		cout << (char)self->uart->DR << endl;
		if(self->buffer_index < self->buffer_lenght)
		{
			self->buffer[self->buffer_index] = self->uart->DR & 0xFF;
			++self->buffer_index;
		}
	}

	// помещение принятого байта во входную очередь(буфер)
#ifdef DEBUG
	cout << endl;
#endif
}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___rxDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___rxDmaIRQ()";
#endif
	// чтение принятого байта по UART
	// помещение принятого байта во входную очередь(буфер)
#ifdef DEBUG
	cout << endl;
#endif

}
// ----------------------------------------------------------------------------
// TODO нужно реализовать согласно заданию
void SerialBus___txDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___txDmaIRQ()";
#endif
	// чтение байта, который на отправку из ДМА
	// тут похоже должна быть команда на отправку данных из ДМА

#ifdef DEBUG
	cout << endl;
#endif

}
// ----------------------------------------------------------------------------

