//============================================================================
// Name        : PlusMinusLab.cpp
// Author      : Aleksey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;


#include "stm32f103xb.h"
#include "serialBus.h"
#include "main.hpp"

#ifdef DEBUG
	USART_TypeDef uart1;
	#undef USART1
	#define USART1	&uart1

	DMA_Channel_TypeDef dma1_channel2;
	DMA_Channel_TypeDef dma1_channel3;
	#undef DMA1_Channel2
	#define DMA1_Channel2 &dma1_channel2

	#undef DMA1_Channel3
	#define DMA1_Channel3 &dma1_channel3

	const char TestMsg1[] = "message1";
	const char TestMsg2[] = "message2";
	const char TestMsg3[] = "message3";
#endif

static SerialBus port;

static uint8_t buffer0[128];
static uint8_t buffer1[128];
static uint8_t *buffer[2] = {buffer0, buffer1};
static uint8_t bufferTog = 0;
static uint8_t message[] = "Echo: ";
static uint8_t terminator[] = "\r\n";

#ifdef DEBUG
void USART1_IRQHandler();
void DMA1_Channel2_3_IRQHandler();
#endif

/* should not be called from ISR context! */
static void serialCallback(void *context, const SerialBus_Event *evt)
{
#ifdef DEBUG
	cout << "serialCallback(): BEGIN" << endl;
#endif
	switch (evt->type)
	{
		case SERIALBUS_EVENT_DATA_RECEIVED:
#ifdef DEBUG
	cout << "SERIALBUS_EVENT_DATA_RECEIVED,";
#endif
			bufferTog ^= 1;
			SerialBus_setRxBuffer(&port, buffer[bufferTog], sizeof(buffer0));
			// should send "Echo: {original message}\r\n"
			SerialBus_write(&port, message, sizeof(message) - 1);
			SerialBus_write(&port, evt->rx.buffer, evt->rx.length);
			SerialBus_write(&port, terminator, sizeof(terminator) - 1);
			break;
		case SERIALBUS_EVENT_TRANSMIT_COMPLETE:
#ifdef DEBUG
	cout << "SERIALBUS_EVENT_TRANSMIT_COMPLETE,";
#endif
			break;
		default:
			break;
	}
#ifdef DEBUG
	cout << "serialCallback(): END" << endl;
#endif
}


int main ()
{
#ifdef DEBUG
	cout << "!!!Start program!!!" << endl;
#endif

	/* init pins here */
	SerialBus__init__(&port, USART1, DMA1_Channel2, DMA1_Channel3);
	SerialBus_setHandler(&port, serialCallback, NULL);
	/* enable interrupts here */
	SerialBus_setRxBuffer(&port, buffer0, sizeof(buffer0));
	for (size_t i = 0; i < 1; ++i)
	{
#ifdef DEBUG
		cout << "Main thread i = " << i << endl;

		for(size_t j = 0; j < sizeof(TestMsg1)-1; ++j)
		{
			port.uart->DR = TestMsg1[j];
			USART1_IRQHandler();
		}
		SerialBus_process(&port);

		for(size_t j = 0; j < 17; ++j)
			SerialBus_process(&port);
//		for(size_t j = 0; j < sizeof(TestMsg2)-1; ++j)
//		{
//			port.uart->DR = TestMsg2[j];
//			USART1_IRQHandler();
//		}
//		SerialBus_process(&port);
//
//		for(size_t j = 0; j < sizeof(TestMsg3)-1; ++j)
//		{
//			port.uart->DR = TestMsg3[j];
//			USART1_IRQHandler();
//		}
//		SerialBus_process(&port);

#endif
		SerialBus_process(&port);
		__WFI();
	}
	cout << "!!!End program!!!" << endl;
}


void DMA1_Channel2_3_IRQHandler()
{
#ifdef DEBUG
	cout << "DMA1_Channel2_3_IRQHandler()" << endl;
#endif

	uint32_t DMA_ISR = DMA1->ISR;
	if (DMA_ISR & DMA_ISR_TCIF2) //!< Channel 2 Transfer Complete flag
	{
		DMA1->IFCR = DMA_IFCR_CTCIF2;
		SerialBus___rxDmaIRQ(&port);
	}
	if (DMA_ISR & DMA_ISR_TCIF3) //!< Channel 3 Transfer Complete flag
	{
		DMA1->IFCR = DMA_IFCR_CTCIF3;
		SerialBus___txDmaIRQ(&port);
	}
}


void USART1_IRQHandler()
{
#ifdef DEBUG
//	cout << "USART1_IRQHandler()" << endl;
	port.uart->SR |= USART_SR_RXNE;
#endif
	SerialBus___uartIRQ(&port);
}
