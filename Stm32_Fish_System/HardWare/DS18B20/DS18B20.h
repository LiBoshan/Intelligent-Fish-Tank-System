#ifndef  __DS18B20_H
#define  __DS18B20_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

//引脚宏定义
#define DS18B20_GPIO_PORT   GPIOB
#define DS18B20_GPIO_PIN    GPIO_Pin_0
#define DS18B20_GPIO_CLK    RCC_APB2Periph_GPIOB

//输出状态定义
#define OUT  1
#define IN   0

//设置输出高低电平
#define DS18B20_Low    GPIO_ResetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)
#define DS18B20_High   GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

void DS18B20_Mode(uint8_t mode);
int16_t DS18B20_Get_Temp(void);
void DS18B20_Rst(void);
uint8_t DS18B20_Check(void);
uint8_t DS18B20_Read_Bit(void);
uint8_t DS18B20_Read_Byte(void);
void DS18B20_Write_Byte(uint8_t data);
void DS18B20_Start(void);
uint8_t DS18B20_Init(void);

#endif
