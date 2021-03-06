//============================================================================
// Name        : PlusMinusLab.cpp
// Author      : Aleksey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "serialBus.h"


static SerialBus port;

static uint8_t buffer0[128];
static uint8_t buffer1[128];
static uint8_t *buffer[2] = {buffer0, buffer1};
static uint8_t bufferTog = 0;
static uint8_t message[] = "Echo: ";
static uint8_t terminator[] = "\r\n";


/* should not be called from ISR context! */
static void serialCallback(void *context, const SerialBus_Event *evt)
{
	switch (evt->type)
	{
	case SERIALBUS_EVENT_DATA_RECEIVED:
		bufferTog ^= 1;
		SerialBus_setRxBuffer(&port, buffer[bufferTog], sizeof(buffer0));
		// should send "Echo: {original message}\r\n"
		SerialBus_write(&port, message, sizeof(message) - 1);
		SerialBus_write(&port, evt->rx.buffer, evt->rx.length);
		SerialBus_write(&port, terminator, sizeof(terminator) - 1);
	default:
		break;
	}
}


int main ()
{
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	/* init pins here */
	SerialBus__init__(&port, USART1, DMA1_Channel2, DMA1_Channel3);
	SerialBus_setHandler(&port, serialCallback, NULL);
	/* enable interrupts here */
	SerialBus_setRxBuffer(&port, buffer0, sizeof(buffer0));
	for (;;)
	{
		SerialBus_process(&port);
		__WFI();
	}
}


void DMA1_Channel2_3_IRQHandler()
{
	uint32_t DMA_ISR = DMA1->ISR;
	if (DMA_ISR & DMA_ISR_TCIF2)
	{
		DMA1->IFCR = DMA_IFCR_CTCIF2;
		SerialBus___rxDmaIRQ(&port);
	}
	if (DMA_ISR & DMA_ISR_TCIF3)
	{
		DMA1->IFCR = DMA_IFCR_CTCIF3;
		SerialBus___txDmaIRQ(&port);
	}
}


void USART1_IRQHandler()
{
	SerialBus___uartIRQ(&port);
}
