#include "Threshold_config.h"

extern uint8_t Low_Level;
extern uint8_t High_Level;
extern uint8_t Low_Temp;
extern uint8_t High_Temp;
extern uint8_t Threshold_Light;
extern uint8_t Threshold_TS;

#define DEFAULT_WATER_LOW   20
#define DEFAULT_WATER_HIGH  80
#define DEFAULT_TEMP_LOW    26
#define DEFAULT_TEMP_HIGH   28
#define DEFAULT_LIGHT       70
#define DEFAULT_TS          60

#pragma pack(1)
typedef struct {
    uint8_t water_low;
    uint8_t water_high;
    uint8_t temp_low;
    uint8_t temp_high;
    uint8_t light;
    uint8_t turbidity;
    uint8_t crc;
} ThresholdStorage_t;
#pragma pack()

static ThresholdType_t currentThreshold = THRESHOLD_WATER_LOW;

static ThresholdConfig_t threshold_configs[THRESHOLD_COUNT] = 
{
    {&Low_Level, {0, 100, 1}},
    {&High_Level, {0, 100, 1}},
    {&Low_Temp, {0, 50,  1}},
    {&High_Temp, {0, 50,  1}},
    {&Threshold_Light, {0, 100, 1}},
    {&Threshold_TS, {0, 100, 1}} 
};

static void Threshold_Validate(void);

void Threshold_Init(void)
{
    ThresholdStorage_t storage;
    if (FlashStorage_Read((uint8_t*)&storage, sizeof(ThresholdStorage_t) - 1)) 
    {
        Low_Level = storage.water_low;
        High_Level = storage.water_high;
        Low_Temp = storage.temp_low;
        High_Temp = storage.temp_high;
        Threshold_Light = storage.light;
        Threshold_TS = storage.turbidity;
        Threshold_Validate();
    } 
    else 
    {
        Low_Level = DEFAULT_WATER_LOW;
        High_Level = DEFAULT_WATER_HIGH;
        Low_Temp = DEFAULT_TEMP_LOW;
        High_Temp = DEFAULT_TEMP_HIGH;
        Threshold_Light = DEFAULT_LIGHT;
        Threshold_TS = DEFAULT_TS;
        Threshold_Validate();
    }
}

void Threshold_Save(void)
{
    ThresholdStorage_t storage;
    storage.water_low = Low_Level;
    storage.water_high = High_Level;
    storage.temp_low = Low_Temp;
    storage.temp_high = High_Temp;
    storage.light = Threshold_Light;
    storage.turbidity = Threshold_TS;

    FlashStorage_Write((uint8_t *)&storage, sizeof(ThresholdStorage_t) - 1);
}

void Threshold_Select(ThresholdType_t index)
{
    if (index < THRESHOLD_COUNT)
    {
        currentThreshold = index;
    }
}

void Threshold_Adjust(uint8_t direction)
{
    ThresholdConfig_t *config = &threshold_configs[currentThreshold];
    uint8_t new_value = *config->value_ptr;

    if (direction == 1)
    {
        if (new_value <= config->range.max_value -config->range.step)
        {
            new_value += config->range.step;
        }
    }
    else if (direction == 0)
    {
        if (new_value >= config->range.min_value + config->range.step)
        {
            new_value -= config->range.step;
        }
    }
    else return;
    *config->value_ptr = new_value;
    Threshold_Validate();
}

static void Threshold_Validate(void)
{
    if (Low_Level >= High_Level) 
    {
        High_Level = Low_Level + 10;
        if (High_Level > 100) High_Level = 100;
    }
    if (Low_Temp >= High_Temp) 
    {
        High_Temp = Low_Temp + 2;
        if (High_Temp > 50) High_Temp = 50;
    }
}
