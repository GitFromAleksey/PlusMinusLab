#include "serialBus.h"

void SerialBus__init__(SerialBus *self, USART_TypeDef *instance,
                       DMA_Channel_TypeDef *rxDMA, DMA_Channel_TypeDef *txDMA)
{
	self->uart = instance;
	self->rx_dma = rxDMA;
	self->tx_dma = txDMA;
	self->dma_transfer_complete = false;
	self->receive_from = SERIALBUS_RECEIVE_FROM_NONE;
}
// ----------------------------------------------------------------------------
void SerialBus_setHandler(SerialBus *self, SerialBus_Handler_Type callback, void *context)
{
	self->serial_bus_handler = callback;
}
// ----------------------------------------------------------------------------
void SerialBus_setRxBuffer(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
	self->buffer = buffer;
	self->buffer = buffer;
	self->rx_data_lenght = 0;
	self->buffer_lenght = bufferLength;
	self->tx_data_lenght = 0;
}
// ----------------------------------------------------------------------------
void SerialBus_write(SerialBus *self, uint8_t *buffer, uint32_t bufferLength)
{
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
}
// ----------------------------------------------------------------------------
void SerialBus_process(SerialBus *self)
{
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
}
// ----------------------------------------------------------------------------
void SerialBus___uartIRQ(SerialBus *self)
{
	if(self->uart->SR & USART_SR_RXNE)
	{
		self->receive_from = SERIALBUS_RECEIVE_FROM_UART;
		if(self->rx_data_lenght < self->buffer_lenght)
		{
			self->buffer[self->rx_data_lenght] = (self->uart->DR & 0xFF);
			++self->rx_data_lenght;
		}
	}
}
// ----------------------------------------------------------------------------
void SerialBus___rxDmaIRQ(SerialBus *self)
{
	if(self->rx_data_lenght < self->buffer_lenght)
	{
		// копируем данные из DMA в буфер
		self->buffer[self->rx_data_lenght] = (self->rx_dma->CPAR & 0xFF);
		++self->rx_data_lenght;
		self->receive_from = SERIALBUS_RECEIVE_FROM_DMA;
	}
}
// ----------------------------------------------------------------------------
void SerialBus___txDmaIRQ(SerialBus *self)
{
	self->dma_transfer_complete = true;
}
// ----------------------------------------------------------------------------
