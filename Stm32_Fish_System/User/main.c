#include "main.h"

extern dataPoint_t currentDataPoint;

uint16_t level;
uint16_t angle, keynum, keynum_esp, light, count = 0;
int16_t temp;
uint16_t Low_ThresholdVale= 0;
uint16_t Hight_ThresholdVale = 80;

int main(void)
{
	OLED_Init();
	PWM_Init();
	AD_Init();
	DS18B20_Init();
	Key_Init();
	Buzzer_Init();
	Pump_Init();
	USART1_Config();
	USART2_Config();
	Timer_Init(1000 - 1, 72 - 1);

	printf("=== system init ok === \r\n");

	OLED_ShowString(1, 1, "Level:");
	OLED_ShowString(1, 9, "%");

	OLED_ShowString(2, 1, "Photo:");
	OLED_ShowString(2, 9, "%");

	OLED_ShowString(3, 1, "temp :");
	
	OLED_ShowString(4, 1, "angle:");
	
	
	Delay_ms(500);
	userInit();
    gizwitsInit();
	Delay_ms(500);

	while(1)
	{	
		// 水位监测
		level = Water_GetLevel();
		if(level >= Hight_ThresholdVale || level <= Low_ThresholdVale)
		{
			Buzzer_Turn();
		}
		else
		{
			Buzzer_OFF();
		}
		OLED_ShowNum(1, 7, level, 2);

		// 光敏检测
		light = Photosensitive_GetValue();
		LED_SetBrightness(light);
		OLED_ShowNum(2, 7, light, 2);


		//  DS18B20温度检测
		DS18B20_Rst();
		if(DS18B20_Check())
		{
			OLED_ShowString(1, 12, "---");
		}else OLED_ShowString(1, 12, "   ");
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


		// 舵机转动
		keynum = Key_GetNum();
		if (keynum == 3)
		{
			angle += 30;
			if (angle > 180) angle = 0;
			Servo_SetAngle(angle);
			Delay_ms(10);
		}
		OLED_ShowNum(4, 7, angle, 3);

		// 水泵模块
		if(keynum == 4)
		{
			count ++;
			if(count % 2 == 1) 
			{
				Pump_ON();
				OLED_ShowString(2, 14, "ON ");
			}
			if(count % 2 == 0)
			{
				Pump_OFF();
				OLED_ShowString(2, 14, "OFF");
			}
		}

		// esp8266-01s 模块
		keynum_esp = Key_ScanWithLongPress();
		if(keynum_esp == 1)
		{
			//printf("short press -> airlink mode\r\n");
			//gizwitsSetMode(WIFI_AIRLINK_MODE);
			printf("short press -> soft mode\r\n");
			gizwitsSetMode(WIFI_SOFTAP_MODE);
			OLED_ShowString(4, 14, "S");
		}
		else if(keynum_esp == 2)
		{
			printf("long press -> reset mode\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);
			OLED_ShowString(4, 14, "R");
		}
		
		userHandle();
        gizwitsHandle(&currentDataPoint);

	}
}
