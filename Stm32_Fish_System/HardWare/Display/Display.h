#ifndef  __DISPLAY_H
#define  __DISPLAY_H

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "main_freertos.h"
#include "main.h"

extern uint8_t selectModeIndex;
extern volatile uint16_t tsdata;
extern volatile uint16_t level;
extern volatile uint16_t light;
extern volatile int16_t temp;
extern volatile uint8_t pump_state;
extern volatile uint8_t ptc_state;
extern volatile uint8_t servo_state;
extern uint8_t current_esp_mode;

void Display_Menu(uint8_t full_refresh);
void Display_Manual(uint8_t full_refresh);
void Display_Auto(uint8_t full_refresh);
void Display_Remote(uint8_t full_refresh);

#endif
