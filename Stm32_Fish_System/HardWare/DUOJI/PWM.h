#ifndef __PWM_H
#define __PWM_H

#define GPIO_PORT  RCC_APB2Periph_GPIOA
#define TIM_PORT   RCC_APB1Periph_TIM2

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);

#endif
