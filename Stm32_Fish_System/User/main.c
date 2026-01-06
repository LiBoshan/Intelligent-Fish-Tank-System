#include "stm32f10x.h"                  // Device header
#include "OLED.h"

int main(void)
{
	OLED_Init();
	OLED_ShowChinese(1, 1 ,0);
	OLED_ShowChinese(1, 2 ,1);
	OLED_ShowChinese(1, 3 ,2);
	OLED_ShowChinese(1, 4 ,3 );
	OLED_ShowChinese(2, 1 ,10);
	OLED_ShowChinese(2, 2 ,11);
	
	while(1)
	{
		
	}
}
