#include "Servo.h"

void Servo_Init(void)
{
	PWM_Init();
}


/**
  * @brief   舵机旋转角度函数
  * @param   Angle,设置旋转角度
  * @retval  无
  */
void Servo_SetAngle(float Angle)
{
	uint16_t pulse = (uint16_t)(Angle / 180 * 2000 + 500);
	PWM_SetCompare1(pulse);
}
