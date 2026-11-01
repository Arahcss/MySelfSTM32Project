# 按键 Task 层实现计划

## 1. 概述（Summary）

新增 `User/Task/Btn_Task.c` / `Btn_Task.h`，实现按键任务函数 `vBtnTask`：初始化按键 → 10ms 周期循环调用 `vBtnExecuteAll` 驱动扫描消抖。任务注册由 CubeMX 生成（你操作），本计划只交付 User 层两个文件。

**范围限定**：仅新增 2 个文件，不改动任何现有文件。

## 2. 现状分析（Current State Analysis）

| 项目 | 现状 |
|------|------|
| 任务注册机制 | `.ioc` 中 `FREERTOS.Tasks01=LedTask,24,128,vLedTask,As weak,...`；CubeMX 在 [main.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/Core/Src/main.c#L122-L123) 生成 `osThreadNew(vLedTask, NULL, &LedTask_attributes)` 与 `__weak void vLedTask(void *argument)` |
| 现有任务实现 | [Led_Task.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Task/Led_Task.c)：Init → `while(1)` → ExecuteAll → `vTaskDelay(周期)` 骨架 |
| Operation 层 | `vBtnInitAll(void)` / `vBtnExecuteAll(void)` 已就绪（消抖阈值 2，按 10ms 周期设计 → 20ms 消抖） |
| 按键任务注册 | `.ioc` 中**不存在**，需你在 CubeMX 添加 |

## 3. CubeMX FreeRTOS 任务配置表

打开 `.ioc` → Middleware → FREERTOS → Tasks and Queues → **Add**，按下表填写：

| 配置项（CubeMX 界面） | 填写值 | 说明 |
|----------------------|--------|------|
| **Task Name** | `BtnTask` | 生成 `BtnTaskHandle` / `BtnTask_attributes` |
| **Priority** | `osPriorityNormal` | 与 LedTask 同级（数值 24） |
| **Stack Size (Words)** | `128` | 即 512 字节，任务体小，与 LedTask 相同 |
| **Entry Function** | `vBtnTask` | 须与 `Btn_Task.h` 声明逐字一致 |
| **Code Generation Option** | `As weak` | 生成 `__weak` 默认实现，由 User 层覆盖 |
| **Parameter** | `NULL` | 不传参 |
| **Allocation** | `Dynamic` | 与 LedTask 一致 |
| **Buffer Name** | `NULL` | 动态分配时无需填 |
| **Control Block Name** | `NULL` | 动态分配时无需填 |

对照：现有 LedTask 的 `.ioc` 记录为 `FREERTOS.Tasks01=LedTask,24,128,vLedTask,As weak,NULL,Dynamic,NULL,NULL`，新任务生成后应出现一行 `FREERTOS.Tasks02=BtnTask,24,128,vBtnTask,As weak,NULL,Dynamic,NULL,NULL`。

**Generate Code** 后 `main.c` 会自动生成：`BtnTask_attributes`、`osThreadNew(vBtnTask, NULL, &BtnTask_attributes)` 和 `__weak void vBtnTask(void *argument)`。

**Keil**：将 `User/Task/Btn_Task.c` 手动 Add 进 target（`.uvprojx` 属规范 09 禁止范围）。

## 4. 修改方案（Proposed Changes）

### 4.1 新增 `User/Task/Btn_Task.h`

```c
#ifndef _BTN_TASK_H_
#define _BTN_TASK_H_


void vBtnTask(void *argument);


#endif

```

### 4.2 新增 `User/Task/Btn_Task.c`

```c
#include "Btn_Task.h"
#include "Btn_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				按键任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期驱动按键扫描消抖
void vBtnTask(void *argument)
{
	(void)argument;

	vBtnInitAll();

	while(1)
	{
		vBtnExecuteAll();
		vTaskDelay(10);
	}
}
```

要点：

- 骨架与 `Led_Task.c` 完全一致：`vBtnInitAll()` 一次性初始化 → `while(1)` 循环 → `vBtnExecuteAll()` → `vTaskDelay(10)`。
- 原型 `void vBtnTask(void *argument)` 与 CubeMX 生成声明逐字一致，确保 weak 覆盖类型安全。
- `(void)argument;` 显式消化未使用参数，消除编译警告。
- Task 层只调 Operation 接口，不触碰 Device/HAL（规范 01 分层）。

## 5. 假设与决策（Assumptions & Decisions）

1. **周期 10ms**：与 Operation 层消抖设计匹配（阈值 2 → 20ms 消抖）；`vTaskDelay(10)` 按 `configTICK_RATE_HZ=1000` 计，CubeMX 默认即 1000Hz。
2. **栈 128 Words / 优先级 Normal**：任务体仅两次函数调用 + 延时，需求与 LedTask 同级，直接沿用其配置。
3. **风格按规范执行**：Tab 缩进、Doxygen `///` 块、guard `_BTN_TASK_H_`（新文件一律合规，不沿 Led_Task.c 现存的 4 空格缩进）。
4. **不动 Led_Task**：其现存签名/风格问题属既有状态，本次不涉及。
5. **`while(1)` 无空格**：与 Led_Task.c 现状及既有代码保持一致。

## 6. 验证（Verification）

1. Grep 扫描两个新文件：无 4 空格缩进（`^    \S`）、无行尾空白；guard `_BTN_TASK_H_` 配对。
2. 原型核对：`Btn_Task.h` 声明与 CubeMX 重新生成后 `main.c` 中 `void vBtnTask(void *argument);` 完全一致。
3. 确认未改动任何现有文件。
4. 编译验证（需你完成第 3 节后）：Keil 编译应 0 error 0 warning；`.map` 中应出现 `BtnTask_attributes` / `BtnTaskHandle`，且 `main.o` 的 weak `vBtnTask` 被 `btn_task.o` 覆盖。
5. 运行验证：调试状态下按 UP 键，观察 `astBtnDeviceParam[emBtnDevNum0].stRunningParam.emCurrentStatus` 变为 `emBtnStatus_Pressed`，松开产生 `emBtnEvent_Release`。
