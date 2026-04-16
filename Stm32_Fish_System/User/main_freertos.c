#include "main_freertos.h"
#include "main.h"

extern dataPoint_t currentDataPoint;

TaskHandle_t sensor_task_handle;
TaskHandle_t key_task_handle;
TaskHandle_t devcontoral_task_handle;
TaskHandle_t display_task_handle;
TaskHandle_t gizwits_task_handle;

volatile uint16_t tsdata = 0;
volatile uint16_t level = 0;
volatile uint16_t light = 0;
volatile int16_t temp = 0;

volatile uint16_t ptc_count = 0;
volatile uint16_t pump_out_count = 0;
volatile uint16_t pump_in_count = 0;
volatile uint16_t servo_count = 0;

uint8_t High_Level = 80;
uint8_t Low_Level = 20;
uint8_t High_Temp = 28;
uint8_t Low_Temp = 26;
uint8_t Threshold_Light = 70;
uint8_t Threshold_TS = 60;


volatile uint8_t servo_state = 0;
volatile uint8_t pump_out_state = 0;
volatile uint8_t pump_in_state = 0;
volatile uint8_t ptc_state = 0;


// 按键触发标志
volatile uint8_t key_servo_flag = 0;
volatile uint8_t key_pump_in_flag = 0;
volatile uint8_t key_pump_out_flag = 0;
volatile uint8_t key_ptc_flag = 0;

volatile uint8_t key_airlink_flag = 0;
volatile uint8_t key_softap_flag = 0;
volatile uint8_t key_reset_flag = 0;

// 阈值设置模式下的备份（用于取消修改时恢复）
static uint8_t backup_Low_Level;
static uint8_t backup_High_Level;
static uint8_t backup_Low_Temp;
static uint8_t backup_High_Temp;
static uint8_t backup_Threshold_Light;
static uint8_t backup_Threshold_TS;

// ESP当前模式 (0=未设置, 1=AirLink, 2=SoftAP, 3=Rest)
uint8_t current_esp_mode = 0;

SystemMode_t currentMode = MODE_AUTO;
MenuState_t menuState = MENU_NONE;
uint8_t selectModeIndex = 0;
uint8_t selectThresholdIndex = 0;

void freertos_start(void)
{
    Threshold_Init();

    xTaskCreate((TaskFunction_t) SensorDataGet_task,
                (char *) "SensorDataGet",
                (configSTACK_DEPTH_TYPE)SENSOR_TASK_STACK,
                (void *) NULL,
                (UBaseType_t) SENSOR_TASK_PRIORITY,
                (TaskHandle_t *) &sensor_task_handle);

    xTaskCreate((TaskFunction_t) Key_task,
                (char *) "Key_task",
                (configSTACK_DEPTH_TYPE)KEY_TASK_STACK,
                (void *) NULL,
                (UBaseType_t) KEY_TASK_PRIORITY,
                (TaskHandle_t *) &key_task_handle);

    xTaskCreate((TaskFunction_t) DevContoral_task,
                (char *) "DevContoral",
                (configSTACK_DEPTH_TYPE)DEVCONTORAL_TASK_STACK,
                (void *) NULL,
                (UBaseType_t) DEVCONTORAL_TASK_PRIORITY,
                (TaskHandle_t *) &devcontoral_task_handle);

    xTaskCreate((TaskFunction_t) Display_task,
                (char *) "Display",
                (configSTACK_DEPTH_TYPE)DISPLAY_TASK_STACK,
                (void *) NULL,
                (UBaseType_t) DISPLAY_TASK_PRIORITY,
                (TaskHandle_t *) &display_task_handle);

    xTaskCreate((TaskFunction_t) Gizwits_task,
                (char *) "Gizwits",
                (configSTACK_DEPTH_TYPE)GIZWITS_TASK_STACK,
                (void *) NULL,
                (UBaseType_t) GIZWITS_TASK_PRIORITY,
                (TaskHandle_t *) &gizwits_task_handle);

    vTaskStartScheduler();
}

void SensorDataGet_task(void *pvParameters)
{
    while(1)
    {
        tsdata = TS_GetData();
        level = Water_GetLevel();
        light = Photosensitive_GetValue();
        
        taskENTER_CRITICAL(); // 设计临界区
        DS18B20_Rst();
        if(!DS18B20_Check())
        {
            temp = DS18B20_Get_Temp();
        }
        taskEXIT_CRITICAL();
        
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Key_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100)); // 稳定启动

    while (1)
    {
        uint8_t key = Key_GetNum(); 

        if (key != 0)
        {
            if (key == KEY_LONG_PC12)
            {
                if (menuState == MENU_NONE)
                {
                    menuState = MENU_SELECT;
                    selectModeIndex = (uint8_t)currentMode;
                    current_esp_mode = 0;
                }
                else
                {
                    current_esp_mode = 0;
                }
            }
            else if (key == KEY_SHORT_PC12)
            {
                if (menuState == MENU_NONE) // 只有不在菜单时才处理设备控制
                {
                    if (currentMode == MODE_MANUAL)
                    {
                        key_pump_out_flag = 1;
                    }
                    else if (currentMode == MODE_REMOTE)
                    {
                        key_airlink_flag = 1;
                    }
                }
                else
                {
                    current_esp_mode = 0;
                }
            }

            else if (key == KEY_SHORT_PC13)
            {
                if (menuState == MENU_SELECT)
                {
                    if (selectModeIndex == 0) selectModeIndex = 3; // 修正减法溢出
                    selectModeIndex --;
                }
                if (currentMode == MODE_THRESHOLD_SET)
                {
                    Threshold_Adjust(0);
                }
                else if (currentMode == MODE_REMOTE)
                {
                    key_softap_flag = 1;
                }
                else if (menuState == MENU_NONE) // 只有不在菜单时才处理设备控制
                {
                    key_pump_in_flag = 1;
                }
            }

            else if (key == KEY_LONG_PC13)
            {
                if (menuState == MENU_NONE && currentMode == MODE_AUTO)
                {

                    // 进入设置模式前，备份当前阈值
                    backup_Low_Level = Low_Level;
                    backup_High_Level = High_Level;
                    backup_Low_Temp = Low_Temp;
                    backup_High_Temp = High_Temp;
                    backup_Threshold_Light = Threshold_Light;
                    backup_Threshold_TS = Threshold_TS;

                    currentMode = MODE_THRESHOLD_SET;
                    selectThresholdIndex = 0;
                    Threshold_Select((ThresholdType_t)selectThresholdIndex);
                }
                if (currentMode == MODE_THRESHOLD_SET)
                {
                    if (selectThresholdIndex == 0)
                    {
                        selectThresholdIndex = THRESHOLD_COUNT - 1;
                    }
                    else
                    {
                        selectThresholdIndex --;
                        Threshold_Select((ThresholdType_t)selectThresholdIndex);
                    }
                }
            }

            else if (key == KEY_SHORT_PC14)
            {
                if (menuState == MENU_SELECT)
                {
                    selectModeIndex ++;
                    if (selectModeIndex > 2) selectModeIndex = 0;
                }
                else if (currentMode == MODE_THRESHOLD_SET)
                {
                   Threshold_Adjust(1);
                }
                else if (menuState == MENU_NONE) // 只有不在菜单时才处理设备控制
                {
                    key_ptc_flag = 1;
                }
            }
            else if (key == KEY_LONG_PC14)
            {
                if (currentMode == MODE_REMOTE)
                {
                    key_reset_flag = 1;
                }
                if (currentMode == MODE_THRESHOLD_SET)
                {
                    selectThresholdIndex ++;
                    if (selectThresholdIndex >= THRESHOLD_COUNT)
                    {
                        selectThresholdIndex = 0;
                    }
                    Threshold_Select((ThresholdType_t)selectThresholdIndex);
                }
            }
            else if (key == KEY_SHORT_PC15)
            {
                if (menuState == MENU_SELECT)
                {
                    currentMode = (SystemMode_t)selectModeIndex;
                    menuState = MENU_NONE;
                }
                else if (currentMode == MODE_REMOTE)
                {
                    key_airlink_flag = 0;
                    key_softap_flag = 0;
                    key_reset_flag = 0;
                    current_esp_mode = 0; // 退出远程模式时，重置ESP模式为“None”
                    menuState = MENU_SELECT;
                    selectModeIndex = (uint8_t)currentMode;
                }
                else if (currentMode == MODE_THRESHOLD_SET)
                {
                    // 取消修改，恢复备份的阈值
                    Low_Level       = backup_Low_Level;
                    High_Level      = backup_High_Level;
                    Low_Temp        = backup_Low_Temp;
                    High_Temp       = backup_High_Temp;
                    Threshold_Light = backup_Threshold_Light;
                    Threshold_TS    = backup_Threshold_TS;
                    currentMode = MODE_AUTO;
                    menuState = MENU_NONE;
                }
                else if (menuState == MENU_NONE) // 只有不在菜单时才处理设备控制
                {
                    key_servo_flag = 1;
                }
            }
            else if (key == KEY_LONG_PC15)
            {
                if (currentMode == MODE_THRESHOLD_SET)
                {
                    Threshold_Save();
                    currentMode = MODE_AUTO;
                    menuState = MENU_NONE;
                }
            }
        }
    
        vTaskDelay(pdMS_TO_TICKS(20)); // 扫描频率
    }
}

void DevContoral_task(void *pvParameters)
{
    while(1)
    {
        switch (currentMode)
        {
            case MODE_MANUAL:

                key_airlink_flag = 0;
                key_softap_flag = 0;
                key_reset_flag = 0;

                // 处理排水泵按键
                if (key_pump_out_flag == 1)
                {
                    pump_out_count++;
                    if (pump_out_count % 2 == 1)
                    {
                        if (pump_in_state == 1) // 互斥保护：如果要开排水泵，先确保补水泵是关的
                        {
                            Pump_IN_OFF();
                            pump_in_state = 0;
                            pump_in_count = 0;
                        }
                        Pump_OUT_ON(); // 开启排水泵
                        pump_out_state = 1;
                    }
                    else
                    {
                        Pump_OUT_OFF(); // 关闭排水泵
                        pump_out_state = 0;
                    }
                    key_pump_out_flag = 0;
                }

                // 处理补水泵按键
                if (key_pump_in_flag == 1)
                {
                    pump_in_count++;
                    if (pump_in_count % 2 == 1)
                    {
                        if (pump_out_state == 1) // 互斥保护：如果要开补水泵，先确保排水泵是关的
                        {
                            Pump_OUT_OFF();
                            pump_out_state = 0;
                            pump_out_count = 0;
                        }
                        Pump_IN_ON(); // 开启补水泵
                        pump_in_state = 1;
                    }
                    else
                    {
                        Pump_IN_OFF(); // 关闭补水泵
                        pump_in_state = 0;
                    }
                    key_pump_in_flag = 0;
                }

                // 处理加热片模块
                if(key_ptc_flag == 1)
                {
                    ptc_count++;
                    if (ptc_count % 2 == 1)
                    {
                        PTC_ON();
                        ptc_state = 1;
                    }
                    else
                    {
                        PTC_OFF();
                        ptc_state = 0;
                    }
                    key_ptc_flag = 0;
                }

                // 处理舵机按键
                if (key_servo_flag == 1)
                {
                    servo_count++;
                    if (servo_count % 2 == 1)
                    {
                        Servo_SetAngle(60);
                        servo_state = 1;
                    }
                    else
                    {
                        Servo_SetAngle(0);
                        servo_state = 0;
                    }
                    key_servo_flag = 0;
                }
                break;
        
            case MODE_AUTO:

                key_servo_flag = 0;
                key_ptc_flag = 0;
                key_pump_in_flag = 0;
                key_pump_out_flag = 0;

                key_airlink_flag = 0;
                key_softap_flag = 0;
                key_reset_flag = 0;

                if (level < Low_Level)
                {
                    if (!pump_in_state)
                    {
                        if (pump_out_state == 1) // 互斥保护
                        {
                            Pump_OUT_OFF();
                            pump_out_state = 0;
                        }
                        Pump_IN_ON(); // 自动模式下，如果水位低于20，开启补水泵
                        pump_in_state = 1;
                    }
                }
                else if (level >= High_Level - 10)
                {
                    if (pump_in_state)
                    {
                        Pump_IN_OFF(); // 达到高水位，关闭补水泵
                        pump_in_state = 0;
                    }
                }

                if (temp < Low_Temp * 10)   // 温度低于 26.0℃ 时打开加热片
                {
                    if (!ptc_state)
                    {
                        PTC_ON();
                        ptc_state = 1;
                    }
                }
                if (temp > High_Temp * 10)              // 温度 >= 26.0℃ 时关闭加热片
                {
                    if (ptc_state)
                    {
                        PTC_OFF();
                        ptc_state = 0;
                    }
                }
                if ((100 - light) < Threshold_Light)
                {
                    LED_SetBrightness(light);
                }
                else
                {
                    LED_OFF();
                }
                if (tsdata > Threshold_TS)
                {
                    Servo_SetAngle(120);
                    servo_state = 1;
                }
                else
                {
                    Servo_SetAngle(0);
                    servo_state = 0;
                }
                break;

            case MODE_REMOTE:

                if (pump_in_state) Pump_IN_OFF();
                if (pump_out_state) Pump_OUT_OFF();
                if (ptc_state) PTC_OFF();
                if (servo_state) Servo_SetAngle(0);
				LED_OFF();
                
                pump_in_state = pump_out_state = ptc_state = servo_state = 0;
                pump_in_count = pump_out_count = ptc_count = servo_count = 0;
                
                key_servo_flag = 0;
                key_pump_in_flag = 0;
                key_pump_out_flag = 0;
                key_ptc_flag = 0;

                if (key_airlink_flag == 1)
                {
                    gizwitsSetMode(WIFI_AIRLINK_MODE);
                    current_esp_mode = 1;
                    key_airlink_flag = 0;
                }
                
                if (key_softap_flag == 1)
                {
                    gizwitsSetMode(WIFI_SOFTAP_MODE);
                    current_esp_mode = 2;
                    key_softap_flag = 0;
                }

                if (key_reset_flag == 1)
                {
                    gizwitsSetMode(WIFI_RESET_MODE);
                    current_esp_mode = 3;
                    key_reset_flag = 0;
                }
                break;

            case MODE_THRESHOLD_SET:
            
                break;
			}

        // 水位报警
        if (level >= High_Level || level <= Low_Level)
        {
            Buzzer_Turn();
        }
        else
        {
            Buzzer_OFF();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Display_task(void *pvParameters)
{
    static MenuState_t lastMenuState = (MenuState_t)-1;
    static SystemMode_t lastMode = (SystemMode_t)-1;
    uint8_t full_refresh = 0;

    while(1)
    {
       // 检测菜单状态或模式是否改变，若改变则触发全屏刷新
       if (menuState != lastMenuState || (menuState == MENU_NONE && currentMode != lastMode))
       {
           full_refresh = 1;
           lastMenuState = menuState;
           lastMode = currentMode;
       }
       else
       {
           full_refresh = 0;
       }

       if (menuState == MENU_NONE)
       {
            switch (currentMode)
            {
                case MODE_MANUAL:
                    Display_Manual(full_refresh);
                    break;
        
                case MODE_AUTO:
                    Display_Auto(full_refresh);
                    break;
                
                case MODE_REMOTE:
                    Display_Remote(full_refresh);
                    break;

                case MODE_THRESHOLD_SET:
                    Display_Threshold_Set(full_refresh);
                    break;
            }
       }
       else 
       {
           Display_Menu(full_refresh);
       }
       
       vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Gizwits_task(void *pvParameters)
{
    while(1)
    {
        userHandle();
        gizwitsHandle(&currentDataPoint);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
