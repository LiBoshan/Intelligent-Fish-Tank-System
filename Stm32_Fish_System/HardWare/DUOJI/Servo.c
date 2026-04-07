#include "Servo.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	uint16_t pulse = (uint16_t)(Angle / 180 * 2000 + 500);
	PWM_SetCompare1(pulse);
}
