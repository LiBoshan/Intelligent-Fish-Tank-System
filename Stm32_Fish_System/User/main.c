#include "main.h"

uint16_t level, temp;
uint16_t Low_ThresholdVale= 20;
uint16_t Hight_ThresholdVale = 80;

int main(void)
{
	OLED_Init();
	Servo_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "Level:");
	OLED_ShowString(1, 9, "%");
	OLED_ShowString(2, 1, "temp:");
	
	while(1)
	{
		
		// 水位监测
		level = Water_GetLevel(ADC_Channel_1);
		if(level > Hight_ThresholdVale | level < Low_ThresholdVale)
		{
			OLED_ShowString(3, 1, "error");
		}
		else
		{
			OLED_ShowString(3, 1, "     ");
		}
		OLED_ShowNum(1, 7, level, 2);
		
		Delay_ms(100);
		
	}
}
