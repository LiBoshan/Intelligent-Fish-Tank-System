#include "LED.h"

void LED_Init(void)
{
    PWM_Init();
}

void LED_SetBrightness(uint8_t percent)
{
    if(percent > 100) percent = 100;
    PWM_SetCompare2(percent);
}

void LED_OFF(void)
{
    PWM_SetCompare2(0);
}
