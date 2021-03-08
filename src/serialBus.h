#ifndef SERIAL_BUS_H
#define SERIAL_BUS_H

#include "main.hpp" // TODO незабыть удалить!!!
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

typedef enum SerialBus_ReceiveFrom {
	SERIALBUS_RECEIVE_FROM_UART,
	SERIALBUS_RECEIVE_FROM_DMA,
	SERIALBUS_RECEIVE_FROM_NONE
} SerialBus_ReceiveFrom;

typedef struct SerialBus {
	/* your members here */
	SerialBus_ReceiveFrom receive_from;
	USART_TypeDef * uart;
	DMA_Channel_TypeDef * rx_dma;
	DMA_Channel_TypeDef * tx_dma;
	bool dma_transfer_complete;
	SerialBus_Handler_Type serial_bus_handler;
	uint8_t *buffer;
	uint32_t buffer_lenght;
	uint32_t rx_data_lenght; // кол-во принятых байт
	uint32_t tx_data_lenght; // кол-во байт для отправки
	uint32_t tx_data_counter; // счетчик отправленных байт
	SerialBus_Event event;
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
