#ifndef  __PTC_H
#define  __PTC_H

#include "stm32f10x.h"                  // Device header

#define PTC_GPIO_CLOCK  RCC_APB2Periph_GPIOB
#define PTC_GPIO_POPT   GPIOB
#define PTC_GPIO_PIN    GPIO_Pin_1

void PTC_Init(void);
void PTC_OFF(void);
void PTC_ON(void);

#endif
