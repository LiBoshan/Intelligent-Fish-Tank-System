#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>

extern uint16_t AD_Value[3];

#define TS_K               204.17

//macro definition GPIO-related parameters
//宏定义 GPIO 相关参数
#define ADC_GPIO_PORT      GPIOA
#define ADC_GPIO_PIN_1     GPIO_Pin_5
#define ADC_GPIO_PIN_2     GPIO_Pin_6
#define ADC_GPIO_PIN_3     GPIO_Pin_7
#define ADC_GPIO_PINS      (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)

//macro definition ADC-related parameters
//宏定义 ADC 相关参数
#define ADC_PORT             ADC1
#define ADC_RCC_PERIPH_ADC   RCC_APB2Periph_ADC1
#define ADC_RCC_PERIPH_GPIO  RCC_APB2Periph_GPIOA
#define ADC_CLK_DIV          RCC_PCLK2_Div6
#define ADC_SAMPLE_TIME      ADC_SampleTime_55Cycles5


void AD_Init(void);
uint16_t TS_GetData(void);
uint16_t Water_GetLevel(void);
uint16_t Photosensitive_GetValue(void);

#endif
