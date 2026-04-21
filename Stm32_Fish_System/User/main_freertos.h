#ifndef  __MAIN_FREERTOS_H
#define  __MAIN_FREERTOS_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FlashStorage.h"
#include "Threshold_Config.h"

#define KEY_SHORT_PB12   1
#define KEY_SHORT_PB13   2
#define KEY_SHORT_PB14   3
#define KEY_SHORT_PB15   4

#define KEY_LONG_PB12    11
#define KEY_LONG_PB13    12
#define KEY_LONG_PB14    13
#define KEY_LONG_PB15    14

#define SENSOR_TASK_STACK            256
#define SENSOR_TASK_PRIORITY         4
extern TaskHandle_t sensor_task_handle;

#define KEY_TASK_STACK               128
#define KEY_TASK_PRIORITY            2
extern TaskHandle_t key_task_handle;

#define DEVCONTORAL_TASK_STACK       256
#define DEVCONTORAL_TASK_PRIORITY    3
extern TaskHandle_t devcontoral_task_handle;

#define DISPLAY_TASK_STACK           512
#define DISPLAY_TASK_PRIORITY        2
extern TaskHandle_t display_task_handle;

#define GIZWITS_TASK_STACK           256
#define GIZWITS_TASK_PRIORITY        1
extern TaskHandle_t gizwits_task_handle;

// 模式选择
typedef enum {
    MODE_MANUAL = 0,
    MODE_AUTO,
    MODE_REMOTE,
    MODE_THRESHOLD_SET
} SystemMode_t;

// 菜单状态
typedef enum{
    MENU_NONE = 0,
    MENU_SELECT
} MenuState_t;

extern SystemMode_t currentMode;
extern MenuState_t menuState;
extern uint8_t selectModeIndex;
extern uint8_t selectThresholdIndex;
extern uint8_t current_esp_mode;

extern volatile uint32_t threshold_save_tick; 

extern volatile uint16_t tsdata;
extern volatile uint16_t level;
extern volatile uint16_t light;
extern volatile int16_t temp;

void freertos_start(void);
void SensorDataGet_task(void *pvParameters);
void Key_task(void *pvParameters);
void DevContoral_task(void *pvParameters);
void Display_task(void *pvParameters);
void Gizwits_task(void *pvParameters);

#endif
