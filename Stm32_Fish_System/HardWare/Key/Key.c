#include "Key.h"

typedef struct {
    uint8_t last_state;        // 上一次电平状态 (1=释放, 0=按下)
    uint32_t press_start_time; // 电平变化时的时间戳
    uint32_t last_return_time;
    uint8_t short_triggered;
    uint8_t long_triggered;
    uint8_t key_num;          // 该按键对应的键值（未使用，保留）
} Key_HandleTypeDef;

static Key_HandleTypeDef keys[4] = {
    {1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0},
};

static const uint16_t key_pins[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
static const uint8_t key_num_map[4] = {1, 2, 3, 4};

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLOCK, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

static uint8_t Key_ScanSingle(uint8_t index)
{
    uint32_t current_time = gizGetTimerCount();
    uint8_t current_state = GPIO_ReadInputDataBit(KEY_GPIO_PORT, key_pins[index]);
    uint8_t short_key = key_num_map[index];
    uint8_t long_key = short_key + 10;

    // 检测状态变化
    if (current_state != keys[index].last_state) 
    {
        if (current_state == 0) // 刚按下 (1 -> 0)
        {
            keys[index].press_start_time = current_time;
            keys[index].long_triggered = 0;
            keys[index].short_triggered = 0;
        }
        else // 刚抬起 (0 -> 1)
        {
            // 如果抬起时还没触发过长按，且按下时间符合短按范围
            if (keys[index].long_triggered == 0)
            {
                uint32_t duration = current_time - keys[index].press_start_time;
                if (duration >= KEY_DEBOUNCE_TIME && duration < KEY_LONG_PRESS_TIME)
                {
                    keys[index].last_state = current_state;
                    return short_key; // 抬起瞬间返回短按
                }
            }
        }
        keys[index].last_state = current_state;
    }

    // 长按检测（在按住不放的过程中）
    if (current_state == 0 && keys[index].long_triggered == 0)
    {
        uint32_t duration = current_time - keys[index].press_start_time;
        if (duration >= KEY_LONG_PRESS_TIME)
        {
            keys[index].long_triggered = 1;
            return long_key; // 达到时间瞬间返回长按
        }
    }
    
    return 0; 
}

uint8_t Key_GetNum(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++) {
        uint8_t result = Key_ScanSingle(i);
        if (result != 0) return result;
    }
    return 0;
}

