#ifndef __OLED_H
#define __OLED_H

// 宏定义OLED使用的GPIO端口和引脚
#define OLED_I2C_GPIO_PORT    GPIOB
#define OLED_I2C_SCL_PIN      GPIO_Pin_8
#define OLED_I2C_SDA_PIN      GPIO_Pin_9

// 引脚配置宏定义
#define OLED_W_SCL(x)         GPIO_WriteBit(OLED_I2C_GPIO_PORT, OLED_I2C_SCL_PIN, (BitAction)(x))
#define OLED_W_SDA(x)         GPIO_WriteBit(OLED_I2C_GPIO_PORT, OLED_I2C_SDA_PIN, (BitAction)(x))

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t num);    //显示汉字
void OLED_BMP(int i);           //显示图片

#endif
