# 智能鱼缸控制系统

## 概述

基于 STM32F103C8 + FreeRTOS + 机智云物联网平台的智能鱼缸控制系统。

## 核心特性

| 项目 | 规格 |
|------|------|
| 主控 | STM32F103C8 (72MHz, 64KB Flash, 20KB RAM) |
| 系统 | FreeRTOS V202212.00 |
| 云平台 | 机智云 (WiFi配网) |
| 开发环境 | Keil MDK 5.06 |

## 主要功能

- **环境监测**: 水温、水位、光照、浊度
- **自动控制**: 加热、补水、排水、增氧、照明
- **远程控制**: 手机APP远程监控
- **本地显示**: OLED屏幕
- **按键操作**: 模式切换、参数设置
- **数据存储**: Flash存储阈值
- **喂食控制**: 舵机喂食
- **报警**: 水位异常蜂鸣器

## 系统架构

```
用户手机APP → 机智云 → WiFi模块(ESP-01S) → STM32F103C8
                                              ↓
                              FreeRTOS调度器
                              ├─ Sensor_task (传感器采集)
                              ├─ Key_task (按键处理)
                              ├─ DevCntrl_task (控制逻辑)
                              ├─ Display_task (OLED显示)
                              └─ Gizwits_task (云平台通信)
```

## 目录结构

```
Stm32_Fish_System/
├── User/                    # 主程序
│   ├── main.c
│   └── FreeRTOSConfig.h
├── HardWare/               # 硬件驱动
│   ├── ADC/
│   ├── DS18B20/
│   ├── OLED/
│   ├── Key/
│   ├── Pump/
│   ├── PTC/
│   └── LED/
├── System/                 # 系统层
│   ├── FlashStorage/
│   ├── USART/
│   └── USART2/
├── Gizwits/               # 机智云协议
└── FreeRTOS/              # RTOS内核
```

## 硬件配置

### 引脚分配

| 功能 | GPIO | 说明 |
|------|------|------|
| OLED | PB8,PB9 | I2C |
| DS18B20 | PB0 | 温度传感器 |
| ADC | PA5,PA6,PA7 | 浊度/水位/光照 |
| 按键 | PC12-15 | 4个按键 |
| 舵机PWM | PA0 | TIM2_CH1 |
| 补光灯PWM | PB7 | TIM4_CH2 |
| 蜂鸣器 | PA4 | 报警输出 |
| PTC加热 | PB1 | 加热控制 |
| 补水/排水泵 | PB10,PB11 | 水泵控制 |
| WiFi | USART2 | ESP-01S |

### 传感器

| 传感器 | 型号 | 接口 | 量程 |
|--------|------|------|------|
| 温度 | DS18B20 | 单总线 | -55~125°C |
| 浊度 | TS-300D | 模拟 | 0-4000 NTU |
| 水位 | 电容式 | 模拟 | 0-100% |
| 光照 | GL5528 | 模拟 | 0-100% |

## 编译

使用 Keil MDK 5.06 打开 `Project.uvprojx` 编译。
