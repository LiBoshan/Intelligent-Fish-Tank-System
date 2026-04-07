#ifndef  __PUMP_H
#define  __PUMP_H

#include "stm32f10x.h"                  // Device header

#define PUMP_GPIO_CLOCK  RCC_APB2Periph_GPIOB
#define PUMP_GPIO_PORT   GPIOB
#define PUMP_GPIO_PIN    GPIO_Pin_10

void Pump_Init(void);
void Pump_OFF(void);
void Pump_ON(void);

#endif
