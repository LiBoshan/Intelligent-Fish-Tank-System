#include "Pump.h"

void Pump_Init(void)
{
    RCC_APB2PeriphClockCmd(PUMP_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PUMP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PUMP_GPIO_PORT, &GPIO_InitStructure);

    Pump_OFF();
}

void Pump_OFF(void)
{
    GPIO_ResetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN);
}

void Pump_ON(void)
{
    GPIO_SetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN);
}
