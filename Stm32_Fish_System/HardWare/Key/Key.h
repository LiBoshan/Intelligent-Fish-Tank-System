#ifndef  __KEY_H
#define  __KEY_H

#include "stm32f10x.h"                  // Device header
#include "gizwits_product.h"

#define KEY_DEBOUNCE_TIME     20
#define KEY_LONG_PRESS_TIME   1000

#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLOCK  RCC_APB2Periph_GPIOB

void Key_Init(void);
static uint8_t Key_ScanSingle(uint8_t index);
uint8_t Key_GetNum(void);

#endif
