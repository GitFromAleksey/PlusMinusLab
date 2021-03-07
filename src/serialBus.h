#ifndef SERIAL_BUS_H
#define SERIAL_BUS_H

#include "main.hpp"
#include "stm32f103xb.h"
#include <stdint.h>

#ifdef DEBUG
	#include <iostream>
	using namespace std;
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef enum SerialBus_Event_Type {
	SERIALBUS_EVENT_DATA_RECEIVED,
	SERIALBUS_EVENT_TRANSMIT_COMPLETE
} SerialBus_Event_Type;


typedef struct SerialBus_TxEvent {
	const uint8_t *buffer;
	uint32_t length;
} SerialBus_TxEvent;


typedef struct SerialBus_RxEvent {
	uint8_t *buffer;
	uint32_t length;
} SerialBus_RxEvent;


typedef struct SerialBus_Event {
	SerialBus_Event_Type type;
	union {
		SerialBus_RxEvent rx;
		SerialBus_TxEvent tx;
	};
} SerialBus_Event;


typedef void (*SerialBus_Handler_Type)(void *context, const SerialBus_Event *evt);


typedef struct SerialBus {
	/* your members here */
	USART_TypeDef * uart;
	DMA_Channel_TypeDef * rx_dma;
	DMA_Channel_TypeDef * tx_dma;
	SerialBus_Handler_Type serial_bus_handler;
	uint8_t * buffer;
	uint16_t buffer_lenght;
	uint16_t buffer_index;
} SerialBus;


void SerialBus__init__(SerialBus *self,USART_TypeDef *instance,
                       DMA_Channel_TypeDef *rxDMA, DMA_Channel_TypeDef *txDMA);
void SerialBus_setHandler(SerialBus *self, SerialBus_Handler_Type, void *context);
void SerialBus_setRxBuffer(SerialBus *self, uint8_t *buffer, uint32_t bufferLength);
void SerialBus_write(SerialBus *self, uint8_t *buffer, uint32_t bufferLength);
void SerialBus_process(SerialBus *self);


void SerialBus___uartIRQ(SerialBus *self);
void SerialBus___rxDmaIRQ(SerialBus *self);
void SerialBus___txDmaIRQ(SerialBus *self);


#ifdef __cplusplus
}
#endif

#endif  // SERIAL_BUS_H
