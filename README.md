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

## 环境配置

### 依赖软件及版本

| 软件 | 版本要求 | 用途 |
|------|----------|------|
| Keil MDK (uVision5) | ≥ 5.06 (ARMCC V5.06 update 5) | IDE 编译器 |
| STM32F1xx DFP Pack | 2.2.0 (Keil.STM32F1xx_DFP.2.2.0) | STM32 器件支持包 |
| ST-Link | 最新固件 | 下载调试器 |
| 机智云 GoKit | — | 云平台调试工具 |
| ESP-01S 固件 | 机智云适配固件 (GAgent) | WiFi 通信模块 |

### 固件层依赖

| 组件 | 版本 | 说明 |
|------|------|------|
| STM32 Standard Peripheral Library | V3.5.0 | STM32 标准外设库 |
| FreeRTOS | V202212.00 | 实时操作系统内核 |
| 机智云 GAgent 协议 | V03030000 | 云平台通信协议 |

### 配置文件说明

| 文件 | 路径 | 用途 |
|------|------|------|
| `FreeRTOSConfig.h` | `User/` | FreeRTOS 内核配置（CPU 频率、Tick 率、任务优先级、堆大小） |
| `stm32f10x_conf.h` | `User/` | STM32 标准外设库裁剪配置，按需启用外设模块 |
| `gizwits_product.h` | `Gizwits/` | 机智云产品配置（Product Key、Product Secret、软硬件版本号、数据点定义） |
| `gizwits_protocol.h` | `Gizwits/` | 机智云通信协议配置（协议版本、事件枚举、数据点范围与长度） |
| `c_cpp_properties.json` | `.vscode/` | VS Code IntelliSense 头文件路径与宏定义配置 |
| `Project.uvprojx` | 项目根目录 | Keil 工程文件（包含编译器版本、芯片型号、头文件路径、源文件列表、编译选项） |

### 关键配置参数

**数据点定义**
| 数据点 | 类型 | 范围 | 说明 |
|--------|------|------|------|
| Heater | Bool | 0/1 | 加热控制 |
| In_WaterPump | Bool | 0/1 | 补水泵控制 |
| Out_WaterPump | Bool | 0/1 | 排水泵控制 |
| Fill_In_Light | Bool | 0/1 | 补光灯控制 |
| Servo | Bool | 0/1 | 舵机喂食控制 |
| Temperature | UInt32 | 0-100 | 温度值 |
| WaterLevel | UInt32 | 0-100 | 水位百分比 |
| Light | UInt32 | 0-100 | 光照百分比 |
| Turbidity | UInt32 | 0-100 | 浊度值 |

## 部署步骤

### 1. 开发环境搭建

1. 安装 **Keil MDK 5.06** 或更高版本
2. 安装 **STM32F1xx DFP Pack 2.2.0**（可通过 Keil Pack Installer 安装）
3. 安装 **ST-Link** 驱动程序
4. （可选）安装 **VS Code** 并配置 `keil-assistant` 插件用于代码浏览

### 2. 项目获取

```bash
# 克隆项目到本地
git clone <项目地址>
cd Intelligent-Fish-Tank-System
```

### 3. Keil 工程配置

1. 打开 `Stm32_Fish_System/Project.uvprojx`
2. 确认芯片型号为 `STM32F103C8`
3. 确认编译器版本为 `ARMCC V5.06`
4. 检查头文件路径是否完整（Project → Options → C/C++ → Include Paths）
5. 检查调试器配置（Project → Options → Debug → 选择ST-Link）


### 4. 编译固件

1. 在 Keil 中点击 **Build** (F7) 编译项目
2. 确认 Output 窗口显示 `0 Error(s), 0 Warning(s)`
3. 编译产物生成在 `Objects/` 目录下（`.hex` / `.bin` 文件）

### 5. 烧录固件

**使用 ST-Link:**
```
1. 连接 ST-Link 至 STM32 (SWDIO, SWCLK, GND, 3.3V)
2. Keil: Project → Options → Debug → ST-Link Debugger → Settings
3. Flash Download 中添加 STM32F10x 128KB Flash 算法
4. 点击 Download (F8) 烧录
```

### 6. ESP-01S 固件

ESP-01S 模块需预先烧录机智云 **GAgent WiFi 固件**：
- 固件版本需与协议版本 `P0_VERSION 00000002` 兼容
- 烧录工具：乐鑫 Flash Download Tools
- 烧录完成后将 ESP-01S 接入 USART2

### 7. 验证部署

1. 系统上电，观察 OLED 是否正常显示
2. 通过串口调试工具（波特率 9600）连接 USART1，查看启动日志：
   ```
   === system init ok ===
   ```
3. 短按 KEY1 切换模式，确认按键响应正常
4. 使用机智云 APP 进行配网，确认设备上线
5. 测试各传感器数据是否正常上报
6. 测试各执行器控制是否响应

## 使用说明

### 1. 硬件准备

1. 将各传感器和执行器按引脚分配表连接至 STM32F103C8
2. 确认 ESP-01S WiFi 模块已正确连接至 USART2
3. 接通电源，系统启动后 OLED 屏幕显示初始状态

### 2. WiFi 配网

1. 系统上电后，ESP-01S 进入 AirLink 配网模式（指示灯快闪）
2. 打开手机端**机智云 APP**，添加设备
3. 输入家中 WiFi 名称和密码，开始配网
4. 配网成功后，ESP-01S 指示灯常亮，OLED 显示已连接状态

### 3. 按键操作

| 按键 | 功能 |
|------|------|
| KEY1 | 切换自动/手动模式 |
| KEY2 | 手动模式下切换选中设备 |
| KEY3 | 手动模式下开关/关闭选中设备 |
| KEY4 | 进入/退出参数设置（阈值调整） |

### 4. 功能说明

- **自动模式**: 系统根据传感器数据自动执行控制逻辑
  - 水温低于设定阈值 → PTC 加热启动
  - 水位低于设定阈值 → 补水泵启动
  - 水位高于设定阈值 → 排水泵启动
  - 光照低于设定阈值 → 补光灯开启
  - 浊度高于设定阈值 → 增氧泵启动

- **手动模式**: 通过按键手动控制各设备开关

- **远程模式**: 通过机智云手机APP手动控制各设备开关

- **喂食控制**: 通过 APP 触发舵机旋转，自动投喂鱼食

- **报警功能**: 水位异常时蜂鸣器鸣响报警

### 5. 手机 APP 使用

1. 下载并安装**机智云 APP**
2. 注册/登录账号
3. 配网成功后设备自动出现在设备列表
4. 点击进入设备控制面板，可：
   - 实时查看水温、水位、光照、浊度数据
   - 切换自动/手动模式
   - 手动控制加热、补水、排水、增氧、照明、喂食
   - 查看报警信息
