#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "ADC.h"
#include "OLED.h"

uint16_t Empty_ADC = 0;
uint16_t Full_ADC = 4095;

void AD_Init(void)
{
	//使能 ADC 和 GPIO 时钟
	RCC_APB2PeriphClockCmd(ADC_RCC_PERIPH_ADC, ENABLE);
    RCC_APB2PeriphClockCmd(ADC_RCC_PERIPH_GPIO, ENABLE);
	/* ADC clock */
	RCC_ADCCLKConfig(ADC_CLK_DIV);
	
	//配置 GPIO 为模拟输入模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
	
	//配置 ADC 参数
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//使能 ADC 
	ADC_Cmd(ADC1, ENABLE);
	
	//ADC 复位校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	
	//ADC 开始校准
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
}

uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SAMPLE_TIME);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

uint16_t Water_GetLevel(uint8_t ADC_Channel)
{
	uint32_t  level = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        level += AD_GetValue(ADC_Channel);
        Delay_ms(5);
    }

    level /= 10;
    uint16_t data = (level - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
	return data;
}

uint16_t Photosensitive_GetValue(uint8_t ADC_Channel)
{
	uint32_t Photo_Value = 0;
	for (uint8_t j = 0; j < 20; j++)
    {
        Photo_Value += AD_GetValue(ADC_Channel);
        Delay_ms(5);
    }
	Photo_Value /= 20;
	uint16_t data = (Photo_Value - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
	return data;
}
