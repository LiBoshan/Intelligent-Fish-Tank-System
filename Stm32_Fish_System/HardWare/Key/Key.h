#ifndef  __KEY_H
#define  __KEY_H

#define KEY_GPIO_PORT  RCC_APB2Periph_GPIOB
#define KEY_GPIO_PINS  GPIO_Pin_1

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif
