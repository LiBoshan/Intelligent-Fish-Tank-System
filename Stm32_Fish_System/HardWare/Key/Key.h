#ifndef  __KEY_H
#define  __KEY_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "gizwits_product.h"


#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLOCK  RCC_APB2Periph_GPIOB


#define KEY_SHORT_PRESS_TIME  50
#define KEY_LONG_PRESS_TIME   3000 // 长按三秒复位
#define KEY_PRESS_SAMPLE_MS   20

void Key_Init(void);
uint8_t Key_GetNum(void);
uint8_t Key_ScanWithLongPress(void);

#endif
