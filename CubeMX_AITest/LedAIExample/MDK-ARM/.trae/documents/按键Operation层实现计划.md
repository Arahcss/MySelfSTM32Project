# 按键 Operation 层实现计划

## 1. 概述（Summary）

在 `User/Operation/` 下新增 `Btn_Operation.c` / `Btn_Operation.h`，完全套用 `Led_Operation` 范式，向上层（Task）暴露两个接口：

- `vBtnInitAll(void)` — 为 4 个按键填入静态参数并逐个调用 `vBtnDeviceInit`
- `vBtnExecuteAll(void)` — 遍历调用 `vBtnDevicePeriodExecute`，驱动全部按键的扫描消抖

**范围限定**：仅 Operation 层 2 个新文件，不改动任何现有文件；Task 层接入（新建 `Btn_Task.c`、注册周期任务）属后续工作，不在本次范围。

## 2. 现状分析（Current State Analysis）

| 项目 | 现状 |
|------|------|
| Device 层 | [BtnDevice.c/.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/BtnDevice.h) 已落地：6 接口、静态/运行参数分离、消抖阈值在静态参数中 |
| Operation 范式 | [Led_Operation.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Operation/Led_Operation.c)：`stXxxStaticParamTdf stInit = {0};` → 逐项赋值 → `vXxxDeviceInit(&stInit, emXxxDevNumN)`；`vXxxExecuteAll` 聚合执行 |
| 设备号宏 | [Project_Config.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Project_Config.h#L9-L13)：`BTN_DEV_NUM 4`，`BUTTON_UP/DOWN/ENTER/CANCEL → emBtnDevNum0~3` |
| 引脚配置 | `.ioc` 已确认 4 引脚全部 `GPIO_Input` + `GPIO_PULLUP` → **低电平有效**（`emBtnActiveLevel_Low`） |

## 3. 引脚映射（已确认）

| 别名宏 | 设备号 | 端口 | 引脚 | 有效电平 |
|--------|--------|------|------|----------|
| `BUTTON_UP` | `emBtnDevNum0` | GPIOA | GPIO_PIN_1 | 低 |
| `BUTTON_DOWN` | `emBtnDevNum1` | GPIOA | GPIO_PIN_0 | 低 |
| `BUTTON_ENTER` | `emBtnDevNum2` | GPIOC | GPIO_PIN_15 | 低 |
| `BUTTON_CANCEL`（ESC） | `emBtnDevNum3` | GPIOC | GPIO_PIN_14 | 低 |

## 4. 修改方案（Proposed Changes）

### 4.1 新增 `User/Operation/Btn_Operation.h`

```c
#ifndef _BTN_OPERATION_H_
#define _BTN_OPERATION_H_

#include "BtnDevice.h"


void vBtnExecuteAll(void);
void vBtnInitAll(void);

#endif
```

### 4.2 新增 `User/Operation/Btn_Operation.c`

```c
#include "Btn_Operation.h"

///	@brief					初始化所有按键设备
///
///	@param					None
///
///	@note					引脚与上下拉由CubeMX配置，此处只登记静态参数
void vBtnInitAll(void)
{
	stBtnStaticParamTdf stInit = {0};

	//UP按键
	stInit.pstGpioBase = GPIOA;
	stInit.usGpioPin = GPIO_PIN_1;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_UP);

	//DOWN按键
	stInit.pstGpioBase = GPIOA;
	stInit.usGpioPin = GPIO_PIN_0;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_DOWN);

	//ENTER按键
	stInit.pstGpioBase = GPIOC;
	stInit.usGpioPin = GPIO_PIN_15;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_ENTER);

	//CANCEL按键
	stInit.pstGpioBase = GPIOC;
	stInit.usGpioPin = GPIO_PIN_14;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_CANCEL);
}

///	@brief					执行所有按键设备
///
///	@param					None
///
///	@note					需以固定周期调用，建议10ms
void vBtnExecuteAll(void)
{
	emBtnDevNumTdf emDevNum;

	for(emDevNum = emBtnDevNum0; emDevNum < BTN_DEV_NUM; emDevNum++)
	{
		vBtnDevicePeriodExecute(emDevNum);
	}
}
```

要点：

- `vBtnInitAll` 复用同一 `stInit` 变量逐设备覆盖赋值（`{0}` 一次清零 + 每轮全字段重写，无残留风险），与 `Led_Operation.c` 风格一致。
- `vBtnExecuteAll` 用 for 循环遍历 `emBtnDevNum0 ~ BTN_DEV_NUM-1`，语义即"执行全部设备"，扩设备时无需改此函数。
- 设备号实参使用 `BUTTON_UP` 等别名宏（呼应 `Project_Config.h` 预留），而非裸枚举。

## 5. 假设与决策（Assumptions & Decisions）

1. **有效电平 = 低**：`.ioc` 中 4 引脚均 `GPIO_PULLUP` 输入，按下接地，故 `emBtnActiveLevel_Low`，无需再问。
2. **消抖阈值 = 2**：计划建议扫描周期 10ms，阈值 2 即 20ms 消抖（机械按键常规值）。后续 Task 层若改周期，按比例调整。
3. **ESC 对应 `BUTTON_CANCEL`**：`Project_Config.h` 无 `BUTTON_ESC` 宏，沿用既有 `BUTTON_CANCEL`（`emBtnDevNum3`），不新增宏、不改现有文件。
4. **不含 Task 层**：`vBtnInitAll`/`vBtnExecuteAll` 需由后续 `Btn_Task` 周期调用才生效；本次只交付 Operation 层。
5. **风格对齐**：Tab 缩进、Doxygen 模板、逗号后空格、guard `_BTN_OPERATION_H_`，均按规范 02/04/05 执行。

## 6. 验证（Verification）

1. Grep 扫描两个新文件：无 4 空格缩进（`^    \S`）、无行尾空白、无 `;//` 行尾注释；guard `_BTN_OPERATION_H_` 首尾配对。
2. 核对 4 组静态参数与第 3 节映射表逐项一致。
3. 确认未改动任何现有文件（`git status` 仅 2 个新增）。
4. 编译验证（需你操作）：将 `Btn_Operation.c` 加入 Keil target，编译应 0 error（若 `BtnDevice.c` 尚未加入 target 会报链接错误，请一并加入）。
