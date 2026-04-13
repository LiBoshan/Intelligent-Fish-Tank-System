#include "DS18B20.h"

void DS18B20_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS18B20_GPIO_CLK, ENABLE);
	
	if(mode)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
	}
	GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);
	
}

void DS18B20_Rst(void)
{
	DS18B20_Mode(OUT);
	DS18B20_Low;
	TIM1_Delay_us(750);
	DS18B20_High;
	TIM1_Delay_us(15);
}

/**
  * @brief   检测是否获取到DS18B20模块
  * @param   无
  * @retval  成功返回 0 
             失败返回 1 
  */
uint8_t DS18B20_Check(void)
{
	uint8_t retry = 0;
	DS18B20_Mode(IN);
	while(GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN) && retry < 200)
	{
		retry++;
		TIM1_Delay_us(1);
	}
	if(retry >= 200) return 1;
	else retry = 0;
	while(!GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN) && retry < 240)
	{
		retry++;
		TIM1_Delay_us(1);
	}
	if(retry >= 240) return 1;
	return 0;
}

/**
  * @brief   读取DS18B20的一个位
  * @param   无
  * @retval  返回 0 或 1
  */
uint8_t DS18B20_Read_Bit(void)
{
	uint8_t data;
	DS18B20_Mode(OUT);
	DS18B20_Low;
	TIM1_Delay_us(2);
	DS18B20_High;
	DS18B20_Mode(IN);
	TIM1_Delay_us(12);
	if(GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)) data =1;
	else data = 0;
	TIM1_Delay_us(50);
	return data;
} 

/**
  * @brief   读取DS18B20的一个字节
  * @param   无
  * @retval  读取的字节
  */
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, data;
	data = 0;
	for(i = 0; i < 8; i++)
	{
		j = DS18B20_Read_Bit();
		data = (j << 7) | (data >> 1);
	}
	return data;
}

void DS18B20_Write_Byte(uint8_t data)
{
	uint8_t j, value;
	DS18B20_Mode(OUT);
	for(j = 0; j < 8; j++)
	{
		value = data & 0x01;
		data = data >> 1;
		if(value)
		{
			DS18B20_Low; // write 1
			TIM1_Delay_us(2);
			DS18B20_High;
			TIM1_Delay_us(60);
		}
		else
		{
			DS18B20_Low; // write 0
			TIM1_Delay_us(60);
			DS18B20_High;
			TIM1_Delay_us(2);
		}
	}
}

void DS18B20_Start(void)
{
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xCC);
	DS18B20_Write_Byte(0x44);
}

uint8_t DS18B20_Init(void)
{
	RCC_APB2PeriphClockCmd(DS18B20_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN);
	
	DS18B20_Rst();

	return DS18B20_Check();
}

/**
  * @brief   获取温度
  * @param   无
  * @retval  返回具体温度值
  */
int16_t DS18B20_Get_Temp(void)
{
	uint8_t TL, TH, tem;
	int16_t temp;
	DS18B20_Start();
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xCC);
	DS18B20_Write_Byte(0xBE);
	TL = DS18B20_Read_Byte();
	TH = DS18B20_Read_Byte();
	
	if(TH > 7)
	{
		TH = ~TH;
		TL = ~TL;
		tem = 0; // 温度为负
	}
	else tem = 1; // 温度为正
	temp = TH;
	temp <<= 8;
	temp += TL;
	temp = (float)temp * 0.625;
	if(tem) return temp;
	else return -temp;
}
