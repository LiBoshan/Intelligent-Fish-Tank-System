#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"                  // Device header
#include "gizwits_product.h"

void Timer3_Init(uint16_t arr, uint16_t psc);
void Time1_Delay_Init(void);
void TIM3_IRQHandler(void);
void TIM1_Delay_us(uint32_t us);

#endif
