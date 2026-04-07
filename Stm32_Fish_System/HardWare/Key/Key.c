#include "Key.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(KEY_GPIO_CLOCK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

uint8_t Key_ScanWithLongPress(void)
{
	uint8_t KeyNum = 0;
	uint32_t PressTime = 0;

	if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
	{
		Delay_ms(20);

		if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
		{
			PressTime = gizGetTimerCount();
			while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
			{
				Delay_ms(KEY_PRESS_SAMPLE_MS);
				if((gizGetTimerCount() - PressTime) >= KEY_LONG_PRESS_TIME)
				{
					while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
					Delay_ms(20);
					KeyNum = 2;  // 长按
					return KeyNum;
				}
			}
			if((gizGetTimerCount() - PressTime) >= KEY_SHORT_PRESS_TIME)
			{
				KeyNum = 1; // 短按
			}
		}
	}
	return KeyNum;
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	// uint32_t PressTime = 0;

	// if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
	// {
	// 	Delay_ms(20);
	// 	if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
	// 	{
	// 		PressTime = gizGetTimerCount();
	// 		while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
	// 		{
	// 			Delay_ms(KEY_PRESS_SAMPLE_MS);
	// 			if((gizGetTimerCount() - PressTime) >= KEY_LONG_PRESS_TIME)
	// 			{
	// 				while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_12) == 0)
	// 				Delay_ms(20);
	// 				KeyNum = 2;  // 长按
	// 				return KeyNum;
	// 			}
	// 		}
	// 		if((gizGetTimerCount() - PressTime) >= KEY_SHORT_PRESS_TIME)
	// 		{
	// 			KeyNum = 1; // 短按
	// 		}
	// 	}
	// }

	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_13) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_13) == 0);
		Delay_ms(20);
		KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_14) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, GPIO_Pin_14) == 0);
		Delay_ms(20);
		KeyNum = 4;
	}

	return KeyNum;
}
