# 通用矩阵键盘驱动实现计划

## 一、概要

在现有三层架构（Task → Operation → Device）下新增通用矩阵键盘模块：

- **Device 层**：行/列引脚登记、整屏扫描、逐键消抖、边沿事件生成
- **Operation 层**：引脚与行列数量配置、批量化接口、供业务层轮询的事件/位掩码接口
- **Task 层**：`vKeyPadTask` 10ms 周期扫描

行列数量由 `Project_Config.h` 宏控制（默认 4×4，共 16 键）。引脚先按 4×4 占位，改行列数或改引脚只需动宏与 Operation 层两张引脚表。引脚模式固定为：**行 = 上拉输入，列 = 开漏输出**（由 CubeMX 配置，Device 层不初始化 GPIO，遵守规范 01）。暂不实现组合键，但通过 `ulKeyPadGetPressedMask()` 预留位掩码扩展点。

---

## 二、现状分析

### 2.1 可复用的既有模式（BtnDevice）

- `astBtnDeviceParam[BTN_DEV_NUM]` 全局数组 + 设备号枚举下标
- 静态参数（`pstGpioBase` / `usGpioPin` / 阈值）经 `vBtnDeviceInit` 用 `memcpy` 登记
- 运行参数（`emCurrentStatus` / `emEvent` / `usDebounceCount`）
- `vBtnDevicePeriodExecute(emDevNum)` 以调用次数为消抖时间基准
- `emBtnGetEvent()` 读清语义；`c_pstGetBtnDeviceParam()` 只读访问
- Operation 层 `vBtnInitAll` 逐项填参、`vBtnExecuteAll` 循环驱动、`emBtnGetPressEvent` 返回首个 Press（无事件返回 `BTN_DEV_NUM` 哨兵）

### 2.2 引脚占用现状（`LedAIExample.ioc` + `main.c`）

| 用途 | 引脚 |
|------|------|
| LED | PC13 |
| 独立按键 | PA0、PA1、PC14、PC15 |
| OLED 软件 I2C | PB8、PB9 |
| SWD | PA13、PA14 |
| HSE 晶振 | PD0、PD1 |

**4×4 矩阵键盘占位引脚（推荐方案，均已确认空闲）**

| 角色 | 引脚 | CubeMX 模式 |
|------|------|-------------|
| 行 0~3 | PA2、PA3、PA4、PA5 | `GPIO_Input` + `GPIO_PULLUP` |
| 列 0~3 | PA6、PA7、PB0、PB1 | `GPIO_Output` + **Open Drain** + `GPIO_PULLUP` |

避开了 SWD（PA13/PA14）、HSE（PD0/PD1）、原 OLED（PB8/PB9）和 JTAG（PB3/PB4）。

### 2.3 资源约束

- `configTOTAL_HEAP_SIZE = 3072`，现有 4 任务 + 1 队列已占用约 2.5KB，余量不足以再容纳一个动态任务（128 Words 栈 + TCB）
- 规范 09：`Core/`、`MDK-ARM/*.uvprojx`、`.ioc` **禁止修改**，因此 CubeMX/Keil 侧动作必须由用户执行

---

## 三、改动清单

### 3.1 `User/Project_Config.h`（修改）

在末尾追加：

```c
#define	KEYPAD_ROW_NUM				4
#define	KEYPAD_COL_NUM				4
#define	KEYPAD_DEV_NUM				(KEYPAD_ROW_NUM * KEYPAD_COL_NUM)
#define	KEYPAD_DEBOUNCE_THRESHOLD	2
```

说明：`KEYPAD_DEBOUNCE_THRESHOLD = 2`，在 10ms 任务周期下即 20ms 消抖，与 `Btn_Operation.c` 中按键阈值一致。

### 3.2 `User/Device/KeyPadDevice.h`（新建）

```c
#ifndef _KEYPAD_DEVICE_H_
#define _KEYPAD_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "Project_Config.h"

///	@brief					设备号枚举（16 键，行优先 row*COL+col）
///
///	@note					若调整 KEYPAD_ROW_NUM/COL_NUM，需同步增删成员
typedef enum
{
	emKeyPadDevNum0				=	0,
	emKeyPadDevNum1,
	...
	emKeyPadDevNum15,
}emKeyPadDevNumTdf;

///	@brief					按键稳定状态枚举
typedef enum
{
	emKeyPadStatus_Released		=	0,
	emKeyPadStatus_Pressed,
}emKeyPadStatusTdf;

///	@brief					按键边沿事件枚举
///
///	@note					由 emKeyPadGetEvent 读取后自动清零
typedef enum
{
	emKeyPadEvent_None			=	0,
	emKeyPadEvent_Press,
	emKeyPadEvent_Release,
}emKeyPadEventTdf;

///	@brief			行列引脚描述
typedef struct
{
	GPIO_TypeDef				*pstGpioBase;			//	使用的GPIOx
	uint16_t					usGpioPin;				//	使用的GPIO_PIN_x
}stKeyPadPinTdf;

///	@brief			静态参数定义（每键记录所属行列索引）
typedef struct
{
	uint8_t						ucRowIndex;				//	行索引 0~KEYPAD_ROW_NUM-1
	uint8_t						ucColIndex;				//	列索引 0~KEYPAD_COL_NUM-1
	uint16_t					usDebounceThreshold;	//	消抖阈值（周期次数）
}stKeyPadStaticParamTdf;

///	@brief			运行参数定义
typedef struct
{
	emKeyPadStatusTdf			emCurrentStatus;		//	消抖后的稳定状态
	emKeyPadEventTdf			emEvent;				//	待处理边沿事件
	uint16_t					usDebounceCount;		//	当前消抖计数
}stKeyPadRunningParamTdf;

///	@brief			按键设备参数总结构
typedef struct
{
	stKeyPadRunningParamTdf		stRunningParam;
	stKeyPadStaticParamTdf		stStaticParam;
}stKeyPadDeviceParamTdf;

void vKeyPadDeviceInit(const stKeyPadStaticParamTdf *pstInit, emKeyPadDevNumTdf emDevNum);
void vKeyPadDeviceRowPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucRowIndex);
void vKeyPadDeviceColPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucColIndex);
void vKeyPadDevicePeriodExecute(void);
const stKeyPadDeviceParamTdf	*c_pstGetKeyPadDeviceParam(emKeyPadDevNumTdf emDevNum);
emKeyPadStatusTdf emKeyPadGetStatus(emKeyPadDevNumTdf emDevNum);
emKeyPadEventTdf emKeyPadGetEvent(emKeyPadDevNumTdf emDevNum);

#endif
```

> 行列引脚作为"总线类硬件资源"，单独用 `astKeyPadRowPin[]` / `astKeyPadColPin[]` 两张小表管理；每个按键只存行列索引，避免 16 键重复存 4 组引脚信息。这与规范 6.2 "设备参数集中在 Device 层 .c" 不冲突（按键参数仍集中在 `astKeyPadDeviceParam[]`）。

### 3.3 `User/Device/KeyPadDevice.c`（新建）

全局数据：

```c
stKeyPadDeviceParamTdf	astKeyPadDeviceParam[KEYPAD_DEV_NUM];
stKeyPadPinTdf		astKeyPadRowPin[KEYPAD_ROW_NUM];
stKeyPadPinTdf		astKeyPadColPin[KEYPAD_COL_NUM];
```

静态函数 `static void s_vKeyPadDebounceUpdate(emKeyPadDevNumTdf emDevNum, emKeyPadStatusTdf emRawStatus)`：逻辑与 `BtnDevice.c` 的 `vBtnDeviceScanExecute` 完全一致（一致则清零计数；不一致则累加，达阈值后更新稳定状态并生成 Press/Release 事件）。

核心扫描函数 `void vKeyPadDevicePeriodExecute(void)`：

```c
uint8_t ucColIndex, ucRowIndex;
emKeyPadDevNumTdf emDevNum;
GPIO_PinState emRowLevel;
emKeyPadStatusTdf emRawStatus;

//逐列扫描：当前列拉低，其余列保持开漏高（由CubeMX内部上拉保持空闲高）
for(ucColIndex = 0; ucColIndex < KEYPAD_COL_NUM; ucColIndex++)
{
	HAL_GPIO_WritePin(astKeyPadColPin[ucColIndex].pstGpioBase,
			astKeyPadColPin[ucColIndex].usGpioPin, GPIO_PIN_RESET);

	//列拉低后等待电平稳定（约10us@72MHz），开漏+上拉驱动能力弱，不能立即读
	for(ucRowIndex = 0; ucRowIndex < 20; ucRowIndex++)
	{
		__NOP();
	}

	//读该列下所有行：行被上拉，读到低即该键按下
	for(ucRowIndex = 0; ucRowIndex < KEYPAD_ROW_NUM; ucRowIndex++)
	{
		emRowLevel = HAL_GPIO_ReadPin(astKeyPadRowPin[ucRowIndex].pstGpioBase,
				astKeyPadRowPin[ucRowIndex].usGpioPin);

		emRawStatus = (emRowLevel == GPIO_PIN_RESET) ? emKeyPadStatus_Pressed : emKeyPadStatus_Released;

		emDevNum = (emKeyPadDevNumTdf)(ucRowIndex * KEYPAD_COL_NUM + ucColIndex);

		s_vKeyPadDebounceUpdate(emDevNum, emRawStatus);
	}

	//释放当前列（开漏输出高 = 高阻，由上拉拉高）
	HAL_GPIO_WritePin(astKeyPadColPin[ucColIndex].pstGpioBase,
			astKeyPadColPin[ucColIndex].usGpioPin, GPIO_PIN_SET);
}
```

其余函数：

- `vKeyPadDeviceInit`：`memcpy` 静态参数（长度 `sizeof(stKeyPadStaticParamTdf) / sizeof(uint8_t)`）
- `vKeyPadDeviceRowPinInit` / `vKeyPadDeviceColPinInit`：按索引 `memcpy` 进对应行/列引脚表
- `c_pstGetKeyPadDeviceParam`：返回 `&astKeyPadDeviceParam[emDevNum]`
- `emKeyPadGetStatus`：返回稳定状态
- `emKeyPadGetEvent`：读值后置 `emKeyPadEvent_None` 再返回

注意：仅 Device 层包含 `stm32f1xx_hal.h`；`__NOP()` 来自 CMSIS core 头（`stm32f1xx_hal.h` 已间接包含）。

### 3.4 `User/Operation/KeyPad_Operation.h`（新建）

```c
#ifndef _KEYPAD_OPERATION_H_
#define _KEYPAD_OPERATION_H_

#include "KeyPadDevice.h"

void vKeyPadInitAll(void);
void vKeyPadExecuteAll(void);
emKeyPadDevNumTdf emKeyPadGetPressEvent(void);
uint32_t ulKeyPadGetPressedMask(void);

#endif
```

### 3.5 `User/Operation/KeyPad_Operation.c`（新建）

顶部两张 `static const` 引脚表，**改引脚只动这里**：

```c
///	@brief			行引脚表（上拉输入，由CubeMX配置）
static const stKeyPadPinTdf s_astKeyPadRowPin[KEYPAD_ROW_NUM] =
{
	{GPIOA, GPIO_PIN_2},
	{GPIOA, GPIO_PIN_3},
	{GPIOA, GPIO_PIN_4},
	{GPIOA, GPIO_PIN_5},
};

///	@brief			列引脚表（开漏输出，由CubeMX配置）
static const stKeyPadPinTdf s_astKeyPadColPin[KEYPAD_COL_NUM] =
{
	{GPIOA, GPIO_PIN_6},
	{GPIOA, GPIO_PIN_7},
	{GPIOB, GPIO_PIN_0},
	{GPIOB, GPIO_PIN_1},
};
```

`vKeyPadInitAll`：

1. 循环把 `s_astKeyPadRowPin` / `s_astKeyPadColPin` 登记进 Device 层
2. 所有列写高（空闲态），避免上电瞬间误判
3. 双重循环填 16 个按键的静态参数：`ucRowIndex`、`ucColIndex`、`usDebounceThreshold = KEYPAD_DEBOUNCE_THRESHOLD`，调用 `vKeyPadDeviceInit`

`vKeyPadExecuteAll`：直接调用 `vKeyPadDevicePeriodExecute()`（整屏一次扫描，不按键号循环）。

`emKeyPadGetPressEvent`：遍历 `emKeyPadDevNum0 ~ KEYPAD_DEV_NUM`，`emKeyPadGetEvent` 为 `Press` 即返回，否则返回哨兵 `KEYPAD_DEV_NUM`（与 `Btn_Operation.c` 风格一致）。

`ulKeyPadGetPressedMask`：遍历 16 键，`emKeyPadGetStatus()==emKeyPadStatus_Pressed` 时置 `bit(ucIndex)`，返回 `uint32_t`。**这是组合键扩展点**：后续做组合键时在 Task/业务层基于该掩码判断，无需改 Device 层。

### 3.6 `User/Task/KeyPad_Task.h` / `.c`（新建）

```c
//	.h
#ifndef _KEYPAD_TASK_H_
#define _KEYPAD_TASK_H_

void vKeyPadTask(void *argument);

#endif
```

```c
//	.c
#include "KeyPad_Task.h"
#include "KeyPad_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				矩阵键盘扫描任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期扫描+消抖；事件由业务层调用
///						emKeyPadGetPressEvent / ulKeyPadGetPressedMask 轮询获取
void vKeyPadTask(void *argument)
{
	(void)argument;

	vKeyPadInitAll();

	while(1)
	{
		vKeyPadExecuteAll();
		vTaskDelay(10);
	}
}
```

### 3.7 需用户在 CubeMX / Keil 侧执行的动作（规范 09 禁止我改动）

1. **CubeMX 引脚配置**
   - PA2、PA3、PA4、PA5 → `GPIO_Input`，Pull-up
   - PA6、PA7、PB0、PB1 → `GPIO_Output`，**Output Open Drain**，Pull-up，Low speed，初始电平 **High**
2. **CubeMX FreeRTOS 配置**：新增任务 `KeyPadTask`，优先级 `osPriorityNormal(24)`，栈 `128 Words`，入口 `vKeyPadTask`，`As weak`，`Dynamic`，无参数
3. **CubeMX FreeRTOS 堆**：`TOTAL_HEAP_SIZE` 由 `3072` 调到 `4096`（`FreeRTOSConfig.h` 在 `Core/Inc`，属禁区）
4. **Keil**：把 `KeyPadDevice.c`、`KeyPad_Operation.c`、`KeyPad_Task.c` 加入 User 分组并编译
5. 生成代码后确认 `main.c` 中新任务为 `__weak void vKeyPadTask(void *argument)`（证明 weak 覆盖链路正常）

---

## 四、假设与决策

| 项 | 决策 | 理由 |
|----|------|------|
| 行列数量 | 宏 `KEYPAD_ROW_NUM` / `KEYPAD_COL_NUM` 可配，默认 4×4 | 用户要求"行数列数自由设置" |
| 引脚分配 | 行 PA2~PA5、列 PA6~PB1 占位 | 均为空闲引脚，改起来只动 Operation 层两张表 |
| 引脚模式 | 行上拉输入、列开漏输出 | 用户明确要求；开漏输出保证多列并联时不硬短路，空闲靠内部上拉 |
| 扫描方式 | 逐列拉低 + 逐行读取（整屏一次调用） | 单键场景无鬼键问题；组合键需掩码另行处理 |
| 消抖 | 以任务周期为基准，阈值 2（≈20ms） | 与既有 `BtnDevice` 一致 |
| 事件机制 | Operation 层轮询（`emKeyPadGetPressEvent` / `ulKeyPadGetPressedMask`） | 用户选定；不新增队列，省堆、不污染 `BtnMsgQueue` |
| 组合键 | 暂不实现，仅暴露位掩码接口 | 用户要求"留接口后续扩展" |
| 设备粒度 | 每个按键一个设备号（16 个） | 复用既有"设备号 + 全局参数数组"模式，便于逐键消抖 |

**已知限制**（写入代码注释，不在本次修复）：多键跨行列同时按下时存在鬼键（ghosting），需加二极管或改为逐行/逐列全扫描矩阵法才能消除。

---

## 五、验证步骤

1. Keil 编译，0 error / 0 warning（重点看 `__NOP()` 与 `GPIO_PIN_RESET` 是否正常解析）
2. 单步/断点在 `User/Task/KeyPad_Task.c` 的 `while(1)` 内（**不要**打在 `main.c` 的 weak 空壳）
3. Watch 窗口观察 `astKeyPadDeviceParam[]`：按键按下后对应键 `emCurrentStatus` 变 `emKeyPadStatus_Pressed`，`emEvent` 出现 `emKeyPadEvent_Press`
4. 依次按 16 个键，确认 `emKeyPadGetPressEvent()` 返回的键号与物理行列位置一致（`row*4+col`）
5. 调用 `ulKeyPadGetPressedMask()`，按住键时对应 bit 为 1，松手回 0
6. 长按不重复触发（事件只出一次），快速点按不漏键
7. 确认 LED（PC13）、4 个独立按键、OLED（PB8/PB9）功能不受影响
8. 若 `KeyPadTaskHandle == NULL` → 检查堆是否已改 4096；若任务不进循环 → 检查 `main.c` 是否已注册任务

---

## 六、风险与应对

| 风险 | 应对 |
|------|------|
| 堆不足导致任务创建失败（3072 余量紧张） | 已在步骤 3.7 要求调到 4096；验证步骤 8 兜底检查 |
| 开漏上拉驱动弱，列拉低后行列电平未稳定就读取 → 误判 | 扫描内已加约 10µs `__NOP()` 稳定延时；若仍有误判，把循环次数 20 上调 |
| 行列数改动后忘记同步设备号枚举 | `KeyPadDevice.h` 枚举注释写明；`KEYPAD_DEV_NUM` 由宏推导，数组维度自动跟随 |
| 引脚占用与其他外设冲突 | 已核对 `.ioc`：PA2~PA7、PB0/PB1 均未使用 |
| 鬼键（多键同按） | 已在文档与代码注释标注限制，属后续增强项 |
| 任务优先级与 LedTask 相同，扫描耗时不长 | 单次扫描 4 列 × ~10µs ≈ 40µs，10ms 周期占用极低，无实时性风险 |
