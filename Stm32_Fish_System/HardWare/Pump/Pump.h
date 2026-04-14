#ifndef  __PUMP_H
#define  __PUMP_H

#include "stm32f10x.h"                  // Device header

#define PUMP_GPIO_CLOCK      RCC_APB2Periph_GPIOB
#define PUMP_GPIO_PORT       GPIOB
#define PUMP_GPIO_PIN_IN     GPIO_Pin_10
#define PUMP_GPIO_PIN_OUT    GPIO_Pin_11
#define PUMP_GPIO_PINS       (GPIO_Pin_10 | GPIO_Pin_11)

void Pump_Init(void);
void Pump_IN_ON(void);
void Pump_IN_OFF(void);
void Pump_OUT_ON(void);
void Pump_OUT_OFF(void);

#endif
