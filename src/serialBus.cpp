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
	cout << "SerialBus__init__()" << endl;
#endif

	self->uart = instance;
	self->rx_dma = rxDMA;
	self->tx_dma = txDMA;
}
// ----------------------------------------------------------------------------
void SerialBus_setHandler(SerialBus *self, SerialBus_Handler_Type callback, void *context)
{
#ifdef DEBUG
	cout << "SerialBus_setHandler()" << endl;
#endif
	self->serial_bus_handler = callback;
}
// ----------------------------------------------------------------------------
void SerialBus_setRxBuffer(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
#ifdef DEBUG
	cout << "SerialBus_setRxBuffer()" << endl;
#endif
	self->buffer = buffer;
	self->buffer_lenght = bufferLength;
	self->buffer_index = 0;
}
// ----------------------------------------------------------------------------
void SerialBus_write(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
#ifdef DEBUG
	cout << "SerialBus_write()" << endl;
	cout << buffer << endl;
#endif
}
// ----------------------------------------------------------------------------
void SerialBus_process(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus_process()" << endl;
#endif
	SerialBus_Event event;
	// это основной поток UART
	// проверка буфера на наличие входящих данных
	if(self->buffer_lenght > 0)
	{
		event.type = SERIALBUS_EVENT_DATA_RECEIVED;
		self->serial_bus_handler(NULL, &event);
	}
	// вызов calback функции
}
// ----------------------------------------------------------------------------

void SerialBus___uartIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___uartIRQ()" << endl;
#endif
	// чтение принятого по UART байта
	if(self->uart->SR & USART_SR_RXNE)
	{
		if(self->buffer_index < self->buffer_lenght)
		{
			self->buffer[self->buffer_index] = self->uart->DR & 0xFF;
			++self->buffer_index;
		}
	}

	// помещение принятого байта во входную очередь(буфер)
}
// ----------------------------------------------------------------------------
void SerialBus___rxDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___rxDmaIRQ()" << endl;
#endif
	// чтение принятого байта по UART
	// помещение принятого байта во входную очередь(буфер)
}
// ----------------------------------------------------------------------------
void SerialBus___txDmaIRQ(SerialBus *self)
{
#ifdef DEBUG
	cout << "SerialBus___txDmaIRQ()" << endl;
#endif
	// чтение байта, который на отправку из ДМА
	// тут похоже должна быть команда на отправку данных из ДМА
}
// ----------------------------------------------------------------------------

