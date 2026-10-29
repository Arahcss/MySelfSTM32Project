# 独立按键驱动（仅 Device 层）实现计划

## 1. 概述（Summary）

在 `User/Device/` 下新增按键驱动模块 `BtnDevice.c` / `BtnDevice.h`，套用现有 `LedDevice` 的"静态参数 / 运行参数分离 + 全局设备数组 + 周期执行"模式。

**设计原则：驱动只提供接口，不关心任何具体硬件信息。** 端口、引脚、按下时的有效电平、消抖阈值全部由调用方通过静态参数传入；GPIO 的输入模式与上下拉由你在 CubeMX 中配置。驱动内部只做三件事：读引脚、换算逻辑状态、消抖并产出状态与边沿事件。

**范围限定**：本次仅实现 Device 层，不新增 Operation / Task 层文件；不修改 `Core/`、`Drivers/`、`Middlewares/`、`MDK-ARM/`（遵循规范 09）。

新增文件（2 个）：

- `User/Device/BtnDevice.h`
- `User/Device/BtnDevice.c`

## 2. 现状分析（Current State Analysis）

| 项目 | 现状 |
|------|------|
| 分层与调用 | Task → Operation → Device 单向；Device 是唯一可包含 `stm32f1xx_hal.h` 的层 |
| 同类模块范式 | [LedDevice.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/LedDevice.h) 定义 `stLedStaticParamTdf` / `stLedRunningParamTdf` / `stLedDeviceParamTdf`；`.c` 中定义 `astLedDeviceParam[LED_DEV_NUM]`；接口为 `vXxxDeviceInit` / `vXxxDeviceRunningParamInit` / `c_pstGetXxxDeviceParam` / `vXxxDevicePeriodExecute` |
| 设备号管理 | `emXxxDevNum0..N` 枚举 + `XXX_DEV_NUM` 数量宏 + 别名宏 |
| 初始化 | `memcpy` 结构体拷贝，长度 `sizeof(类型)/sizeof(uint8_t)`；**GPIO 模式由 CubeMX 的 `MX_GPIO_Init` 配置，Device 层只读写引脚** |
| 命名预留 | [Project_Config.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Project_Config.h#L9-L13) 已定义 `BTN_DEV_NUM 4` 与 `BUTTON_UP/DOWN/ENTER/CANCEL → emBtnDevNum0~3`（当前全工程未被引用） |
| 引脚配置 | [LedAIExample.ioc](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/LedAIExample.ioc) 目前仅配置 PC13 为输出，按键引脚待你在 CubeMX 中添加 |

## 3. 职责边界（Responsibility Boundary）

| 角色 | 负责内容 |
|------|----------|
| **CubeMX（你操作）** | 4 个按键引脚的 GPIO 模式（`GPIO_Input`）、上拉/下拉、时钟使能；生成 `MX_GPIO_Init` |
| **调用方（后续 Operation 层）** | 通过 `stBtnStaticParamTdf` 传入 `pstGpioBase`、`usGpioPin`、`emActiveLevel`、`usDebounceThreshold`，并按固定周期调用 `vBtnDevicePeriodExecute` |
| **本驱动（Device 层）** | 读引脚 → 换算逻辑状态 → 消抖 → 输出稳定状态与边沿事件；不写死任何端口、引脚、极性、阈值，不初始化 GPIO |

## 4. 修改方案（Proposed Changes）

### 4.1 新增 `User/Device/BtnDevice.h`

```c
#ifndef _BTN_DEVICE_H_
#define _BTN_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "Project_Config.h"

///	@brief					设备号枚举
///
///	@note					与 Project_Config.h 中 BUTTON_* 宏对应
typedef enum
{
	emBtnDevNum0				=	0,
	emBtnDevNum1,
	emBtnDevNum2,
	emBtnDevNum3,
}emBtnDevNumTdf;

///	@brief					按键按下时的电平枚举
///
///	@note
typedef enum
{
	emBtnActiveLevel_Low		=	0,
	emBtnActiveLevel_High,
}emBtnActiveLevelTdf;

///	@brief					按键稳定状态枚举
///
///	@note
typedef enum
{
	emBtnStatus_Released		=	0,
	emBtnStatus_Pressed,
}emBtnStatusTdf;

///	@brief					按键边沿事件枚举
///
///	@note					由 emBtnGetEvent 读取后自动清零
typedef enum
{
	emBtnEvent_None			=	0,
	emBtnEvent_Press,
	emBtnEvent_Release,
}emBtnEventTdf;

///	@brief			静态参数定义
///
///	@note
typedef struct
{
	GPIO_TypeDef							*pstGpioBase;			//	使用的GPIOx
	uint16_t								usGpioPin;				//	使用的GPIO_PIN_x
	emBtnActiveLevelTdf			emActiveLevel;			//	按下时的电平
	uint16_t								usDebounceThreshold;	//	消抖阈值（周期次数）
}
stBtnStaticParamTdf;

///	@brief			运行参数定义
///
///	@note
typedef struct
{
	emBtnStatusTdf					emCurrentStatus;			//	消抖后的稳定状态
	emBtnEventTdf					emEvent;						//	待处理边沿事件
	uint16_t								usDebounceCount;			//	当前消抖计数
}
stBtnRunningParamTdf;

///	@brief			按键设备参数总结构
///
///	@note
typedef struct
{
	stBtnRunningParamTdf	stRunningParam;		//运行参数
	stBtnStaticParamTdf		stStaticParam;		//静态参数
}
stBtnDeviceParamTdf;

void vBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum);
void vBtnDeviceRunningParamInit(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum);
void vBtnDevicePeriodExecute(emBtnDevNumTdf emDevNum);
const stBtnDeviceParamTdf	*c_pstGetBtnDeviceParam(emBtnDevNumTdf emDevNum);
emBtnStatusTdf emBtnGetStatus(emBtnDevNumTdf emDevNum);
emBtnEventTdf emBtnGetEvent(emBtnDevNumTdf emDevNum);

#endif
```

要点：

- `BTN_DEV_NUM` 与 `BUTTON_*` 保留在 `Project_Config.h`（既有预留，不改动），本头文件包含它。宏为文本替换，`BUTTON_UP` 展开引用的枚举成员在本文件中已定义，无顺序问题。
- 消抖**阈值**放静态参数（调用方一次配置），**计数**放运行参数，二者不混用。

### 4.2 新增 `User/Device/BtnDevice.c`

```c
#include "BtnDevice.h"
#include "string.h"

stBtnDeviceParamTdf	astBtnDeviceParam[BTN_DEV_NUM];

///	@brief				按键扫描执行（读取引脚 + 消抖 + 事件生成）
///
///	@param			emDevNum		:	设备编号
///
///	@note				消抖以本函数被调用的次数为基准
static void vBtnDeviceScanExecute(emBtnDevNumTdf	emDevNum)
{
	uint8_t ucPinLevel;
	emBtnStatusTdf emRawStatus;

	//读取引脚电平
	ucPinLevel = (uint8_t)HAL_GPIO_ReadPin(astBtnDeviceParam[emDevNum].stStaticParam.pstGpioBase, astBtnDeviceParam[emDevNum].stStaticParam.usGpioPin);

	//换算为逻辑状态
	emRawStatus = (emBtnStatusTdf)!(ucPinLevel ^ astBtnDeviceParam[emDevNum].stStaticParam.emActiveLevel);

	//原始状态与稳定状态一致则清零消抖计数
	if(emRawStatus == astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus)
	{
		astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;
	}
	else
	{
		//消抖计数累加
		astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount++;

		//达到阈值后更新稳定状态
		if(astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount >= astBtnDeviceParam[emDevNum].stStaticParam.usDebounceThreshold)
		{
			astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus = emRawStatus;

			astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;

			//生成边沿事件
			if(emRawStatus == emBtnStatus_Pressed)
			{
				astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_Press;
			}
			else
			{
				astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_Release;
			}
		}
	}
}

///	@brief				按键周期执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				需以固定周期调用，调用周期即消抖的时间基准
void vBtnDevicePeriodExecute(emBtnDevNumTdf	emDevNum)
{
	vBtnDeviceScanExecute(emDevNum);
}

///	@brief				获取按键设备参数
///
///	@param			emDevNum		:	设备编号
///
///	@note				返回const指针，仅供只读访问
const stBtnDeviceParamTdf	*c_pstGetBtnDeviceParam(emBtnDevNumTdf emDevNum)
{
	return &astBtnDeviceParam[emDevNum];
}

///	@brief				获取按键稳定状态
///
///	@param			emDevNum		:	设备编号
///
///	@note				消抖后的状态，不受 emBtnGetEvent 影响
emBtnStatusTdf emBtnGetStatus(emBtnDevNumTdf emDevNum)
{
	return astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus;
}

///	@brief				读取并清除按键边沿事件
///
///	@param			emDevNum		:	设备编号
///
///	@note				读取后事件自动清零，避免重复处理
emBtnEventTdf emBtnGetEvent(emBtnDevNumTdf emDevNum)
{
	emBtnEventTdf emEvent;

	emEvent = astBtnDeviceParam[emDevNum].stRunningParam.emEvent;

	astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_None;

	return emEvent;
}

///	@brief				按键运行参数初始化
///
///	@param			pstInit			:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///
///	@note
void vBtnDeviceRunningParamInit(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
	memcpy(&astBtnDeviceParam[emDevNum].stRunningParam, pstInit, sizeof(stBtnRunningParamTdf) / sizeof(uint8_t));
}

///	@brief				按键设备初始化
///
///	@param			pstInit			:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///
///	@note				引脚模式与上下拉由CubeMX配置，本函数只记录静态参数
void vBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
	memcpy(&astBtnDeviceParam[emDevNum].stStaticParam, pstInit, sizeof(stBtnStaticParamTdf) / sizeof(uint8_t));
}
```

实现要点：

- 电平换算沿用 LED 的异或取反公式 `!(pinLevel ^ emActiveLevel)`：低有效时 `pin=0 → Pressed`，高有效时 `pin=1 → Pressed`，与具体硬件无关。
- `emBtnGetEvent` 写运行参数（读清语义），故不加 `const`，返回枚举用 `em` 前缀（见决策 2）。
- `vBtnDevicePeriodExecute` 为薄封装，扫描逻辑收敛为 `static`，与 `vLedDeviceBreathExecute` 的收敛方式一致。

## 5. 假设与决策（Assumptions & Decisions）

1. **驱动方式为周期轮询**：与 LedDevice 一致，不引入 EXTI 与中断回调（那需要改 `Core/`，规范 09 禁止）。
2. **新增"返回枚举"的函数前缀用 `em`**：规范 03 只定义了 `v`（void）与 `c_pst`（const 结构体指针），`em` 与变量前缀表一致，属规则的自然延伸。
3. **`BTN_DEV_NUM` / `BUTTON_*` 仍留在 `Project_Config.h`**：不改动现有文件，按最小改动原则保留；虽与 LED 把别名放模块头文件的做法略有差异，但属既有预留。
4. **Device 层不初始化 GPIO、不使能时钟**：完全交给 CubeMX。这样驱动内不出现端口→时钟映射，保持与硬件解耦。
5. **消抖以调用次数计**：`usDebounceThreshold = N` 表示"连续 N 次采样与当前稳定状态不同才确认翻转"。若按 10 ms 周期调用，取 2 即 20 ms 消抖。
6. **不含短按/长按/连发**：Device 层只输出稳定状态与边沿，长按判定由后续 Operation 层基于稳定状态自行计时，避免 Device 层膨胀。

## 6. 验证（Verification）

1. **静态检查**（Grep 扫描新增两个文件）：
   - `_BTN_DEVICE_H_` 首尾配对
   - 无 4 空格缩进（`^    \S`）、无行尾空白、无 `电频`、无 `;//` 行尾注释
   - 结构体成员 Tab 三列对齐，所有函数具 `///` Doxygen 块
   - 全文不出现具体 `GPIOA/GPIOB/...`、`GPIO_PIN_x` 常量（确认零写死）
2. **一致性核对**：对照 `LedDevice.h/.c` 逐项确认 `Tdf` 后缀、匈牙利前缀（`st/em/us/uc/pst`）、`emDevNum` 形参名、接口命名顺序一致。
3. **编译验证**：`BtnDevice.c` 需加入 Keil target（`MDK-ARM/*.uvprojx` 属规范 09 禁止范围，需你在 Keil 中手动 Add Existing Files，或明确授权我修改工程文件）。编译应 0 error。
4. **功能验证**（待 CubeMX 配好引脚、补 Operation 层后）：周期调用 `vBtnDevicePeriodExecute`，按下与松开各产生一次对应边沿事件，抖动期间不产生重复事件，`emBtnGetStatus` 与引脚实际状态一致。
