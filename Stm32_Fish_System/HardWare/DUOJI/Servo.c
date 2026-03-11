#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	// 限制角度范围 0~180°，对应脉宽 500~2500 (0.5ms~2.5ms @ 20ms 周期)
	if (Angle < 0) Angle = 0;
	if (Angle > 180) Angle = 180;
	PWM_SetCompare1((uint16_t)(Angle / 180 * 2000 + 500));
}
