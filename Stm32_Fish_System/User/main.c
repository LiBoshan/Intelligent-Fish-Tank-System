#include "main.h"
#include "main_freertos.h"

int main(void)
{
	OLED_Init();
	PWM_Init();
	AD_Init();
	Key_Init();
	Buzzer_Init();
	Pump_Init();
	PTC_Init();
	USART1_Config();
	USART2_Config();
	Timer3_Init(1000 - 1, 72 - 1);
	Time1_Delay_Init();
	DS18B20_Init();

	printf("=== system init ok === \r\n");
	
	userInit();
    gizwitsInit();

	freertos_start();
}
