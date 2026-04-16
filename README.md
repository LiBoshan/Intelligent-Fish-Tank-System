# 智能鱼缸控制系统 - 项目技术文档

## 目录

1. [项目概述](#1-项目概述)
2. [系统架构](#2-系统架构)
3. [硬件设计](#3-硬件设计)
4. [软件设计](#4-软件设计)
5. [功能模块详解](#5-功能模块详解)
6. [云平台接入](#6-云平台接入)
7. [阈值配置与存储](#7-阈值配置与存储)
8. [状态机设计](#8-状态机设计)
9. [中断与定时器](#9-中断与定时器)
10. [编译与调试](#10-编译与调试)
11. [开发注意事项](#11-开发注意事项)
12. [扩展指南](#12-扩展指南)
13. [常见问题排查](#13-常见问题排查)

---

## 1. 项目概述

### 1.1 项目简介

本项目是一款基于 STM32F103C8 单片机的智能鱼缸控制系统，集成了 FreeRTOS 实时操作系统和机智云物联网平台，实现对鱼缸环境的全面感知与智能控制。

系统采用模块化设计，将各个功能划分为独立的硬件驱动和软件任务，便于维护和扩展。FreeRTOS 的引入确保了多任务的实时性和可靠性，机智云平台则提供了远程监控和控制的能力。

### 1.2 核心特性

| 特性 | 说明 |
|------|------|
| **主控芯片** | STM32F103C8 (Cortex-M3, 72MHz) |
| **操作系统** | FreeRTOS V202212.00 |
| **云平台** | 机智云 (支持 AirLink/SoftAP 配网) |
| **开发环境** | Keil MDK 5.06 |
| **晶振频率** | 8MHz 外部晶振 |
| **Flash大小** | 64KB |
| **RAM大小** | 20KB |

### 1.3 主要功能

- **环境监测**: 实时采集水温、水位、光照强度、水浊度
- **自动控制**: 根据阈值自动控制加热、补水、排水、增氧、照明
- **远程控制**: 通过手机 APP 远程查看状态和控制设备
- **本地显示**: OLED 屏幕显示实时数据和设备状态
- **本地操作**: 按键支持手动控制模式切换和参数设置
- **数据存储**: Flash 存储阈值配置，掉电不丢失
- **喂食控制**: 舵机控制喂食器开关
- **报警功能**: 水位异常时蜂鸣器报警

### 1.4 系统框图

```
                            ┌─────────────────┐
                            │   用户手机 APP   │
                            └────────┬────────┘
                                     │ 4G/WiFi
                            ┌────────▼────────┐
                            │   机智云平台     │
                            └────────┬────────┘
                                     │ USART2
┌────────────────────────────────────┴────────────────────────────────────┐
│                         STM32F103C8T6 MCU                               │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │                         FreeRTOS 调度器                            │  │
│  ├──────────┬──────────┬──────────┬──────────┬───────────────────────┤  │
│  │ Sensor   │   Key    │ DevCntrl │ Display  │ Gizwits               │  │
│  │ task     │ task     │ task     │ task     │ task                  │  │
│  │ (Pri:4)  │ (Pri:2)  │ (Pri:3)  │ (Pri:2)  │ (Pri:1)              │  │
│  └────┬─────┴────┬─────┴────┬─────┴────┬─────┴────────┬────────────┘  │
│       │          │          │          │              │                │
│  ┌────▼──────────▼──────────▼──────────▼──────────────▼────────────┐   │
│  │                      硬件抽象层 (HAL)                            │   │
│  ├─────────┬─────────┬─────────┬─────────┬─────────┬───────────────┤   │
│  │ ADC DMA  │ GPIO    │ TIM1/2/3│ USART1/│ I2C     │ Flash        │   │
│  │         │         │    /4   │ 2      │ (软件)  │              │   │
│  └────┬────┴────┬────┴────┬────┴────┬────┴────┬────┴────────┬────┘   │
└───────┼─────────┼─────────┼─────────┼─────────┼─────────────┼──────────┘
        │         │         │         │         │             │
┌───────▼─────────▼─────────▼─────────▼─────────▼─────────────▼──────────┐
│                           外部设备层                                    │
├──────────┬──────────┬──────────┬──────────┬──────────┬────────────────┤
│ DS18B20  │ OLED     │ ESP-01S  │ 按键     │ 执行器   │ 传感器         │
│ 温度     │ 显示屏    │ WiFi模块 │ 矩阵     │ 水泵/加热│ 水位/浊度/光照 │
└──────────┴──────────┴──────────┴──────────┴──────────┴────────────────┘
```

---

## 2. 系统架构

### 2.1 分层架构

系统采用经典的三层架构设计：

#### 2.1.1 硬件层 (Hardware Layer)
直接与物理硬件交互，包含所有外设的寄存器配置和底层驱动。

#### 2.1.2 驱动层 (Driver Layer)
封装硬件操作为统一的接口函数，为上层提供抽象的硬件访问能力。

#### 2.1.3 业务逻辑层 (Business Logic Layer)
实现具体的业务功能，如传感器数据采集、设备控制逻辑、阈值管理等。

#### 2.1.4 用户层 (User Layer)
处理用户交互，包括 OLED 显示、按键输入、云平台通信等。

### 2.2 项目目录结构

```
Intelligent-Fish-Tank-System/
├── README.md                          # 项目说明
│
└── Stm32_Fish_System/                 # STM32 主工程目录
    │
    ├── Project.uvprojx                # Keil 工程文件
    ├── Project.uvoptx                  # Keil 工程配置
    │
    ├── Start/                          # 启动文件目录
    │   ├── startup_stm32f10x_md.s      # 启动汇编代码 (Flash 启动)
    │   ├── system_stm32f10x.c/h        # 系统时钟配置
    │   ├── stm32f10x.h                 # 外设寄存器映射
    │   ├── core_cm3.c/h                # Cortex-M3 核心外设
    │   └── memory.scf                  # 散列文件 (可选)
    │
    ├── Library/                        # STM32 标准外设库 (SPL)
    │   ├── stm32f10x_adc.c/h           # ADC 模数转换
    │   ├── stm32f10x_gpio.c/h          # GPIO 通用输入输出
    │   ├── stm32f10x_rcc.c/h           # 复位和时钟控制
    │   ├── stm32f10x_tim.c/h           # 定时器驱动
    │   ├── stm32f10x_usart.c/h         # 串口通信
    │   ├── stm32f10x_flash.c/h         # Flash 编程
    │   ├── stm32f10x_dma.c/h           # DMA 控制器
    │   └── misc.c/h                     # 混杂配置 (NVIC, AFIO)
    │
    ├── User/                           # 用户主程序
    │   ├── main.c                       # 程序入口
    │   ├── main.h                       # 全局头文件
    │   ├── main_freertos.c              # FreeRTOS 任务定义
    │   ├── main_freertos.h              # 任务配置和变量声明
    │   ├── FreeRTOSConfig.h             # FreeRTOS 内核配置
    │   ├── stm32f10x_it.c/h             # 中断服务程序
    │   ├── stm32f10x_conf.h             # 库函数配置
    │   └── gizwits_product.c/h          # 机智云产品实现
    │
    ├── HardWare/                       # 硬件驱动模块
    │   ├── ADC/                         # 模数转换驱动
    │   │   ├── ADC.c
    │   │   └── ADC.h
    │   ├── Buzzer/                      # 蜂鸣器驱动
    │   │   ├── Buzzer.c
    │   │   └── Buzzer.h
    │   ├── Display/                     # 显示界面模块
    │   │   ├── Display.c
    │   │   └── Display.h
    │   ├── DS18B20/                     # 温度传感器驱动
    │   │   ├── DS18B20.c
    │   │   └── DS18B20.h
    │   ├── DUOJI/                       # 舵机和 PWM
    │   │   ├── PWM.c/h                  # PWM 初始化
    │   │   └── Servo.c/h                # 舵机控制
    │   ├── Key/                         # 按键驱动
    │   │   ├── Key.c
    │   │   └── Key.h
    │   ├── LED/                         # LED 补光灯驱动
    │   │   ├── LED.c
    │   │   └── LED.h
    │   ├── OLED/                       # OLED 显示屏驱动
    │   │   ├── OLED.c
    │   │   ├── OLED.h
    │   │   └── OLED_Font.h             # 字模数据
    │   ├── PTC/                         # PTC 加热片驱动
    │   │   ├── PTC.c
    │   │   └── PTC.h
    │   ├── Pump/                        # 水泵驱动
    │   │   ├── Pump.c
    │   │   └── Pump.h
    │   └── Threshold/                   # 阈值配置管理
    │       ├── Threshold_config.c
    │       └── Threshold_config.h
    │
    ├── System/                          # 系统层
    │   ├── FlashStorage/                # Flash 存储管理
    │   │   ├── FlashStorage.c
    │   │   └── FlashStorage.h
    │   ├── Timer/                       # 定时器管理
    │   │   ├── Timer.c
    │   │   └── Timer.h
    │   ├── USART/                       # 调试串口 (USART1)
    │   │   ├── USART.c
    │   │   └── USART.h
    │   └── USART2/                      # WiFi 串口 (USART2)
    │       ├── USART2.c
    │       └── USART2.h
    │
    ├── Gizwits/                         # 机智云协议
    │   ├── gizwits_protocol.c          # 协议层实现
    │   ├── gizwits_protocol.h          # 协议定义
    │   └── ...                          # 其他协议文件
    │
    ├── Utils/                           # 工具函数
    │   ├── common.c/h                   # 通用工具
    │   ├── dataPointTools.c/h           # 数据点工具
    │   └── ringbuffer.c/h               # 环形缓冲区
    │
    ├── FreeRTOS/                        # FreeRTOS 内核
    │   ├── include/                     # API 头文件
    │   ├── src/                         # 内核源码
    │   └── port/                        # 移植层
    │       ├── ARM_CM3/                  # Cortex-M3 移植
    │       │   ├── port.c
    │       │   └── portmacro.h
    │       └── MemMang/                  # 内存管理算法
    │
    ├── DebugConfig/                      # 调试配置
    ├── Objects/                          # 编译输出目录
    └── Listings/                         # 列表文件目录
```

### 2.3 时钟树

```
                              ┌─────────┐
                              │ HSE 8MHz│
                              └────┬────┘
                                   │
                              ┌────▼────┐
                              │  PLL    │
                              │ x1~x16  │──► System Clock (72MHz)
                              └────┬────┘
                                   │
        ┌──────────────────────────┼──────────────────────────┐
        │                          │                          │
   ┌────▼────┐              ┌──────▼──────┐            ┌──────▼──────┐
   │ APB1 Bus│              │  AHB Bus    │            │  APB2 Bus   │
   │  36MHz  │              │   72MHz    │            │   72MHz     │
   └────┬────┘              └──────┬──────┘            └──────┬──────┘
        │                         │                          │
   ┌────▼────┐              ┌──────▼──────┐            ┌──────▼──────┐
   │ TIM2    │              │    DMA1     │            │ TIM1       │
   │ TIM3    │              │    DMA2     │            │ TIM4       │
   │ TIM4    │              │   Flash     │            │ GPIOA/B/C  │
   │ USART2  │              │     ADC1    │            │ USART1     │
   │ SPI2    │              │              │            │ EXTI       │
   └─────────┘              └─────────────┘            └────────────┘
```

---

## 3. 硬件设计

### 3.1 MCU 引脚分配详解

STM32F103C8T6 是一款基于 ARM Cortex-M3 内核的微控制器，具有以下资源：

- **工作电压**: 2.0V ~ 3.6V
- **主频**: 最高 72MHz
- **Flash**: 64KB
- **SRAM**: 20KB
- **GPIO**: 37个（PA~PG）
- **通信接口**: USART, SPI, I2C, CAN
- **定时器**: 4个16位定时器 + 高级定时器
- **ADC**: 2个12位ADC，共10通道

#### 3.1.1 完整引脚分配表

| 功能模块 | GPIO | Pin | 配置模式 | 说明 |
|---------|------|-----|---------|------|
| **OLED_SCL** | PB8 | 30 | 复用推挽输出 (AF_PP) | I2C时钟线 |
| **OLED_SDA** | PB9 | 31 | 复用推挽输出 (AF_PP) | I2C数据线 |
| **DS18B20** | PB0 | 18 | 通用推挽输出/浮空输入 | 单总线数据线 |
| **ADC_TURBIDITY** | PA5 | 11 | 模拟输入 (AIN) | 浊度传感器 |
| **ADC_WATER_LEVEL** | PA6 | 12 | 模拟输入 (AIN) | 水位传感器 |
| **ADC_LIGHT** | PA7 | 13 | 模拟输入 (AIN) | 光敏传感器 |
| **KEY1** | PC12 | 34 | 浮空输入上拉 (IPU) | 模式切换 |
| **KEY2** | PC13 | 35 | 浮空输入上拉 (IPU) | 加/设置 |
| **KEY3** | PC14 | 36 | 浮空输入上拉 (IPU) | 减/确认 |
| **KEY4** | PC15 | 37 | 浮空输入上拉 (IPU) | 舵机/返回 |
| **SERVO_PWM** | PA0 | 10 | 复用推挽输出 (AF_PP) | TIM2_CH1, 舵机 |
| **LED_PWM** | PB7 | 29 | 复用推挽输出 (AF_PP) | TIM4_CH2, 补光灯 |
| **BUZZER** | PA4 | 8 | 通用推挽输出 (PP) | 有源蜂鸣器 |
| **PTC_HEATER** | PB1 | 3 | 通用推挽输出 (PP) | PTC加热片 |
| **PUMP_IN** | PB10 | 21 | 通用推挽输出 (PP) | 补水泵 |
| **PUMP_OUT** | PB11 | 22 | 通用推挽输出 (PP) | 排水泵 |
| **USART1_TX** | PA9 | 25 | 复用推挽输出 (AF_PP) | 调试串口 |
| **USART1_RX** | PA10 | 26 | 浮空输入 (IN_FLOATING) | 调试串口 |
| **USART2_TX** | PA2 | 29 | 复用推挽输出 (AF_PP) | WiFi模块 |
| **USART2_RX** | PA3 | 30 | 浮空输入 (IN_FLOATING) | WiFi模块 |

#### 3.1.2 引脚功能复用图

```
                    ┌─────────────────────────────────┐
                    │         STM32F103C8T6            │
                    │                                 │
    ┌───────────┐   │  ┌─────────────────────────────┐│
    │   OLED    │   │  │ PA0  ──► TIM2_CH1 (Servo)  ││
    │  (I2C)    │◄──┼──│ PA2  ──► USART2_TX (WiFi)  ││
    │           │   │  │ PA3  ──► USART2_RX         ││
    └───────────┘   │  │ PA4  ──► BUZZER             ││
                    │  │ PA5  ──► ADC_CH5 (Turbidity)││
    ┌───────────┐   │  │ PA6  ──► ADC_CH6 (Water)   ││
    │ DS18B20   │◄──┼──│ PA7  ──► ADC_CH7 (Light)   ││
    │(单总线)   │   │  │ PA9  ──► USART1_TX (Debug) ││
    └───────────┘   │  │ PA10 ──► USART1_RX         ││
                    │  ├─────────────────────────────┤│
    ┌───────────┐   │  │ PB0  ──► DS18B20           ││
    │  ESP-01S  │◄──┼──│ PB1  ──► PTC_HEATER        ││
    │  (WiFi)   │   │  │ PB7  ──► TIM4_CH2 (LED)    ││
    └───────────┘   │  │ PB8  ──► OLED_SCL          ││
                    │  │ PB9  ──► OLED_SDA          ││
    ┌───────────┐   │  │ PB10 ──► PUMP_IN           ││
    │  按键矩阵  │◄──┼──│ PB11 ──► PUMP_OUT         ││
    │ (4x按钮)  │   │  ├─────────────────────────────┤│
    └───────────┘   │  │ PC12 ──► KEY1               ││
                    │  │ PC13 ──► KEY2               ││
    ┌───────────┐   │  │ PC14 ──► KEY3               ││
    │  执行器   │◄──┼──│ PC15 ──► KEY4               ││
    │(泵/加热)  │   │  └─────────────────────────────┘│
    └───────────┘   │                                 │
                    └─────────────────────────────────┘
```

### 3.2 定时器资源分配

STM32F103C8 包含 4 个通用定时器 (TIM2-TIM4) 和 1 个高级定时器 (TIM1)。

| 定时器 | 类型 | 通道 | 用途 | 时钟源 | 配置 |
|-------|------|------|------|--------|------|
| **TIM1** | 高级 | - | 微秒延时 | APB2 (72MHz) | 72分频，1MHz计数频率 |
| **TIM2** | 通用 | CH1 | 舵机PWM | APB1 (36MHz) | 50Hz，周期20ms |
| **TIM3** | 通用 | - | 系统计时 | APB1 (36MHz) | 1ms中断，gizTimerMs |
| **TIM4** | 通用 | CH2 | LED PWM | APB1 (36MHz) | 1kHz，8位分辨率 |

#### 3.2.1 TIM1 微秒延时原理

```c
// TIM1 配置: 72分频后计数频率 = 72MHz / 72 = 1MHz
// 即每微秒计数一次

void TIM1_Delay_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    // 72分频
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;       // 最大计数
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM1, ENABLE);
}

void TIM1_Delay_us(uint32_t us)
{
    uint16_t start = TIM1->CNT;       // 记录开始值
    uint16_t target = start + us;     // 计算目标值

    if (target < start) {             // 处理溢出情况
        while (TIM1->CNT >= start);  // 等待计数器溢出
        while (TIM1->CNT < target);   // 等待到达目标
    } else {
        while (TIM1->CNT < target);   // 正常情况
    }
}
```

#### 3.2.2 TIM2 舵机PWM配置

```c
// 舵机 SG-90 规格:
// - 控制信号: 50Hz PWM
// - 周期: 20ms
// - 脉宽: 0.5ms ~ 2.5ms (对应 0° ~ 180°)

// 配置: PSC=1440-1, ARR=2000-1
// 频率 = 72MHz / 1440 / 2000 = 25Hz (接近50Hz)
// 实际使用: ARR=36000-1, PSC=1000-1
// 频率 = 72MHz / 1000 / 36000 = 2Hz (需调整)

// 正确配置
#define SERVO_PSC    720 - 1    // 100kHz
#define SERVO_ARR    2000 - 1   // 50Hz

// 角度计算: 脉宽 = 0.5ms + (角度/180) * 2ms
// CCR = (角度/180) * 2000 + 500
```

### 3.3 ADC配置详解

#### 3.3.1 ADC工作原理

ADC (Analog-to-Digital Converter) 将模拟信号转换为数字信号。STM32F103C8 集成 2 个 12 位 ADC，支持：

- 最高 1MHz 采样率
- 单一或连续转换模式
- 扫描模式 (多通道)
- DMA 传输
- 模拟看门狗

#### 3.3.2 DMA配置

```c
// DMA 配置 - ADC 自动传输
DMA_InitTypeDef DMA_InitStructure;
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  // ADC数据寄存器
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 地址不自增
DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;       // 目标数组
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 地址自增
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                // 外设→内存
DMA_InitStructure.DMA_BufferSize = 3;                             // 3个通道
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                  // 循环模式
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                     // 非内存到内存
DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
```

### 3.4 硬件模块详细规格

#### 3.4.1 传感器模块

| 传感器 | 型号 | 接口 | 量程 | 精度 | 输出 |
|--------|------|------|------|------|------|
| 温度 | DS18B20 | 单总线 | -55~125°C | ±0.5°C | 数字 |
| 浊度 | TS-300D | 模拟 | 0-4000 NTU | ±5% | 电压 |
| 水位 | 电容式 | 模拟 | 0-100% | ±3% | 电压 |
| 光照 | GL5528 | 模拟 | 0-100% | ±10% | 电阻 |

#### 3.4.2 执行器模块

| 执行器 | 型号 | 控制方式 | 功耗 | 驱动电压 |
|--------|------|----------|------|----------|
| 补水泵 | 直流潜水泵 | GPIO高电平 | 5W | 12V |
| 排水泵 | 直流潜水泵 | GPIO高电平 | 5W | 12V |
| PTC加热 | PTC加热片 | GPIO高电平 | 50W | 12V |
| 补光灯 | 5050 LED模组 | PWM调光 | 3W | 12V |
| 舵机 | SG-90 | PWM 50Hz | 0.2W | 4.8-6V |
| 蜂鸣器 | 有源蜂鸣器 | GPIO低电平触发 | 0.5W | 5V |

### 3.5 电路连接图

#### 3.5.1 电源系统

```
                            ┌──────────────────┐
                            │   DC 12V 输入    │
                            └────────┬─────────┘
                                     │
                    ┌────────────────┼────────────────┐
                    │                │                │
               ┌────▼────┐     ┌─────▼─────┐    ┌─────▼─────┐
               │ 水泵12V │     │ PTC 12V   │    │ LED 12V   │
               │         │     │           │    │           │
               │         │     │            │    │           │
               └─────────┘     └───────────┘    └───────────┘

               ┌─────────────────────────────────────────┐
               │              AMS1117-3.3               │
               │            ┌───────────┐               │
               │    12V ──► │  IN    OUT├──► 3.3V       │
               │            │   AMS1117 │               │
               │    GND ──► │  GND   GND│──► GND        │
               │            └───────────┘               │
               └─────────────────────────────────────────┘
                                     │
               ┌─────────────────────┼─────────────────────┐
               │                     │                     │
          ┌────▼────┐          ┌─────▼─────┐        ┌─────▼─────┐
          │ STM32   │          │ ESP-01S   │        │ 继电器模块 │
          │ 3.3V    │          │ 3.3V      │        │ VCC       │
          └─────────┘          └───────────┘        └───────────┘
```

#### 3.5.2 继电器驱动电路

```c
// 注意: GPIO输出高电平(3.3V)无法直接驱动继电器
// 需要使用三极管或专用驱动芯片

// 典型继电器模块电路:
// GPIO ─► 限流电阻(1kΩ) ─► 三极管基极
// 集电极 ─► 继电器线圈 ─► VCC
// 发射极 ─► GND

// 控制逻辑 (共阴极设计):
void Relay_ON(void)  { GPIO_SetBits(RELAY_PORT, RELAY_PIN); }   // 三极管导通
void Relay_OFF(void) { GPIO_ResetBits(RELAY_PORT, RELAY_PIN); }  // 三极管截止
```

---

## 4. 软件设计

### 4.1 FreeRTOS任务架构

#### 4.1.1 任务调度图

```
                        ┌────────────────────────────────┐
                        │     FreeRTOS 调度器             │
                        │                                │
     ┌──────────────────┼────────────────────────────────┼──────────────────┐
     │                  │                                │                  │
     │   ┌──────────────▼──────────────┐    ┌──────────▼───────────┐    │
     │   │    SensorDataGet_task       │    │    DevContoral_task   │    │
     │   │    ┌─────────────────────┐  │    │   ┌───────────────┐  │    │
     │   │    │ 1. 读取 ADC 传感器  │  │    │   │ MODE_MANUAL:  │  │    │
     │   │    │ 2. 读取 DS18B20    │  │    │   │   处理按键标志 │  │    │
     │   │    │ 3. 更新全局变量     │  │    │   │   控制执行器  │  │    │
     │   │    └─────────────────────┘  │    │   ├───────────────┤  │    │
     │   │    优先级: 4 (最高)         │    │   │ MODE_AUTO:    │  │    │
     │   │    栈大小: 256              │    │   │   水位控制    │  │    │
     │   │    周期: 200ms             │    │   │   温度控制    │  │    │
     │   └────────────────────────────┘    │   │   光照控制    │  │    │
     │                                     │   │   喂食控制    │  │    │
     │   ┌────────────────────────────┐    │   ├───────────────┤  │    │
     │   │      Display_task           │    │   │ MODE_REMOTE:  │  │    │
     │   │  ┌──────────────────────┐  │    │   │   处理WiFi    │  │    │
     │   │  │ 显示当前模式界面     │  │    │   │   指令       │  │    │
     │   │  │ 刷新传感器数值       │  │    │   └───────────────┘  │    │
     │   │  │ 刷新设备状态         │  │    │   优先级: 3           │    │
     │   │  └──────────────────────┘  │    │   栈大小: 256         │    │
     │   │  优先级: 2                │    └───────────────────────┘    │
     │   │  栈大小: 512              │                                 │
     │   └───────────────────────────┘                                 │
     │                                                                  │
     │   ┌────────────────┐              ┌────────────────────────┐    │
     │   │   Key_task     │              │     Gizwits_task       │    │
     │   │ ┌────────────┐ │              │ ┌────────────────────┐ │    │
     │   │ │ 扫描按键   │ │              │ │ userHandle()        │ │    │
     │   │ │ 识别短按   │ │              │ │  更新数据点        │ │    │
     │   │ │ 识别长按   │ │              │ ├────────────────────┤ │    │
     │   │ │ 设置标志位 │ │              │ │ gizwitsHandle()     │ │    │
     │   │ └────────────┘ │              │ │ 处理云平台协议     │ │    │
     │   │ 优先级: 2       │              │ └────────────────────┘ │    │
     │   │ 栈大小: 128     │              │ 优先级: 1 (最低)       │    │
     │   └─────────────────┘              │ 栈大小: 256            │    │
     │                                     └────────────────────────┘    │
     └──────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
                    ┌───────────────────────────────┐
                    │      空闲任务 (IDLE)          │
                    │  - 释放已删除任务的内存       │
                    │  - 运行 IDLE 钩子函数         │
                    └───────────────────────────────┘
```

#### 4.1.2 任务间通信

```c
// 任务间通信使用全局变量 + volatile 修饰符
// 避免使用信号量/队列以节省资源

// SensorDataGet_task → DevContoral_task
volatile uint16_t tsdata = 0;      // 浊度数据
volatile uint16_t level = 0;       // 水位数据
volatile uint16_t light = 0;      // 光照数据
volatile int16_t temp = 0;         // 温度数据

// Key_task → DevContoral_task (按键标志)
volatile uint8_t key_servo_flag = 0;
volatile uint8_t key_pump_in_flag = 0;
volatile uint8_t key_pump_out_flag = 0;
volatile uint8_t key_ptc_flag = 0;

// Key_task → Key_task (配网标志)
volatile uint8_t key_airlink_flag = 0;
volatile uint8_t key_softap_flag = 0;
volatile uint8_t key_reset_flag = 0;

// Gizwits_task ↔ DevContoral_task
// 远程模式下，gizwitsEventProcess 直接控制执行器
// 自动模式下，DevContoral_task 忽略远程指令
```

### 4.2 数据结构设计

#### 4.2.1 系统模式枚举

```c
typedef enum {
    MODE_MANUAL = 0,        // 手动模式: 本地按键直接控制
    MODE_AUTO,              // 自动模式: 根据阈值自动控制
    MODE_REMOTE,            // 远程模式: 云平台远程控制
    MODE_THRESHOLD_SET      // 阈值设置模式: 调整参数阈值
} SystemMode_t;
```

#### 4.2.2 菜单状态枚举

```c
typedef enum {
    MENU_NONE = 0,          // 无菜单，正常显示
    MENU_SELECT             // 模式选择菜单
} MenuState_t;
```

#### 4.2.3 数据点结构

```c
typedef struct {
    // 控制型数据点 (可写)
    uint8_t valueHeater;        // 加热器: 0=关闭, 1=开启
    uint8_t valueIn_WaterPump;  // 补水泵: 0=关闭, 1=开启
    uint8_t valueOut_WaterPump; // 排水泵: 0=关闭, 1=开启
    uint8_t valueFill_In_Light; // 补光灯: 0=关闭, 1=开启
    uint8_t valueServo;         // 喂食舵机: 0=关闭, 1=开启

    // 状态型数据点 (只读)
    int16_t valueTemperature;    // 温度值 (×0.1°C, 如 285 表示 28.5°C)
    uint8_t valueWaterLevel;    // 水位百分比 (0-100%)
    uint8_t valueLight;         // 光照百分比 (0-100%)
    uint8_t valueTurbidity;     // 浊度百分比 (0-100%)
} dataPoint_t;
```

#### 4.2.4 阈值配置结构

```c
typedef struct {
    uint8_t min_value;          // 最小值
    uint8_t max_value;          // 最大值
    uint8_t step;              // 调节步进
} ThresholdRange_t;

typedef struct {
    uint8_t *value_ptr;         // 指向阈值的指针
    ThresholdRange_t range;     // 取值范围
} ThresholdConfig_t;

typedef enum {
    THRESHOLD_WATER_LOW = 0,   // 水位低阈值
    THRESHOLD_WATER_HIGH,       // 水位高阈值
    THRESHOLD_TEMP_LOW,         // 温度低阈值
    THRESHOLD_TEMP_HIGH,        // 温度高阈值
    THRESHOLD_LIGHT,            // 光照阈值
    THRESHOLD_TURBIDITY,        // 浊度阈值
    THRESHOLD_COUNT             // 阈值总数
} ThresholdType_t;
```

### 4.3 程序主流程

```c
/**
 * main.c - 系统入口
 *
 * 初始化顺序:
 * 1. 硬件外设初始化 (HAL/驱动层)
 * 2. 系统组件初始化 (Flash存储、阈值)
 * 3. 云平台初始化 (机智云)
 * 4. FreeRTOS 任务创建
 * 5. 启动调度器
 */

int main(void)
{
    // ========== 阶段1: 硬件外设初始化 ==========
    OLED_Init();              // OLED 显示屏
    PWM_Init();               // PWM 输出 (舵机+LED)
    AD_Init();                // ADC + DMA
    Key_Init();               // 按键 GPIO
    Buzzer_Init();            // 蜂鸣器
    Pump_Init();              // 水泵 GPIO
    PTC_Init();               // 加热片 GPIO
    USART1_Config();          // 调试串口 (115200)
    USART2_Config();          // WiFi 串口 (9600)
    Timer3_Init(1000-1, 72-1);// 系统定时器 (1ms中断)
    Time1_Delay_Init();       // TIM1 微秒延时
    DS18B20_Init();           // 温度传感器

    printf("=== system init ok === \r\n");

    // ========== 阶段2: 系统组件初始化 ==========
    userInit();               // 机智云数据点初始化
    gizwitsInit();            // 机智云协议栈初始化

    // ========== 阶段3: FreeRTOS 任务创建 ==========
    freertos_start();         // 创建5个任务并启动调度器

    // 永远不会执行到这里
    while(1);
}
```

---

## 5. 功能模块详解

### 5.1 DS18B20 温度传感器

#### 5.1.1 单总线协议

DS18B20 采用单总线 (1-Wire) 协议，仅需一根数据线即可通信：

```
┌─────────────────────────────────────────────────────────────────┐
│                    单总线通信时序                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  MCU → DS18B20 (写操作):                                        │
│  ┌────┐  ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐                      │
│  │START│ │0│ │1│ │1│ │0│ │1│ │0│ │0│ │1│ ...                  │
│  └────┘  └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘                      │
│          ├───┘                                           ┘     │
│          写0: 低电平60us, 写1: 低电平2us, 高电平60us            │
│                                                                 │
│  DS18B20 → MCU (读操作):                                        │
│  ┌────┐    ┌─┐    ┌───────────────┐                           │
│  │START│    │←│    │ ← 数据位 ←    │                           │
│  └────┘    └─┘    └───────────────┘                           │
│          拉低2us    采样窗口: 15us后                             │
│                                                                 │
│  复位脉冲:                                                      │
│  ┌──────────┐                                                   │
│  │  低480us  │ ──► 释放总线 ──► DS18B20响应低电平 ──► 高电平    │
│  └──────────┘                                                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 5.1.2 温度读取流程

```c
/**
 * DS18B20_Get_Temp - 获取温度值
 *
 * 流程:
 * 1. 发送复位脉冲
 * 2. 检测 DS18B20 响应
 * 3. 跳过 ROM (0xCC)
 * 4. 启动温度转换 (0x44)
 * 5. 等待转换完成 (750us)
 * 6. 复位脉冲
 * 7. 跳过 ROM
 * 8. 发送读取温度命令 (0xBE)
 * 9. 读取两个字节 (TL, TH)
 * 10. 转换为实际温度值
 */
int16_t DS18B20_Get_Temp(void)
{
    uint8_t TL, TH;
    int16_t temp;

    // 1. 启动转换
    DS18B20_Start();

    // 2. 复位
    DS18B20_Rst();

    // 3. 检查存在
    if (DS18B20_Check()) return 0;

    // 4. 发送读取命令
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0xBE);  // 读取暂存器

    // 5. 读取数据
    TL = DS18B20_Read_Byte();  // 低字节
    TH = DS18B20_Read_Byte();  // 高字节

    // 6. 数据处理
    if (TH > 7) {              // 温度为负
        TH = ~TH;
        TL = ~TL;
        temp = -(TH << 8 | TL) * 0.625;
    } else {                   // 温度为正
        temp = (TH << 8 | TL) * 0.625;
    }

    return temp;               // 返回值 ×0.1°C
}
```

#### 5.1.3 精度与分辨率

| 分辨率配置 | 数据长度 | 转换时间 | 精度 |
|-----------|---------|---------|------|
| 9位 | 2字节 | 93.75ms | ±0.5°C |
| 10位 | 2字节 | 187.5ms | ±0.25°C |
| 11位 | 2字节 | 375ms | ±0.125°C |
| 12位 | 2字节 | 750ms | ±0.0625°C |

本项目使用默认12位分辨率。

### 5.2 ADC 传感器模块

#### 5.2.1 浊度传感器 (TS-300D)

```c
/**
 * TS_GetData - 获取浊度值
 *
 * 物理连接:
 *   VCC ──► 传感器 ──► 分压电阻 ──► GND
 *                   │
 *                   └──► PA5 (ADC)
 *
 * 特性:
 * - 电压越高，浊度越低
 * - 纯水电压约 2.5V
 * - 高浊度电压约 0.5V
 */
uint16_t TS_GetData(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
        sum += AD_Value[0];           // ADC_Channel_5 (PA5)
    }
    uint16_t adc_raw = sum / 10;

    // 电压转换 (mV)
    uint32_t voltage_mv = adc_raw * 3300 / 4095;

    // 计算实际电压 (考虑分压)
    float voltage = voltage_mv / U0_DIV_MV;  // U0_DIV_MV = 1000mV分压

    // 限幅
    if (voltage > 1.0f) voltage = 1.0f;
    if (voltage < 0.15f) voltage = 0.15f;

    // NTU转换 (0-4000 NTU)
    uint16_t ntu = (uint16_t)((1.0f - voltage) / 0.85f * 4000.0f);
    if (ntu > 4000) ntu = 4000;

    // 转换为百分比 (0-100%)
    return (uint16_t)((float)ntu / 4000.0f * 100.0f);
}
```

#### 5.2.2 水位传感器

```c
/**
 * Water_GetLevel - 获取水位百分比
 *
 * 原理:
 * - 水位传感器输出与水位成正比的电压
 * - Empty_ADC: 无水时ADC值 (干接点)
 * - Full_ADC: 满水位时ADC值
 *
 * 计算公式:
 * 水位% = (当前ADC值 - 空ADC值) × 100 / (满ADC值 - 空ADC值)
 */
uint16_t Water_GetLevel(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
        sum += AD_Value[1];           // ADC_Channel_6 (PA6)
    }
    uint16_t level = sum / 10;

    return (level - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
}
```

#### 5.2.3 光敏传感器 (GL5528)

```c
/**
 * Photosensitive_GetValue - 获取光照百分比
 *
 * 原理:
 * - GL5528是光敏电阻，阻值随光照强度变化
 * 光照强 → 电阻小 → 电压高 → ADC值大
 *
 * 返回值:
 * - 0%: 最暗
 * - 100%: 最亮
 */
uint16_t Photosensitive_GetValue(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
        sum += AD_Value[2];           // ADC_Channel_7 (PA7)
    }
    uint16_t Photo_Value = sum / 10;

    return (Photo_Value - Empty_ADC) * 100 / (Full_ADC - Empty_ADC);
}
```

### 5.3 执行器模块

#### 5.3.1 PWM 舵机控制

```c
/**
 * Servo_SetAngle - 设置舵机角度
 *
 * SG-90 规格:
 * - 工作电压: 4.8-6V
 * - 控制频率: 50Hz (周期20ms)
 * - 角度范围: 0°-180°
 *
 * 脉宽计算:
 * - 0°:  0.5ms  (CCR = 500)
 * - 90°: 1.5ms  (CCR = 1500)
 * - 180°:2.5ms  (CCR = 2500)
 *
 * 公式: CCR = Angle/180 * 2000 + 500
 */
void Servo_SetAngle(float Angle)
{
    // 角度限幅
    if (Angle < 0) Angle = 0;
    if (Angle > 180) Angle = 180;

    // 计算脉宽CCR值
    uint16_t pulse = (uint16_t)(Angle / 180.0f * 2000.0f + 500.0f);

    // 设置PWM比较值
    PWM_SetCompare1(pulse);
}
```

#### 5.3.2 LED 补光灯PWM调光

```c
/**
 * LED_SetBrightness - 设置LED亮度
 *
 * PWM调光原理:
 * - 占空比 = CCR / ARR
 * - 占空比越高，LED越亮
 *
 * TIM4 配置:
 * - 频率: 1kHz (周期1ms)
 * - ARR: 100
 * - CCR: 0-100 (对应0%-100%亮度)
 */
void LED_SetBrightness(uint8_t percent)
{
    if (percent > 100) percent = 100;
    PWM_SetCompare2(percent);  // 设置TIM4_CH2比较值
}

void LED_OFF(void)
{
    PWM_SetCompare2(0);        // 关闭LED
}
```

#### 5.3.3 水泵控制

```c
/**
 * 水泵控制 - 互斥保护
 *
 * 重要: 补水泵和排水泵不能同时开启！
 * 水泵使用继电器驱动，高电平开启
 */
void Pump_IN_ON(void)   { GPIO_SetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_IN); }
void Pump_IN_OFF(void)  { GPIO_ResetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_IN); }
void Pump_OUT_ON(void)  { GPIO_SetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_OUT); }
void Pump_OUT_OFF(void) { GPIO_ResetBits(PUMP_GPIO_PORT, PUMP_GPIO_PIN_OUT); }
```

### 5.4 OLED 显示模块

#### 5.4.1 I2C 软件模拟

```c
/**
 * OLED I2C 通信协议
 *
 * 从机地址: 0x78 (SA0=0) 或 0x7A (SA0=1)
 * 控制字节:
 *   - 0x00: 命令
 *   - 0x40: 数据
 *
 * I2C 时序:
 *  START → 从机地址 → ACK → 控制字节 → ACK → 数据 → ACK → STOP
 */

/**
 * OLED_I2C_Start - I2C起始信号
 * ┌────┐
 * │    │___┌───┐
 * │    │  │   │
 * └────┘  └───┘
 * SDA: 高→低
 * SCL: 高电平期间
 */
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);      // SDA拉高
    OLED_W_SCL(1);      // SCL拉高
    OLED_W_SDA(0);      // SDA产生下降沿
    OLED_W_SCL(0);      // SCL拉低，准备发送数据
}

/**
 * OLED_I2C_SendByte - 发送一个字节
 */
void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        OLED_W_SDA(Byte & (0x80 >> i));  // 发送高位在前
        OLED_W_SCL(1);                    // SCL产生上升沿
        OLED_W_SCL(0);                    // SCL拉低
    }
    OLED_W_SCL(1);    // 第9个时钟，不处理应答
    OLED_W_SCL(0);
}
```

#### 5.4.2 显示界面布局

```
┌────────────────────────────────────────┐
│  手动模式 (MODE_MANUAL)                 │
├────────────────────────────────────────┤
│ 排水:OFF  补水:OFF  加热:OFF  喂食:OFF │
├────────────────────────────────────────┤
│                                        │
│  自动模式 (MODE_AUTO)                   │
├────────────────────────────────────────┤
│ 水位: 75%  浊度: 35%  光照: 82%        │
│ 温度: +26.5°C                          │
├────────────────────────────────────────┤
│                                        │
│  远程模式 (MODE_REMOTE)                 │
├────────────────────────────────────────┤
│ Remote Mode                            │
│ lev: 75%  ts: 35%                      │
│ pho: 18%  tem: +26.5°C                 │
├────────────────────────────────────────┤
│                                        │
│  阈值设置 (MODE_THRESHOLD_SET)          │
├────────────────────────────────────────┤
│ Set Thresholds                         │
│ ----------------                        │
│ Low Level:  20                         │
│ Long:Switch Save/Exit                   │
└────────────────────────────────────────┘
```

### 5.5 按键模块

#### 5.5.1 按键扫描原理

```c
/**
 * 按键识别状态机
 *
 * 状态:
 *   IDLE ──[按下]──► PRESSED ──[松开<1s]──► SHORT_PRESS
 *         │              │
 *         │              └──[松开>=1s]──► LONG_PRESS
 *         │
 *         └──[持续松开]──► IDLE
 *
 * 时间参数:
 * - 消抖时间: 20ms
 * - 长按阈值: 1000ms
 */
typedef struct {
    uint8_t last_state;         // 上次电平状态
    uint32_t press_start_time;   // 按下开始时间
    uint8_t short_triggered;     // 短按已触发标志
    uint8_t long_triggered;      // 长按已触发标志
} Key_HandleTypeDef;

static Key_HandleTypeDef keys[4];

uint8_t Key_GetNum(void)
{
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t result = Key_ScanSingle(i);
        if (result != 0) return result;
    }
    return 0;
}
```

#### 5.5.2 按键功能映射

| 按键 | 模式 | 短按 | 长按 |
|------|------|------|------|
| PC12 | MENU_NONE | 手动:排水泵 / 远程:AirLink配网 | 进入模式选择菜单 |
| PC13 | MENU_NONE | 手动:补水泵 / 远程:SoftAP配网 | 自动:进入阈值设置 |
| PC14 | MENU_NONE | 手动:加热片 / 远程:恢复出厂 | 阈值设置:切换项目 |
| PC15 | MENU_NONE | 手动:舵机喂食 | 阈值设置:保存退出 |

---

## 6. 云平台接入

### 6.1 机智云平台架构

```
┌─────────────────────────────────────────────────────────────────┐
│                      机智云物联网平台                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────┐    ┌──────────┐    ┌──────────┐                   │
│  │ 产品管理  │    │ 设备管理  │    │ 数据管理  │                   │
│  │ 定义数据点│    │ 设备接入  │    │ 数据存储  │                   │
│  │ 配置触发器│    │ 固件升级  │    │ 历史数据  │                   │
│  └──────────┘    └──────────┘    └──────────┘                   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    GAgent 通信协议                        │  │
│  │  - 心跳: 30秒                                              │  │
│  │  - 加密: AES128                                            │  │
│  │  - 格式: JSON                                              │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
         │                    │                    │
         │ USART             │ MQTT              │ HTTP
         ▼                    ▼                    ▼
┌─────────────┐       ┌─────────────┐       ┌─────────────┐
│   ESP-01S   │       │   云服务器   │       │  手机APP    │
│   WiFi模块  │◄────►│             │◄────►│             │
└─────────────┘       └─────────────┘       └─────────────┘
         │
         ▼
┌─────────────────────────────┐
│       STM32F103C8T6         │
│                             │
│  ┌─────────────────────────┐│
│  │    gizwits_protocol.c   ││
│  │    - 数据打包/解包      ││
│  │    - 协议帧处理         ││
│  │    - 心跳维护           ││
│  └─────────────────────────┘│
└─────────────────────────────┘
```

### 6.2 配网流程详解

#### 6.2.1 AirLink 配网 (一键配置)

```
1. 设备进入 AirLink 模式
   └─► gizwitsSetMode(WIFI_AIRLINK_MODE)

2. 手机APP发送广播
   └─► SSID: Gizwits-XXXX
   └─► Password: 握手密码

3. ESP-01S 接收WiFi信息
   └─► 连接指定路由器

4. 注册到机智云服务器
   └─► 建立MQTT长连接

5. 配网成功，开始数据通信
```

#### 6.2.2 SoftAP 配网 (热点配置)

```
1. 设备进入 SoftAP 模式
   └─► gizwitsSetMode(WIFI_SOFTAP_MODE)
   └─► ESP-01S 创建热点

2. 手机连接设备热点
   └─► SSID: XPG-GAgent-XXXX

3. 手机发送WiFi配置
   └─► 通过本地HTTP

4. 设备切换为Station模式
   └─► 连接目标路由器

5. 注册到机智云服务器
```

### 6.3 数据点协议

#### 6.3.1 数据上报格式

```c
/**
 * userHandle - 用户数据采集
 *
 * 定时向上报数据点值
 * 周期: Gizwits_task (200ms)
 */
void userHandle(void)
{
    currentDataPoint.valueTemperature = temp / 10;    // 温度
    currentDataPoint.valueWaterLevel = level;         // 水位
    currentDataPoint.valueLight = light;              // 光照
    currentDataPoint.valueTurbidity = tsdata;        // 浊度
}
```

#### 6.3.2 云端控制处理

```c
/**
 * gizwitsEventProcess - 事件处理回调
 *
 * 当APP下发控制指令时触发
 */
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
    for (uint8_t i = 0; i < info->num; i++) {
        switch (info->event[i]) {
            case EVENT_Heater:
                if (currentDataPoint.valueHeater == 1) {
                    PTC_ON();
                } else {
                    PTC_OFF();
                }
                break;

            case EVENT_In_WaterPump:
                if (currentDataPoint.valueIn_WaterPump == 1) {
                    Pump_IN_ON();
                } else {
                    Pump_IN_OFF();
                }
                break;

            case EVENT_Out_WaterPump:
                if (currentDataPoint.valueOut_WaterPump == 1) {
                    Pump_OUT_ON();
                } else {
                    Pump_OUT_OFF();
                }
                break;

            case EVENT_Fill_In_Light:
                if (currentDataPoint.valueFill_In_Light == 1) {
                    LED_SetBrightness(100);
                } else {
                    LED_OFF();
                }
                break;

            case EVENT_Servo:
                if (currentDataPoint.valueServo == 1) {
                    Servo_SetAngle(60);
                } else {
                    Servo_SetAngle(0);
                }
                break;
        }
    }
    return 0;
}
```

---

## 7. 阈值配置与存储

### 7.1 Flash 存储架构

```c
/**
 * Flash 存储布局
 *
 * STM32F103C8 Flash 地址:
 * - 0x08000000 - 0x0800FFFF (64KB)
 *
 * 存储页分配:
 * ┌─────────────────────────────────┐
 * │ 0x0800F000                      │
 * │ (用户代码区)                     │
 * ├─────────────────────────────────┤
 * │ 0x0800F800  Page1 (备份页)       │
 * │ - 阈值数据                       │
 * │ - CRC校验                        │
 * │ - 有效标志 (0xA5A5)              │
 * ├─────────────────────────────────┤
 * │ 0x0800FC00  Page0 (主存储页)     │
 * │ - 阈值数据                       │
 * │ - CRC校验                        │
 * │ - 有效标志 (0xA5A5)              │
 * └─────────────────────────────────┘
 */

#define FLASH_STORAGE_PAGE0_BASE    0x0800FC00
#define FLASH_STORAGE_PAGE1_BASE    0x0800F800
#define FLASH_PAGE_SIZE             1024
#define STORAGE_FLAG_VALID          0xA5A5
```

### 7.2 双页备份机制

```c
/**
 * FlashStorage_Write - 写阈值数据
 *
 * 写入流程:
 * 1. 检测有效页 (检查0xA5A5标志)
 * 2. 备份页 = 有效页的反面
 * 3. 擦除备份页
 * 4. 写入新数据 + CRC
 * 5. 写入有效标志
 *
 * ┌────────────────────────────────┐
 * │ 初始状态:                       │
 * │ Page0: [数据A][CRC][0xA5A5]    │
 * │ Page1: [数据B][CRC][0xA5A5]    │
 * └────────────────────────────────┘
 *
 * ┌────────────────────────────────┐
 * │ 写入新数据:                     │
 * │ Page0: [数据A][CRC][0xA5A5]    │ ◄─ 当前有效
 * │ Page1: [数据B][CRC][0xFFFF]    │   擦除
 * │ Page1: [数据B+1][CRC+1][0xFFFF]│   写入
 * │ Page1: [数据B+1][CRC+1][0xA5A5]│   标记有效
 * └────────────────────────────────┘
 */

bool FlashStorage_Write(uint8_t *data, uint8_t data_len)
{
    // 1. 计算CRC
    uint8_t crc = calc_crc8(data, data_len);

    // 2. 查找有效页
    uint32_t active_page, backup_page;
    if (is_page_valid(FLASH_STORAGE_PAGE0_BASE)) {
        active_page = FLASH_STORAGE_PAGE0_BASE;
        backup_page = FLASH_STORAGE_PAGE1_BASE;
    } else if (is_page_valid(FLASH_STORAGE_PAGE1_BASE)) {
        active_page = FLASH_STORAGE_PAGE1_BASE;
        backup_page = FLASH_STORAGE_PAGE0_BASE;
    } else {
        // 无有效页，擦除Page0作为活动页
        erase_page(FLASH_STORAGE_PAGE0_BASE);
        active_page = FLASH_STORAGE_PAGE0_BASE;
        backup_page = FLASH_STORAGE_PAGE1_BASE;
    }

    // 3. 写入新数据到备份页
    erase_page(backup_page);
    write_data_to_page(backup_page, data, data_len);
    write_data_to_page(backup_page + data_len, &crc, 1);

    // 4. 标记备份页为有效
    FLASH_Unlock();
    FLASH_ProgramHalfWord(backup_page + FLASH_PAGE_SIZE - 2, STORAGE_FLAG_VALID);
    FLASH_Lock();

    return true;
}
```

### 7.3 阈值配置管理

```c
/**
 * 阈值配置表
 *
 * 每个阈值包含:
 * - 指针: 指向全局变量
 * - 范围: 最小值、最大值、调节步进
 */
static ThresholdConfig_t threshold_configs[THRESHOLD_COUNT] = {
    {&Low_Level,       {0,  100, 1}},   // 水位低阈值: 0-100%, 步进1
    {&High_Level,      {0,  100, 1}},   // 水位高阈值: 0-100%, 步进1
    {&Low_Temp,        {0,  50,  1}},   // 温度低阈值: 0-50°C, 步进1
    {&High_Temp,       {0,  50,  1}},   // 温度高阈值: 0-50°C, 步进1
    {&Threshold_Light, {0,  100, 1}},   // 光照阈值:   0-100%, 步进1
    {&Threshold_TS,    {0,  100, 1}},   // 浊度阈值:   0-100%, 步进1
};

/**
 * Threshold_Adjust - 调整阈值
 *
 * @param direction: 1=增加, 0=减少
 */
void Threshold_Adjust(uint8_t direction)
{
    ThresholdConfig_t *config = &threshold_configs[currentThreshold];
    uint8_t new_value = *config->value_ptr;

    if (direction == 1) {
        if (new_value <= config->range.max_value - config->range.step) {
            new_value += config->range.step;
        }
    } else if (direction == 0) {
        if (new_value >= config->range.min_value + config->range.step) {
            new_value -= config->range.step;
        }
    }

    *config->value_ptr = new_value;
    Threshold_Validate();  // 确保阈值有效性
}

/**
 * Threshold_Validate - 验证阈值合理性
 *
 * 确保:
 * - 水位低阈值 < 水位高阈值
 * - 温度低阈值 < 温度高阈值
 */
static void Threshold_Validate(void)
{
    if (Low_Level >= High_Level) {
        High_Level = Low_Level + 10;
        if (High_Level > 100) High_Level = 100;
    }
    if (Low_Temp >= High_Temp) {
        High_Temp = Low_Temp + 2;
        if (High_Temp > 50) High_Temp = 50;
    }
}
```

---

## 8. 状态机设计

### 8.1 系统状态机

```
                    ┌─────────────────────────────────────┐
                    │                                     │
                    │           系统状态机                  │
                    │                                     │
                    └───────────────┬─────────────────────┘
                                    │
    ┌────────────────────────────────┼────────────────────────────────┐
    │                                │                                │
    │                                ▼                                │
    │    ┌────────────────────────────────────────────────────┐     │
    │    │                                                    │     │
    │    │              MENU_SELECT (菜单选择)                  │     │
    │    │                                                    │     │
    │    │   ┌─────────┐  ┌─────────┐  ┌─────────┐          │     │
    │    │   │  Manual  │  │  Auto   │  │ Remote  │          │     │
    │    │   └────┬────┘  └────┬────┘  └────┬────┘          │     │
    │    │        │            │            │                │     │
    │    │        └────────────┼────────────┘                │     │
    │    │                     │                             │     │
    │    │    PC15短按 ────────┴──────── PC12短按            │     │
    │    │           (确认选择)       (返回)                  │     │
    │    │                                                    │     │
    │    └────────────────────────┬───────────────────────────┘     │
    │                             │                                   │
    │                             ▼                                   │
    │    ┌────────────────────────────────────────────────────────┐  │
    │    │                                                        │  │
    │    │                 MENU_NONE (正常运行)                    │  │
    │    │                                                        │  │
    │    │   ┌──────────┐   ┌──────────┐   ┌──────────┐         │  │
    │    │   │   MODE   │   │   MODE   │   │   MODE   │         │  │
    │    │   │  MANUAL  │   │   AUTO   │   │  REMOTE  │         │  │
    │    │   └────┬─────┘   └────┬─────┘   └────┬─────┘         │  │
    │    │        │              │              │                │  │
    │    │        │   ┌──────────┼──────────────┘                │  │
    │    │        │   │          │                                  │  │
    │    │        │   │   PC12长按 ──────────────────┐            │  │
    │    │        │   │   (进入菜单)                  │            │  │
    │    │        │   │                              ▼            │  │
    │    │        │   │                       ┌──────────┐       │  │
    │    │        │   │                       │  MODE    │       │  │
    │    │        │   │                       │THRESHOLD │       │  │
    │    │        │   │                       │   _SET   │       │  │
    │    │        │   │                       └────┬─────┘       │  │
    │    │        │   │                            │             │  │
    │    │        │   │   PC15长按 ────────────────┘             │  │
    │    │        │   │   (保存退出)                              │  │
    │    │        │   │                                          │  │
    │    │        │   │   PC15短按 ──────────────────────┐       │  │
    │    │        │   │   (取消退出)                      │       │  │
    │    │        │   │                                  ▼       │  │
    │    │        │   │                           MODE_AUTO      │  │
    │    │        │   │                                          │  │
    │    └────────┼───┼──────────────────────────────────────────┘  │
    │             │   │                                                    │
    └─────────────┼───┼────────────────────────────────────────────────────┘
                  │   │
        PC12长按  │   │ PC13长按(仅AUTO模式)
                  │   │
                  ▼   ▼
            ┌─────────────────┐
            │  MENU_SELECT    │
            └─────────────────┘
```

### 8.2 自动模式控制流程

```
┌─────────────────────────────────────────────────────────────────┐
│                      MODE_AUTO 控制流程                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌───────────────┐                                              │
│  │   读取传感器   │                                              │
│  │  温度/水位    │                                              │
│  │  光照/浊度    │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│          ▼                                                       │
│  ┌───────────────┐                                              │
│  │   水位控制     │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│    ┌─────┴─────┐                                                │
│    │           │                                                │
│    ▼           ▼                                                │
│  水位<低阈值  水位>高阈值                                        │
│    │           │                                                │
│    ▼           ▼                                                │
│  补水泵ON    排水泵ON                                            │
│    │           │                                                │
│    └─────┬─────┘                                                │
│          ▼                                                       │
│  ┌───────────────┐                                              │
│  │   温度控制     │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│    ┌─────┴─────┐                                                │
│    │           │                                                │
│    ▼           ▼                                                │
│  温度<低阈值  温度>高阈值                                        │
│    │           │                                                │
│    ▼           ▼                                                │
│  PTC加热ON   PTC加热OFF                                          │
│          │                                                       │
│          ▼                                                       │
│  ┌───────────────┐                                              │
│  │   光照控制     │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│    ┌─────┴─────┐                                                │
│    │           │                                                │
│    ▼           ▼                                                │
│  暗度>阈值   暗度<=阈值                                          │
│    │           │                                                │
│    ▼           ▼                                                │
│  LED按比例   LED OFF                                             │
│  补光ON                                                 │
│          │                                                       │
│          ▼                                                       │
│  ┌───────────────┐                                              │
│  │   浊度/喂食    │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│    ┌─────┴─────┐                                                │
│    │           │                                                │
│    ▼           ▼                                                │
│  浊度>阈值  浊度<=阈值                                           │
│    │           │                                                │
│    ▼           ▼                                                │
│  舵机120°   舵机0°                                              │
│          │                                                       │
│          ▼                                                       │
│  ┌───────────────┐                                              │
│  │   报警检测     │                                              │
│  └───────┬───────┘                                              │
│          │                                                       │
│    ┌─────┴─────┐                                                │
│    │           │                                                │
│    ▼           ▼                                                │
│  水位异常    水位正常                                            │
│    │           │                                                │
│    ▼           ▼                                                │
│  蜂鸣器ON   蜂鸣器OFF                                            │
│          │                                                       │
│          ▼                                                       │
│    延时500ms ──► 循环                                            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 9. 中断与定时器

### 9.1 中断向量表

```c
/**
 * STM32F103C8 中断向量表 (部分)
 *
 * 优先级说明:
 * - 数值越小，优先级越高
 * - 分抢占优先级和子优先级
 */

| 中断源          | 中断号 | 优先级组   | 用途                    |
|----------------|--------|-----------|-------------------------|
| Reset          | -1     | -         | 系统复位                 |
| NMI            | -14    | -         | 不可屏蔽中断             |
| HardFault      | -13    | -         | 硬件错误                 |
| SysTick        | -1     | 15 (最低) | FreeRTOS系统时钟        |
| PendSV         | -2     | 15 (最低) | 上下文切换              |
| TIM3_IRQn      | 29     | 5.3       | 系统定时器(gizTimerMs)  |
```

### 9.2 TIM3 中断处理

```c
/**
 * TIM3_IRQHandler - 定时器3中断
 *
 * 功能:
 * - 提供 gizTimerMs 毫秒计数
 * - 为按键扫描和延时提供时间基准
 *
 * 配置:
 * - 预分频: 72-1 (1MHz)
 * - 自动重载: 1000-1 (1kHz)
 * - 中断周期: 1ms
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
        gizTimerMs();  // 机智云毫秒计时器
    }
}

/**
 * TIM1_Delay_us - 微秒延时
 *
 * 注意: TIM1 用于DS18B20延时
 *      不能与TIM3冲突
 */
void TIM1_Delay_us(uint32_t us)
{
    uint16_t start = TIM1->CNT;
    uint16_t target = start + us;

    if (target < start) {
        while (TIM1->CNT >= start);
        while (TIM1->CNT < target);
    } else {
        while (TIM1->CNT < target);
    }
}
```

### 9.3 NVIC 优先级配置

```c
/**
 * NVIC 优先级组配置
 *
 * STM32F103使用4位优先级，分为:
 * - 高位: 抢占优先级 (pre-emption priority)
 * - 低位: 子优先级 (sub priority)
 *
 * 配置为2位抢占优先级，2位子优先级
 * - 抢占优先级: 0-3
 * - 子优先级: 0-3
 */
void Timer3_Init(uint16_t arr, uint16_t psc)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;

    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}
```

---

## 10. 编译与调试

### 10.1 Keil MDK 配置

#### 10.1.1 项目选项设置

```
┌─────────────────────────────────────────────────────────┐
│                    Options for Target                   │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  Device: STM32F103C8                                    │
│  Target: STM32F103C8                                    │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ C/C++ (AC6)                                      │   │
│  │ - Optimization: -O1 (优化等级)                   │   │
│  │ - One ELF Section per Function: √ (函数分段)     │   │
│  │ - Misc Controls: --gnu                          │   │
│  │ - Include Paths:                                 │   │
│  │   ..\Start                                       │   │
│  │   ..\Library                                    │   │
│  │   ..\User                                       │   │
│  │   ..\HardWare                                   │   │
│  │   ..\System                                     │   │
│  │   ..\Gizwits                                    │   │
│  │   ..\Utils                                      │   │
│  │   ..\FreeRTOS\include                           │   │
│  │   ..\FreeRTOS\port\ARM_CM3                      │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ Linker                                         │   │
│  │ - Misc Controls: --cpu Cortex-M3               │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ Utilities                                       │   │
│  │ - Use MicroLIB: √ (必须勾选!)                    │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

#### 10.1.2 内存配置

```
┌─────────────────────────────────────────────────────────┐
│                   Memory Configuration                   │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  IROM1:                                                 │
│    Start: 0x08000000                                    │
│    Size:  0x00010000 (64KB)                            │
│                                                         │
│  IRAM1:                                                │
│    Start: 0x20000000                                    │
│    Size:  0x00005000 (20KB)                            │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 10.2 调试配置

#### 10.2.1 SWD 调试接口

```
┌─────────────────────────────────────────┐
│           ST-LINK 调试器连接            │
├───────────────┬─────────────────────────┤
│   ST-LINK     │    STM32F103C8          │
├───────────────┼─────────────────────────┤
│     SWCLK     │     PA14 (SWCLK)        │
│     SWDIO     │     PA13 (SWDIO)        │
│     GND       │     GND                 │
│     3.3V      │     VCC (3.3V)          │
└───────────────┴─────────────────────────┘
```

#### 10.2.2 调试技巧

```c
/**
 * 1. 使用 ITM printf
 *
 * 需要ITM Stimulus端口配置:
 * - 开启 ITM 端口0
 * - 在 SWO 引脚输出
 */
#ifdef DEBUG
volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
#endif

/**
 * 2. 使用 GPIO 翻转法调试
 */
#define DEBUG_LED_PIN GPIO_Pin_13
void toggle_debug_led(void) {
    static uint8_t state = 0;
    state ^= 1;
    if (state) GPIO_SetBits(GPIOC, DEBUG_LED_PIN);
    else GPIO_ResetBits(GPIOC, DEBUG_LED_PIN);
}

/**
 * 3. 使用断点调试 FreeRTOS
 *
 * 注意: 在 PendSV_Handler 和 SysTick_Handler 中
 *      不要设置断点，会导致任务切换异常
 */
```

### 10.3 编译输出分析

```bash
# Keil 编译输出示例

Build target 'Project'
compiling main.c...
compiling main_freertos.c...
compiling ADC.c...
...
linking...
Program Size: Code=XXXX RO-data=XXXX RW-data=XXZI-data=XX
".\Objects\Project.axf" - 0 Error(s), 0 Warning(s).

# 内存分析
# Code:   编译后的指令代码
# RO-data: 只读数据 (常量)
# RW-data: 读写数据 (已初始化变量)
# ZI-data: 零初始化数据 (堆栈)
```

---

## 11. 开发注意事项

### 11.1 MicroLIB 配置

> **问题**: 移植机智云后 OLED 屏幕不能正常显示

**原因分析**:
1. 标准库 `printf` 函数依赖操作系统底层接口
2. `fputc` 重定向可能不生效
3. 内部缓冲机制导致输出丢失

**解决方案**:
在 Keil **Target Options → Utilities → Use MicroLIB** 勾选 ✓

```c
// MicroLIB 简化了标准库，适合裸机/RTOS环境
// 启用后 printf 会正确调用 fputc()
```

### 11.2 PWM 频率分离

> **问题**: 同一定时器输出舵机和 LED PWM，LED 闪烁

**原因**: SG-90 舵机需要 50Hz (周期20ms)，LED 需要 1kHz (周期1ms)

**解决方案**:
- TIM2 → 舵机 (PA0, CH1): 50Hz
- TIM4 → LED (PB7, CH2): 1kHz

```c
// PWM_Init() 中分开配置
void PWM_Init(void)
{
    // TIM2 舵机配置
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // ... 配置 50Hz

    // TIM4 LED配置
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // ... 配置 1kHz
}
```

### 11.3 FreeRTOS 延时冲突

> **问题**: 自定义 `delay()` 函数与 FreeRTOS SysTick 冲突

**原因**: FreeRTOS 使用 SysTick 作为心跳，不能被其他代码直接操作

**解决方案**:
1. 删除所有使用 SysTick 的 `delay()` 函数
2. 使用 `vTaskDelay()` 或 `vTaskDelayUntil()`
3. DS18B20 微秒延时使用 TIM1 实现

```c
/**
 * 错误写法:
 */
void delay_us(uint32_t us) {
    SysTick->LOAD = 72 * us;  // 冲突!
    SysTick->VAL = 0;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

/**
 * 正确写法:
 */
// DS18B20 延时使用 TIM1 (见 Timer.c)
void TIM1_Delay_us(uint32_t us) { ... }

// 任务延时使用 vTaskDelay
void some_task(void *pvParameters) {
    while (1) {
        // 正确: 让出CPU
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

### 11.4 临界区使用

> **问题**: 多任务访问共享资源导致数据错乱

**原因**: 任务切换发生在读取和写入之间

**解决方案**: 使用 `taskENTER_CRITICAL()` 和 `taskEXIT_CRITICAL()`

```c
/**
 * DS18B20 读取需要原子操作
 *
 * DS18B20 单总线不能被打断
 */
void SensorDataGet_task(void *pvParameters)
{
    while (1) {
        tsdata = TS_GetData();
        level = Water_GetLevel();
        light = Photosensitive_GetValue();

        // 临界区: 保护 DS18B20 通信
        taskENTER_CRITICAL();
        DS18B20_Rst();
        if (!DS18B20_Check()) {
            temp = DS18B20_Get_Temp();
        }
        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/**
 * OLED 写入也需要临界区
 *
 * I2C 通信不能被打断
 */
void OLED_WriteCommand(uint8_t Command)
{
    taskENTER_CRITICAL();
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);
    OLED_I2C_SendByte(0x00);
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
    taskEXIT_CRITICAL();
}
```

---

## 12. 常见问题排查

### 13.1 系统类问题

| 问题现象 | 可能原因 | 解决方案 |
|---------|---------|---------|
| 程序卡死 | 中断优先级冲突 | 检查 NVIC 配置 |
| 程序卡死 | 栈溢出 | 增加任务栈大小 |
| 程序卡死 | 死循环 | 检查 while 循环条件 |
| 复位不断 | 看门狗超时 | 喂狗或增加超时时间 |
| 复位不断 | 电源不稳定 | 检查电源和去耦电容 |

### 13.2 传感器类问题

| 问题现象 | 可能原因 | 解决方案 |
|---------|---------|---------|
| DS18B20 读取失败 | 总线被拉低 | 检查上拉电阻 (4.7kΩ) |
| DS18B20 读取失败 | 延时不够 | 使用示波器检查时序 |
| ADC 值跳动大 | 电源干扰 | 添加滤波电容 |
| ADC 值一直0 | 引脚配置错误 | 检查 GPIO 模式 |
| OLED 显示乱码 | I2C 接线错误 | 检查 SDA/SCL 是否接反 |

### 13.3 通信类问题

| 问题现象 | 可能原因 | 解决方案 |
|---------|---------|---------|
| WiFi 连接失败 | 密码错误 | 重新配网 |
| WiFi 连接失败 | 信号太弱 | 靠近路由器 |
| 数据不同步 | 波特率不匹配 | 检查 ESP-01S 波特率 |
| 云端无数据 | 数据点未上报 | 检查 userHandle() |
| 指令下发失败 | 设备离线 | 检查网络连接 |

### 13.4 调试方法

```c
/**
 * 1. 添加调试日志
 */
#define DEBUG_MODE 1
#if DEBUG_MODE
#define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

/**
 * 2. 闪烁 LED 调试
 */
void blink_error_code(uint8_t code)
{
    while (1) {
        for (uint8_t i = 0; i < code; i++) {
            GPIO_SetBits(GPIOC, GPIO_Pin_13);
            vTaskDelay(200);
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);
            vTaskDelay(200);
        }
        vTaskDelay(2000);  // 停顿2秒
    }
}

/**
 * 3. 串口打印调试
 */
void debug_print_all_sensors(void)
{
    printf("=== Sensor Debug ===\r\n");
    printf("Temp: %d.%d C\r\n", temp/10, temp%10);
    printf("Level: %d%%\r\n", level);
    printf("Light: %d%%\r\n", light);
    printf("Turbidity: %d%%\r\n", tsdata);
    printf("====================\r\n");
}
```

---

## 附录

### A. 代码统计

```
文件类型        数量    代码行数
────────────────────────────────
.c/.h 文件      45      ~8000
驱动模块        12      ~3000
用户逻辑        5       ~1500
机智云协议      8       ~3500
FreeRTOS        12      ~5000
库文件          8       ~3000
```

### B. 资源占用

```
资源类型        使用        总量        使用率
─────────────────────────────────────────────
Flash          ~48KB      64KB        75%
RAM            ~12KB      20KB        60%
GPIO           18          37          49%
定时器         4          4           100%
ADC通道        3          10          30%
串口           2          3           67%
```

### C. 参考资料

- [STM32F103参考手册](https://www.st.com/resource/en/reference_manual/cd00171190.pdf)
- [FreeRTOS官方文档](https://www.freertos.org/)
- [机智云开发者中心](https://dev.gizwits.com/)
- [DS18B20数据手册](https://www.maximintegrated.com/)

---

*文档版本: 2.0*
*最后更新: 2026-04-15*
