#include "Display.h"

void Display_Menu(uint8_t full_refresh)
{
    if (full_refresh)
    {
        OLED_Clear();
        OLED_ShowChinese(1, 1, 8);
        OLED_ShowChinese(1, 2, 9);
        OLED_ShowString(1, 5, ":");

        char* modes[] = {"Manual", "Auto", "Remote"};
        for (int8_t i = 0; i < 3; i++)
        {
            OLED_ShowString(2 + i, 3, modes[i]);
        }
    }

    for (int8_t i = 0; i < 3; i++)
    {
        if (i == selectModeIndex)
        {
            OLED_ShowString(2 + i, 1, ">");
        }
        else
        {
            OLED_ShowString(2 + i, 1, " ");
        }
    }
}

void Display_Manual(uint8_t full_refresh)
{
    if (full_refresh)
    {
        OLED_Clear();

        OLED_ShowChinese(1, 7, 18);
        OLED_ShowChinese(1, 8, 19);

        // 排水泵标签
        OLED_ShowChinese(1, 1, 80);
        OLED_ShowChinese(1, 2, 81);
        OLED_ShowString(1, 5, ":");

        // 补水泵标签
        OLED_ShowChinese(2, 1, 82);
        OLED_ShowChinese(2, 2, 83);
        OLED_ShowString(2, 5, ":");

        // 加热片标签
        OLED_ShowChinese(3, 1, 86);
        OLED_ShowChinese(3, 2, 87);
        OLED_ShowString(3, 5, ":");

        // 喂食标签
        OLED_ShowChinese(4, 1, 78);
        OLED_ShowChinese(4, 2, 79);
        OLED_ShowString(4, 5, ":");
    }

    // 更新状态数值
    OLED_ShowString(1, 6, pump_state ? "ON " : "OFF");
    OLED_ShowString(2, 6, "OFF"); // 补水泵暂无状态变量，先保持OFF
    OLED_ShowString(3, 6, ptc_state ? "ON " : "OFF");
    OLED_ShowString(4, 6, servo_state ? "ON " : "OFF");
}

void Display_Auto(uint8_t full_refresh)
{
    if (full_refresh)
    {

        OLED_Clear();

        OLED_ShowChinese(1, 7, 16);
        OLED_ShowChinese(1, 8, 17);

        // 水位标签
        OLED_ShowChinese(1, 1, 39);
        OLED_ShowChinese(1, 2, 40);
        OLED_ShowString(1, 5, ":");
        OLED_ShowString(1, 8, "%");

        // 浊度标签
        OLED_ShowChinese(2, 1, 37);
        OLED_ShowChinese(2, 2, 38);
        OLED_ShowString(2, 5, ":");

        // 光照标签
        OLED_ShowChinese(3, 1, 4);
        OLED_ShowChinese(3, 2, 5);
        OLED_ShowString(3, 5, ":");
        OLED_ShowString(3, 8, "%");

        // 温度标签
        OLED_ShowChinese(4, 1, 26);
        OLED_ShowChinese(4, 2, 28);
        OLED_ShowString(4, 5, ":");
    }

    // 水位数值
    OLED_ShowNum(1, 6, level, 2);

    // 浊度数值
    OLED_ShowNum(2, 6, tsdata, 4);

    // 光照数值
    OLED_ShowNum(3, 6, (100 - light), 2);

    // 温度数值
    int16_t temp_val = temp;
    if (temp_val < 0)
    {
        OLED_ShowChar(4, 6, '-');
        temp_val = -temp_val;
    }
    else
    {
        OLED_ShowChar(4, 6, '+');
    }
    OLED_ShowNum(4, 7, temp_val / 10, 2);
    OLED_ShowChar(4, 9, '.');
    OLED_ShowNum(4, 10, temp_val % 10, 1);
}

void Display_Remote(uint8_t full_refresh)
{
    if (full_refresh)
    {
        OLED_Clear();

        OLED_ShowChinese(1, 7, 20);
        OLED_ShowChinese(1, 8, 21);
        
        // 显示当前ESP模式标签
        OLED_ShowString(1, 1, "Mode:");
    }
    
    // 动态显示模式
    if (current_esp_mode == 1) 
    {
        OLED_ShowString(2, 2, "AirLink  ");
    } 
    else if (current_esp_mode == 2) 
    {
        OLED_ShowString(2, 2, "SoftAP   ");
    } 
    else if (current_esp_mode == 3) 
    {
        OLED_ShowString(2, 2, "Resetting");
    } 
    else 
    {
        OLED_ShowString(2, 2, "None     ");
    }
}
