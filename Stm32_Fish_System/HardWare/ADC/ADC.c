#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "ADC.h"
#include "OLED.h"

uint16_t Empty_ADC = 0;
uint16_t Full_ADC = 4095;
uint16_t AD_Value[2];

void AD_Init(void)
{
	//使能 ADC 和 GPIO 以及 DMA1 时钟
	RCC_APB2PeriphClockCmd(ADC_RCC_PERIPH_ADC, ENABLE);
    RCC_APB2PeriphClockCmd(ADC_RCC_PERIPH_GPIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* ADC clock */
	RCC_ADCCLKConfig(ADC_CLK_DIV);
	
	//配置 GPIO 为模拟输入模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SAMPLE_TIME);

	//配置 ADC 参数
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//配置 DMA 相关参数
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	//使能DMA
	DMA_Cmd(DMA1_Channel1, ENABLE);

	ADC_DMACmd(ADC1, ENABLE);

	//使能 ADC 
	ADC_Cmd(ADC1, ENABLE);
	
	//ADC 复位校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	
	//ADC 开始校准
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// uint16_t AD_GetValue(uint8_t ADC_Channel)
// {
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SAMPLE_TIME);
//     ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//     while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//     return ADC_GetConversionValue(ADC1);
// }

uint16_t Water_GetLevel(void)
{
	uint32_t  level = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        // level += AD_GetValue(ADC_Channel);
		level += AD_Value[0];
        Delay_ms(5);
    }

    level /= 10;
    uint16_t data = (level - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
	return data;
}

uint16_t Photosensitive_GetValue(void)
{
	uint32_t Photo_Value = 0;
	for (uint8_t j = 0; j < 20; j++)
    {
        // Photo_Value += AD_GetValue(ADC_Channel);
		Photo_Value += AD_Value[1];
        Delay_ms(5);
    }
	Photo_Value /= 20;
	uint16_t data = (Photo_Value - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
	return data;
}
