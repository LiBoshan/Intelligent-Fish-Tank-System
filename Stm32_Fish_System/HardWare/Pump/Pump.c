#include "Pump.h"

void Pump_Init(void)
{
    RCC_APB2PeriphClockCmd(PUMP_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PUMP_GPIO_PINS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(PUMP_GPIO_PORT, &GPIO_InitStructure);
    
    // 默认关闭水泵
    Pump_IN_OFF();
    Pump_OUT_OFF();
}

void Pump_IN_ON(void)
{
    GPIO_SetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_IN);
}

void Pump_IN_OFF(void)
{
    GPIO_ResetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_IN);
}

void Pump_OUT_ON(void)
{
    GPIO_SetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_OUT);
}

void Pump_OUT_OFF(void)
{
    GPIO_ResetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_OUT);
}

