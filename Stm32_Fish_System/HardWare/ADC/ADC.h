#ifndef __ADC_H
#define __ADC_H

extern uint16_t AD_Value[2];

//macro definition GPIO-related parameters
//宏定义 GPIO 相关参数
#define ADC_GPIO_PORT      GPIOA
#define ADC_GPIO_PIN_1     GPIO_Pin_1
#define ADC_GPIO_PIN_2     GPIO_Pin_2
#define ADC_GPIO_PINS      (GPIO_Pin_1 | GPIO_Pin_2)

//macro definition ADC-related parameters
//宏定义 ADC 相关参数
#define ADC_RCC_PERIPH_ADC   RCC_APB2Periph_ADC1
#define ADC_RCC_PERIPH_GPIO  RCC_APB2Periph_GPIOA
#define ADC_CLK_DIV          RCC_PCLK2_Div6
#define ADC_SAMPLE_TIME      ADC_SampleTime_55Cycles5


void AD_Init(void);
// uint16_t AD_GetValue(uint8_t ADC_Channel);
uint16_t Water_GetLevel(void);
uint16_t Photosensitive_GetValue(void);

#endif
