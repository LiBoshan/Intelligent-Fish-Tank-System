#ifndef __USART2_H
#define __USART2_H
 
#include "stm32f10x.h"                  // Device header
#include "gizwits_protocol.h"
#include <stdio.h>

extern uint8_t Usart2_RxPacket[6];
extern uint8_t Usart_RxFlag;

void USART2_Config(void);
void USART2_IRQHandler(void);

#endif
