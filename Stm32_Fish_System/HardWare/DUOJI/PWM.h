#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"                  // Device header

#define Servo_GPIO_CLOCK    RCC_APB2Periph_GPIOA
#define Servo_TIM_CLOCK     RCC_APB1Periph_TIM2

#define LED_GPIO_CLOCK      RCC_APB2Periph_GPIOB
#define LED_TIM_CLOCK       RCC_APB1Periph_TIM4

#define Servo_ARR            20000 - 1
#define Servo_PSC            72 - 1
#define Servo_GPIO_PORT     GPIOA
#define Servo_GPIO_PIN      GPIO_Pin_0
#define Servo_TIM_PORT      TIM2


#define LED_ARR             100 - 1
#define LED_PSC             72 - 1
#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        GPIO_Pin_7
#define LED_TIM_PORT        TIM4

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);

#endif
