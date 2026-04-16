#ifndef  __THRESHOLD_CONFIG_H
#define  __THRESHOLD_CONFIG_H

#include <stdint.h>
#include <string.h>
#include "FlashStorage.h"
#include "main_freertos.h"

// 类型
typedef enum {
    THRESHOLD_WATER_LOW = 0,
    THRESHOLD_WATER_HIGH,
    THRESHOLD_TEMP_LOW,
    THRESHOLD_TEMP_HIGH,
    THRESHOLD_LIGHT,
    THRESHOLD_TURBIDITY,
    THRESHOLD_COUNT
} ThresholdType_t;

// 范围
typedef struct {
    uint8_t min_value;
    uint8_t max_value;
    uint8_t step;
} ThresholdRange_t;

// 配置
typedef struct {
    uint8_t *value_ptr;
    ThresholdRange_t range;
} ThresholdConfig_t;

void Threshold_Init(void);
void Threshold_Adjust(uint8_t direction);
void Threshold_Save(void);
void Threshold_Select(ThresholdType_t index);

#endif
