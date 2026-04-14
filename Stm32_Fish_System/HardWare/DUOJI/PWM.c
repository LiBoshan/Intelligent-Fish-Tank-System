#include "PWM.h"

/**
  * @brief   PWM初始化函数,分别配置舵机和补光灯
  * @param   无
  * @retval  无
  */
void PWM_Init(void)
{
	// 舵机配置
	RCC_APB1PeriphClockCmd(Servo_TIM_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(Servo_GPIO_CLOCK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Servo_GPIO_PIN;	// PA0 = TIM2_CH1 舵机输出
	GPIO_Init(Servo_GPIO_PORT, &GPIO_InitStructure);

	TIM_InternalClockConfig(Servo_TIM_PORT);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = Servo_ARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = Servo_PSC;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(Servo_TIM_PORT, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR

	TIM_OC1Init(Servo_TIM_PORT, &TIM_OCInitStructure);

	TIM_Cmd(Servo_TIM_PORT, ENABLE);

	// 补光灯配置
	RCC_APB1PeriphClockCmd(LED_TIM_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(LED_GPIO_CLOCK, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;	// PB7 = TIM4_CH2 补光灯输出
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

	TIM_InternalClockConfig(LED_TIM_PORT);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = LED_ARR;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = LED_PSC;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(LED_TIM_PORT, &TIM_TimeBaseInitStructure);
	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR

	TIM_OC2Init(LED_TIM_PORT, &TIM_OCInitStructure);

	TIM_Cmd(LED_TIM_PORT, ENABLE);
}


/**
  * @brief   通道一设置,用于舵机
  * @param   Compare,需要的数值
  * @retval  无
  */
void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(Servo_TIM_PORT, Compare);
}


/**
  * @brief   通道二设置,用于补光灯
  * @param   Compare,需要的数值
  * @retval  无
  */
void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(LED_TIM_PORT, Compare);
}
