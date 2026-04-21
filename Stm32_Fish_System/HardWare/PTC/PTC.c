#include "PTC.h"

void PTC_Init(void)
{
    RCC_APB2PeriphClockCmd(PTC_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PTC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PTC_GPIO_PORT, &GPIO_InitStructure);

    PTC_OFF();
}

void PTC_ON(void)
{
    GPIO_SetBits(PTC_GPIO_PORT, PTC_GPIO_PIN);
}

void PTC_OFF(void)
{
    GPIO_ResetBits(PTC_GPIO_PORT, PTC_GPIO_PIN);
}
