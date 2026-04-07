#ifndef __USART_H
#define __USART_H
 
#include "stm32f10x.h"                  // Device header
#include <stdarg.h>
#include <stdio.h>

void USART1_Config(void);
int fputc(int ch, FILE *f);

#endif
