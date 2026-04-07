#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void LED_Init(void);
void LED_OFF(void);
void LED_SetBrightness(uint8_t percent);

#endif
