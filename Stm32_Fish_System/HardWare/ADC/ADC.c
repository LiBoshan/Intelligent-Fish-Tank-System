#include "ADC.h"

uint16_t Empty_ADC = 0;
uint16_t Full_ADC = 4095;
uint16_t AD_Value[3];

/**
  * @brief   ADC初始化
  * @param   无
  * @retval  无
  */
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
	
	ADC_RegularChannelConfig(ADC_PORT, ADC_Channel_5, 1, ADC_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC_PORT, ADC_Channel_6, 2, ADC_SAMPLE_TIME);
	ADC_RegularChannelConfig(ADC_PORT, ADC_Channel_7, 3, ADC_SAMPLE_TIME);

	//配置 ADC 参数
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC_PORT, &ADC_InitStructure);
	
	//配置 DMA 相关参数
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC_PORT->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 3;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	//使能DMA
	DMA_Cmd(DMA1_Channel1, ENABLE);

	ADC_DMACmd(ADC_PORT, ENABLE);

	//使能 ADC 
	ADC_Cmd(ADC_PORT, ENABLE);
	
	//ADC 复位校准
	ADC_ResetCalibration(ADC_PORT);
	while (ADC_GetResetCalibrationStatus(ADC_PORT) == SET);
	
	//ADC 开始校准
	ADC_StartCalibration(ADC_PORT);
	while (ADC_GetCalibrationStatus(ADC_PORT) == SET);
	
	ADC_SoftwareStartConvCmd(ADC_PORT, ENABLE);
}

uint16_t TS_GetData(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
        sum += AD_Value[0];
        for (volatile uint32_t delay = 0; delay < 200; delay++);
    }
    uint16_t adc_raw = sum / 10;
    uint32_t voltage_div_mv = (uint32_t)adc_raw * 3300 / Full_ADC;
    float f = (float)voltage_div_mv / U0_DIV_MV;
    if (f > 1.0f) f = 1.0f;
    if (f < 0.15f) f = 0.15f;
    uint16_t ntu = (uint16_t)((1.0f - f) / 0.85f * 4000.0f);
    if (ntu > 4000) ntu = 4000;
    // 将NTU值转换为百分比 (0-100)
    uint16_t percentage_turbidity = (uint16_t)((float)ntu / 4000.0f * 100.0f);
    return percentage_turbidity;
}


/**
  * @brief   获取水位传感器数值
  * @param   无
  * @retval  水位值
  */
uint16_t Water_GetLevel(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        sum += AD_Value[1];
		for (volatile uint32_t delay = 0; delay < 200; delay++);
    }
    uint16_t level = sum / 10;
    uint16_t data = (level - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
    return data;
}


/**
  * @brief   获取光敏传感器数值
  * @param   无
  * @retval  光照强度
  */
uint16_t Photosensitive_GetValue(void)
{
	uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        sum += AD_Value[2];
		for (volatile uint32_t delay = 0; delay < 200; delay++);
    }
    uint16_t Photo_Value = sum / 10;
    uint16_t data = (Photo_Value - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
    return data;
}
