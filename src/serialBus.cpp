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
	cout << "SerialBus_setRxBuffer(): ";
#endif

	self->rx_buffer_index = 0;
	self->evt.rx.buffer = buffer;
	self->evt.rx.length = bufferLength;

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
		if(self->tx_buffer_index >= self->evt.tx.length)
			return;

//		self->evt.tx.buffer[self->tx_buffer_index] = *buffer;
		// TODO хрен знает как тут писать в tx буфер, так как он const. Будем писать пока в rx
		self->evt.rx.buffer[self->tx_buffer_index] = *buffer;
		++self->tx_buffer_index;

		++buffer;
		--bufferLength;
	}

#ifdef DEBUG
#ifdef DEBUG_SerialBus_write
	cout << " self->evt.rx.buffer: " << self->evt.rx.buffer;
	cout << endl;
#endif
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_process(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus_process(): ";
#endif
	SerialBus_Event event;
	// это основной поток UART
	// проверка буфера на наличие входящих данных
	if(self->rx_buffer_index > 0)
	{
		cout << "Received message: " << self->evt.tx.buffer << endl;

		self->evt.rx.length = self->rx_buffer_index;
		self->rx_buffer_index = 0;
		event = self->evt;
		self->serial_bus_handler(NULL, &event);
	}

	if(self->tx_buffer_index > 0)
	{
		cout << "Transmit echo message: " << self->evt.tx.buffer << endl;


		self->evt.tx.length = self->tx_buffer_index;
		self->tx_buffer_index = 0;
		self->evt.type = SERIALBUS_EVENT_TRANSMIT_COMPLETE;
		event = self->evt;
		self->serial_bus_handler(NULL, &event);
	}

#ifdef DEBUG
	cout << "SerialBus_process(): END" << endl;
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
		if(self->rx_buffer_index < self->evt.rx.length)
		{
			self->evt.rx.buffer[self->rx_buffer_index] = (self->uart->DR & 0xFF);
			++self->rx_buffer_index;
			self->evt.type = SERIALBUS_EVENT_DATA_RECEIVED;
		}
	}

	// помещение принятого байта во входную очередь(буфер)
#ifdef DEBUG
//	cout << endl;
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

