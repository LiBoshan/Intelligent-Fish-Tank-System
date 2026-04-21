#ifndef  __DISPLAY_H
#define  __DISPLAY_H

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Threshold_Config.h"
#include "main_freertos.h"
#include "main.h"

extern uint8_t selectModeIndex;
extern uint8_t selectThresholdIndex;

extern uint8_t High_Level;
extern uint8_t Low_Level;
extern uint8_t High_Temp;
extern uint8_t Low_Temp;
extern uint8_t Threshold_Light;
extern uint8_t Threshold_TS;

extern volatile uint16_t tsdata;
extern volatile uint16_t level;
extern volatile uint16_t light;
extern volatile int16_t temp;

extern volatile uint32_t threshold_save_tick;

extern volatile uint8_t pump_out_state;
extern volatile uint8_t pump_in_state;
extern volatile uint8_t ptc_state;
extern volatile uint8_t servo_state;

extern uint8_t current_esp_mode;

void Display_Menu(uint8_t full_refresh);
void Display_Manual(uint8_t full_refresh);
void Display_Auto(uint8_t full_refresh);
void Display_Remote(uint8_t full_refresh);
void Display_Threshold_Set(uint8_t full_refresh);

#endif
