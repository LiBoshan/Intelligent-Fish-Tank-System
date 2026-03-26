//#ifndef __DS18B20_H
//#define __DS18B20_H 
//#include "sys.h"   

////IO方向设置
//#define DS18B20_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}  
//#define DS18B20_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}
//////IO操作函数											   
//#define	DS18B20_DQ_OUT PBout(0) //数据端口	PB0 
//#define	DS18B20_DQ_IN  PBin(0)  //数据端口	PB0 
//   	
//u8 DS18B20_Init(void);//初始化DS18B20
//short DS18B20_Get_Temp(void);//获取温度
//void DS18B20_Start(void);//开始温度转换
//void DS18B20_Write_Byte(u8 dat);//写入一个字节
//u8 DS18B20_Read_Byte(void);//读出一个字节
//u8 DS18B20_Read_Bit(void);//读出一个位
//u8 DS18B20_Check(void);//检测是否存在DS18B20
//void DS18B20_Rst(void);//复位DS18B20    
//#endif


#ifndef  __DS18B20_H
#define  __DS18B20_H

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
