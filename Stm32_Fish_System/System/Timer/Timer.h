#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"                  // Device header
#include "gizwits_product.h"

void Timer_Init(uint16_t arr, uint16_t psc);
void TIM3_IRQHandler(void);

#endif
