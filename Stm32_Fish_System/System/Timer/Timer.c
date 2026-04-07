#include "Timer.h"

void Timer_Init(uint16_t arr, uint16_t psc)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructInitStructure;
    
    TIM_TimeBaseStructInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructInitStructure.TIM_Period = arr;
    TIM_TimeBaseStructInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructInitStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);

        gizTimerMs();
    }

}

