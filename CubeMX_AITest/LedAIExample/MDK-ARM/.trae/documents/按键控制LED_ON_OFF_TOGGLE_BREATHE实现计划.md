# 按键控制 LED（ON/OFF/TOGGLE/BREATHE）实现计划

## 1. 需求与映射

四个独立按键分别控制板载 LED（PC13）的四种动作：

| 按键 | 引脚 | 动作 |
|------|------|------|
| BUTTON_UP | PA1 | LED **ON** |
| BUTTON_DOWN | PA0 | LED **OFF** |
| BUTTON_ENTER | PC15 | LED **TOGGLE** |
| BUTTON_CANCEL | PC14 | LED **BREATHE**（呼吸） |

**数据流（三任务 + 一队列）**：

```
[BtnTask 按键获取任务] --10ms扫描消抖--> 检测到Press事件
        |
        |  osMessageQueuePut(BtnMsgQueue, 按键ID)
        v
[BtnMsgQueue 按键消息队列]  <-- FreeRTOS 消息队列
        |
        |  osMessageQueueGet(BtnMsgQueue, ...)
        v
[BtnProcessTask 按键处理任务] --映射按键->动作--> vLedSetAction()
        |
        v
[LedTask LED任务] --按emMode执行--> Static(保持) / Breath(呼吸调制)
```

## 2. 现状分析（Repository Research）

| 模块 | 现状 | 需改动 |
|------|------|--------|
| [BtnDevice.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/BtnDevice.c) | 周期扫描+消抖，产生 Press/Release 事件，`emBtnGetEvent` 读清 | 不改 |
| [Btn_Operation.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Operation/Btn_Operation.c) | `vBtnExecuteAll` 只扫描，无事件采集 | **新增** `emBtnGetPressEvent`（采集按下事件的按键号） |
| [Btn_Task.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Task/Btn_Task.c) | 10ms 周期调用 `vBtnExecuteAll` | **改**：扫描后采集 Press 事件，发送到 `BtnMsgQueue` |
| [LedDevice.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/LedDevice.c) | 有 `emLedMode_Static/Blink/Breath`，`vLedDevicePeriodExecute` 按模式执行，`vLedOn/Off/Toggle` | **新增** `vLedSetMode`（设置模式 + 复位计数 + 配呼吸参数） |
| [Led_Operation.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Operation/Led_Operation.c) | `vLedExecuteAll` 直接 `vLedToggle`，1000ms 翻转 | **改**：新增 `vLedSetAction`；`vLedExecuteAll` 改为 `vLedDevicePeriodExecute` |
| [Led_Task.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Task/Led_Task.c) | `void vLedTask(void)`（**签名与 CubeMX 生成的 `void *argument` 不一致**），`vTaskDelay(1000)` | **改**：修正签名为 `void vLedTask(void *argument)`；延时改 10ms |
| `BtnProcessTask` | 不存在 | **新增** `BtnProcess_Task.c/.h`：阻塞读队列 → 映射按键→动作 → `vLedSetAction` |
| `BtnMsgQueue` | 不存在 | **CubeMX 新增** 消息队列 |

**关键事实**：
- `emLedMode_Static` 在 `vLedDevicePeriodExecute` 的 `switch` 中无对应 case（走 `default`），即静态模式下 LED 保持 `vLedOn/Off/Toggle` 设置的状态不变。
- `vLedDeviceBreathExecute` 依赖 `ulOnCountThreshold`、`ulOffCountThreshold`、`ulBreathPeriod` 均非零（存在除法），切换到 Breath 模式时必须初始化这三个参数。
- `BtnMsgQueueHandle` 由 CubeMX 在 `main.c` 生成全局变量，User 代码用 `extern` 引用。

## 3. CubeMX 配置清单（由你操作）

打开 `.ioc` → Middleware → FREERTOS → **Tasks and Queues**：

### 3.1 新增消息队列（Add → Queue）

| 配置项 | 填写值 | 说明 |
|--------|--------|------|
| **Queue Name** | `BtnMsgQueue` | 生成 `BtnMsgQueueHandle` / `BtnMsgQueue_attributes` |
| **Queue Size** | `8` | 最多缓存 8 条消息 |
| **Queue Item Size** | `4` | 单位：字节，存 `uint32_t`（按键号） |
| **Allocation** | `Dynamic` | 动态分配 |

### 3.2 新增按键处理任务（Add → Task）

| 配置项 | 填写值 | 说明 |
|--------|--------|------|
| **Task Name** | `BtnProcessTask` | 生成 `BtnProcessTaskHandle` |
| **Priority** | `osPriorityNormal`（24） | 与 LedTask 同级 |
| **Stack Size (Words)** | `128` | 512 字节，任务体仅收队列+调函数 |
| **Entry Function** | `vBtnProcessTask` | 须与 `BtnProcess_Task.h` 逐字一致 |
| **Code Generation Option** | `As weak` | User 层覆盖 |
| **Parameter** | `NULL` | |
| **Allocation** | `Dynamic` | |
| **Buffer Name / Control Block Name** | `NULL` | 动态分配无需填 |

### 3.3 不需要改动

- GPIO：PA0/PA1/PC14/PC15（Input+Pull-up）、PC13（Output）已就绪。
- LedTask、BtnTask：保持现有配置。
- FreeRTOS 内核参数：`configTOTAL_HEAP_SIZE=3072` 足够（队列 32B + 新任务栈 512B + TCB）。

### 3.4 操作步骤

1. 按 3.1、3.2 添加队列与任务 → **Generate Code**。
2. Keil 中将 `User/Task/BtnProcess_Task.c` **Add** 进 target。
3. 编译验证。

## 4. 修改方案（Files and Modules）

> 所有改动仅限 `User/` 目录（规范 09）。

### 4.1 `User/Device/LedDevice.h` — 新增模式设置接口

新增声明：
```c
void vLedSetMode(emLedDevNumTdf emDevNum, emLedModeTdf emMode);
```

### 4.2 `User/Device/LedDevice.c` — 实现 `vLedSetMode`

```c
void vLedSetMode(emLedDevNumTdf emDevNum, emLedModeTdf emMode)
{
    astLedDeviceParam[emDevNum].stRunningParam.emMode = emMode;
    astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount = 0;

    if(emMode == emLedMode_Breath)
    {
        // 闪烁步长周期 = 10 次计数 × 10ms = 100ms
        astLedDeviceParam[emDevNum].stRunningParam.ulOnCountThreshold  = 5;
        astLedDeviceParam[emDevNum].stRunningParam.ulOffCountThreshold = 5;
        // 呼吸总周期 = 100 次计数 × 10ms = 1s，分 10 个闪烁步长
        astLedDeviceParam[emDevNum].stRunningParam.ulBreathPeriod = 100;
    }
}
```

### 4.3 `User/Operation/Led_Operation.h` — 新增动作枚举与接口

```c
typedef enum
{
    emLedAction_On       = 0,
    emLedAction_Off,
    emLedAction_Toggle,
    emLedAction_Breath,
}emLedActionTdf;

void vLedSetAction(emLedDevNumTdf emDevNum, emLedActionTdf emAction);
```

### 4.4 `User/Operation/Led_Operation.c` — 实现 `vLedSetAction` + 改 `vLedExecuteAll`

```c
void vLedSetAction(emLedDevNumTdf emDevNum, emLedActionTdf emAction)
{
    switch(emAction)
    {
        case emLedAction_On:
            vLedSetMode(emDevNum, emLedMode_Static);
            vLedOn(emDevNum);
            break;
        case emLedAction_Off:
            vLedSetMode(emDevNum, emLedMode_Static);
            vLedOff(emDevNum);
            break;
        case emLedAction_Toggle:
            vLedSetMode(emDevNum, emLedMode_Static);
            vLedToggle(emDevNum);
            break;
        case emLedAction_Breath:
            vLedSetMode(emDevNum, emLedMode_Breath);
            break;
        default:
            break;
    }
}

void vLedExecuteAll(void)
{
    vLedDevicePeriodExecute(emLedDevNum0);   // 按模式执行（Static 保持 / Breath 调制）
}
```

### 4.5 `User/Operation/Btn_Operation.h` — 新增事件采集接口

```c
emBtnDevNumTdf emBtnGetPressEvent(void);
```

### 4.6 `User/Operation/Btn_Operation.c` — 实现 `emBtnGetPressEvent`

```c
emBtnDevNumTdf emBtnGetPressEvent(void)
{
    emBtnDevNumTdf emDevNum;

    for(emDevNum = emBtnDevNum0; emDevNum < BTN_DEV_NUM; emDevNum++)
    {
        if(emBtnGetEvent(emDevNum) == emBtnEvent_Press)
        {
            return emDevNum;
        }
    }
    return BTN_DEV_NUM;   // 无按键按下
}
```

> 注：每次调用只读清第一个 Press 事件；BtnTask 中循环调用直到返回 `BTN_DEV_NUM`，可处理同周期多键按下。

### 4.7 `User/Task/Btn_Task.c` — 扫描后发队列

```c
#include "Btn_Task.h"
#include "Btn_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t BtnMsgQueueHandle;

void vBtnTask(void *argument)
{
    (void)argument;
    emBtnDevNumTdf emBtn;

    vBtnInitAll();

    while(1)
    {
        vBtnExecuteAll();

        // 采集所有按下事件，逐条入队（非阻塞）
        while((emBtn = emBtnGetPressEvent()) < BTN_DEV_NUM)
        {
            uint32_t ulMsg = (uint32_t)emBtn;
            osMessageQueuePut(BtnMsgQueueHandle, &ulMsg, 0, 0);
        }

        vTaskDelay(10);
    }
}
```

### 4.8 `User/Task/Led_Task.c` + `Led_Task.h` — 修正签名 + 改周期

`Led_Task.h`：
```c
void vLedTask(void *argument);
```

`Led_Task.c`：
```c
#include "Led_Task.h"
#include "Led_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief		LED任务
///	@param		argument	:	CubeMX传入参数（未使用）
///	@note		10ms周期驱动LED模式执行（呼吸灯需高频）
void vLedTask(void *argument)
{
    (void)argument;

    vLedInitAll();

    while(1)
    {
        vLedExecuteAll();
        vTaskDelay(10);
    }
}
```

> 修正签名是为了与 CubeMX 在 [main.c](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/Core/Src/main.c#L65) 生成的 `void vLedTask(void *argument)` 声明一致，确保 `__weak` 覆盖正确。

### 4.9 新增 `User/Task/BtnProcess_Task.h`

```c
#ifndef _BTN_PROCESS_TASK_H_
#define _BTN_PROCESS_TASK_H_

void vBtnProcessTask(void *argument);

#endif
```

### 4.10 新增 `User/Task/BtnProcess_Task.c`

```c
#include "BtnProcess_Task.h"
#include "Led_Operation.h"
#include "Project_Config.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t BtnMsgQueueHandle;

///	@brief		按键处理任务
///	@param		argument	:	CubeMX传入参数（未使用）
///	@note		阻塞读按键消息队列，映射按键到LED动作
void vBtnProcessTask(void *argument)
{
    (void)argument;
    uint32_t ulMsg;

    while(1)
    {
        osMessageQueueGet(BtnMsgQueueHandle, &ulMsg, NULL, osWaitForever);

        switch((emBtnDevNumTdf)ulMsg)
        {
            case BUTTON_UP:     vLedSetAction(LED_BOARD, emLedAction_On);      break;
            case BUTTON_DOWN:   vLedSetAction(LED_BOARD, emLedAction_Off);     break;
            case BUTTON_ENTER:  vLedSetAction(LED_BOARD, emLedAction_Toggle);  break;
            case BUTTON_CANCEL: vLedSetAction(LED_BOARD, emLedAction_Breath);  break;
            default:            break;
        }
    }
}
```

> `LED_BOARD` = `emLedDevNum0`（PC13 板载 LED），定义于 [LedDevice.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/LedDevice.h#L58)。

## 5. 实现步骤（Dependency-ordered）

1. **Device 层**：`LedDevice.h` 加 `vLedSetMode` 声明 → `LedDevice.c` 实现。
2. **Operation 层**：
   - `Led_Operation.h` 加 `emLedActionTdf` + `vLedSetAction` 声明 → `Led_Operation.c` 实现 + 改 `vLedExecuteAll`。
   - `Btn_Operation.h` 加 `emBtnGetPressEvent` 声明 → `Btn_Operation.c` 实现。
3. **Task 层**：
   - 改 `Led_Task.h/.c`（签名 + 10ms 周期）。
   - 改 `Btn_Task.c`（发队列）。
   - 新建 `BtnProcess_Task.h/.c`。
4. **CubeMX**（你操作）：加 `BtnMsgQueue` 队列 + `BtnProcessTask` 任务 → Generate Code。
5. **Keil**（你操作）：Add `BtnProcess_Task.c` → 编译。

## 6. 依赖与注意事项

- **分层约束**（规范 01）：Task 层只调 Operation（`emBtnGetPressEvent`、`vLedSetAction`），不直接碰 Device；`osMessageQueue` 属 RTOS 原语，留在 Task 层。
- **队列句柄**：`BtnMsgQueueHandle` 由 CubeMX 生成于 `main.c`，User 文件用 `extern osMessageQueueId_t BtnMsgQueueHandle;` 引用，名称必须与 CubeMX 队列名一致。
- **LedTask 签名修正**：原 `void vLedTask(void)` 与生成代码不一致，本次一并修正为 `void vLedTask(void *argument)`，属于必要修复（非范围蔓延）。
- **呼吸参数**：`ulBreathPeriod=100`、`ulOn/OffCountThreshold=5`，对应 10ms 任务周期下 1 秒呼吸周期、10 个亮度步长。
- **静态变量**：`vLedDeviceBreathExecute` 内部 `static s_ulBreathCount` 跨模式切换不复位，呼吸相位会延续——单 LED 场景下无功能影响。

## 7. 验证

1. **编译**：Keil 0 error 0 warning；`.map` 出现 `BtnMsgQueueHandle`、`BtnProcessTaskHandle`、`BtnProcessTask_attributes`。
2. **静态检查**：Grep 确认无 4 空格缩进（新文件 Tab）、无行尾空白、guard 配对。
3. **运行验证**：
   - 按 UP（PA1）→ LED 常亮。
   - 按 DOWN（PA0）→ LED 熄灭。
   - 按 ENTER（PC15）→ LED 翻转。
   - 按 CANCEL（PC14）→ LED 进入呼吸（1 秒周期渐亮渐暗）。
   - 呼吸中按 UP/DOWN → 立即退出呼吸，保持常亮/灭。
4. **队列验证**：调试时观察 `BtnMsgQueue` 消息数，快速连按时队列缓存正常，无丢失（队列满时 `osMessageQueuePut` 非阻塞返回错误，不影响扫描）。

## 8. 风险与处理

| 风险 | 处理 |
|------|------|
| `BtnMsgQueueHandle` 名称与 CubeMX 不一致 → 链接错误 | 生成后核对 `main.c` 中句柄名，确保 `extern` 逐字一致 |
| 呼吸模式参数未初始化 → 除零 | `vLedSetMode` 在 Breath 分支强制设置三个阈值 |
| LedTask 原签名 `(void)` 与生成代码冲突 | 本次修正为 `(void *argument)`，与 BtnTask 一致 |
| 队列满丢键 | 队列深度 8 足够；非阻塞发送不阻塞扫描任务，丢键可接受（人工按键频率远低于处理速度） |
