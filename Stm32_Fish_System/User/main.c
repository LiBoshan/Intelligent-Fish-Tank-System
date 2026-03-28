#include "main.h"

uint16_t level, angle, keynum, light, count = 0;
int16_t temp;
uint16_t Low_ThresholdVale= 0;
uint16_t Hight_ThresholdVale = 80;

int main(void)
{
	OLED_Init();
	LED_Init();
	Servo_Init();
	AD_Init();
	DS18B20_Init();
	Key_Init();

	OLED_ShowString(1, 1, "Level:");
	OLED_ShowString(1, 9, "%");

	OLED_ShowString(2, 1, "Photo:");
	OLED_ShowString(2, 9, "%");

	OLED_ShowString(3, 1, "temp :");
	
	OLED_ShowString(4, 1, "angle:");
// 	OLED_ShowString(4, 1, "count:");
	
	
	while(1)
	{	
		
		// 水位监测
		level = Water_GetLevel();
		if(level >= Hight_ThresholdVale | level <= Low_ThresholdVale)
		{
			//Water_Led_Warn();
		}
		else
		{
			LED_OFF();
		}
		OLED_ShowNum(1, 7, level, 2);

		// 光敏检测
		light = Photosensitive_GetValue();
		OLED_ShowNum(2, 7, light, 2);


		// 舵机转动
		keynum = Key_GetNum();
		if (keynum == 1)
		{
			angle += 30;
			if (angle > 180) angle = 0;
			Servo_SetAngle(angle);
			Delay_ms(10);
		}
		OLED_ShowNum(4, 7, angle, 3);

		//  DS18B20温度检测
		DS18B20_Rst();
		if(DS18B20_Check())
		{
			OLED_ShowString(4, 12, "---");
		}else OLED_ShowString(4, 12, "  ");
		temp = DS18B20_Get_Temp();
		if (temp < 0)
		{
			OLED_ShowChar(3, 7, '-');
			temp = -temp;   
		}
		else
		{
			OLED_ShowChar(3, 7, '+');
		}
		OLED_ShowNum(3, 8, temp / 10, 2);
		OLED_ShowChar(3, 10, '.'); 
		OLED_ShowNum(3, 11, temp % 10, 1);
	}
}
