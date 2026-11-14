# 多级菜单与应用框架 Spec

## Why

计算器、LED、矩阵键盘、OLED 均已分别打通，但彼此是"平铺"的两条独立链路（KeyPad→CalcTask→显示、板载键→BtnProcessTask→LED），没有统一入口，也没有地方挂新功能。本次引入多级菜单与应用框架，把现有功能收进菜单，并让"新增一个应用"变成只写一个文件 + 注册一行的事。

## What Changes

- 新增应用框架：统一应用接口（进入/退出/按键/周期 tick/渲染）+ 应用注册表，贪吃蛇等新功能按同一接口接入
- 新增多级菜单：MAIN → LED CONTROL → SINGLE LED / ALL LED 三级树，板载 4 键（UP/DOWN/ENTER/CANCEL）导航
- 计算器由"独立任务"改为"应用模块"：保留 `Calculator_Operation` 状态机与 20 键映射不变，新增 `CalcApp_Operation` 适配层
- LED 控制改为菜单应用：单灯页（选灯后循环切换 亮/灭/翻转/呼吸）、全部页（全部点亮/熄灭/翻转/呼吸）
- 新增统一输入事件队列：板载 4 键与矩阵键盘键号编码为同一个 `uint32_t` 事件，菜单任务单点消费
- 显示帧统一为 tag 联合体（计算器帧 / 列表帧），OLED 任务按帧类型分发渲染；新增列表帧渲染（标题 + 3 行 + 选中高亮）
- **BREAKING**：删除 `User/Task/Calc_Task.c/.h`、`User/Task/BtnProcess_Task.c/.h`（其功能分别被计算器应用、LED 应用取代），需在 CubeMX 删除 `CalcTask`、`BtnProcessTask`，并在 Keil target 移除这两个源文件

## Impact

- Affected specs：`add-integer-calculator`（计算器的按键来源由独立任务改为菜单分发，键位与运算行为不变）；矩阵键盘驱动（`KeyPadMsgQueue` 语义扩展为统一输入队列，键号编码方式保留）
- Affected code：
  - 新增：`User/Operation/Input_Operation.h`、`App_Operation.c/.h`、`Menu_Operation.c/.h`、`CalcApp_Operation.c/.h`、`LedApp_Operation.c/.h`、`GameApp_Operation.c/.h`、`User/Task/Menu_Task.c/.h`
  - 修改：`User/Operation/OLED_Operation.c/.h`、`User/Task/OLED_Task.c/.h`、`User/Task/Btn_Task.c`、`User/Task/KeyPad_Task.c`、`User/Project_Config.h`
  - 删除：`User/Task/Calc_Task.c/.h`、`User/Task/BtnProcess_Task.c/.h`
  - 禁区不变：`Core/`、`.ioc`、`User/Device/`、`MDK-ARM/*.uvprojx`

## ADDED Requirements

### Requirement: 多级菜单导航

系统 SHALL 提供三级菜单树，用板载 4 键导航，菜单状态下矩阵键盘事件被忽略：

| 层级 | 菜单 | 选项 |
| --- | --- | --- |
| 0 | `MAIN` | `CALCULATOR`（应用）、`LED CONTROL`（子菜单）、`GAME`（子菜单） |
| 1 | `LED CONTROL` | `SINGLE LED`（子菜单）、`ALL LED`（子菜单） |
| 2 | `SINGLE LED` | `LED0` ~ `LED8`（应用，参数=灯号） |
| 2 | `ALL LED` | `ALL ON`、`ALL OFF`、`ALL TOGGLE`、`ALL BREATH`（应用，参数=动作） |
| 1 | `GAME` | `SNAKE`（占位应用） |

- 导航语义：UP/DOWN 移动选中项并循环、ENTER 进入子菜单或启动应用、CANCEL 返回上一级（在 MAIN 根菜单下 CANCEL 无动作）
- 菜单项超过 3 项时列表窗口按选中项滚动，每级菜单各自记住选中位置，返回上级后回到原位置
- 每次导航动作后刷新一次屏幕

#### Scenario: 进入并返回

- **WHEN** 在 MAIN 选中 `LED CONTROL` 按 ENTER，再选中 `SINGLE LED` 按 ENTER，选中 `LED3` 按 ENTER
- **THEN** 屏幕依次显示 LED CONTROL 菜单、SINGLE LED 灯列表、LED3 控制页；按 CANCEL 逐级返回且各级选中项保持

#### Scenario: 列表滚动

- **WHEN** 在 `SINGLE LED`（9 项）中连续按 DOWN 至第 5 项
- **THEN** 窗口始终显示 3 项并随选中项滚动，选中项始终可见且高亮

### Requirement: 统一应用接口与注册表

系统 SHALL 用函数表定义应用，菜单与显示层不感知具体应用实现：

```c
typedef struct
{
	const char *c_pcName;								//	应用名（调试用）
	void (*vEnter)(uint8_t ucParam);					//	进入应用，ucParam 来自菜单项
	void (*vExit)(void);								//	退出应用
	void (*vHandleKey)(emInputKeyTdf emKey);			//	处理归一化按键
	void (*vTick)(void);								//	周期回调，可为NULL（游戏类扩展点）
	const stDisplayMsgTdf *(*c_pstRender)(void);		//	返回当前显示帧
}stAppInterfaceTdf;
```

- 注册表为 `App_Operation.c` 内的静态常量数组，`c_pstGetAppInterface(uint8_t ucAppIndex)` 按下标取接口
- 应用模式下 `vMenuExecuteTick()` 周期调用当前应用的 `vTick`（为 NULL 则跳过，不产生刷新）
- 应用模式下 CANCEL 由菜单统一拦截：先调 `vExit` 再回到进入前的菜单，应用不必自己处理返回
- 新增应用的最小步骤 MUST 只有 4 步：①按接口实现 5 个函数（`vTick` 可空）②在注册表加一行 ③在菜单项表加一个 `App` 项 ④若需周期刷新则实现 `vTick`
- 贪吃蛇本次只做占位应用（显示 `COMING SOON`），不改动游戏逻辑；图形类应用如需逐像素绘制，扩展点为"新增帧类型 + union 成员 + `vOledShowFrame` 增加分支"

#### Scenario: 新增应用不改框架代码

- **WHEN** 开发者新增一个应用文件并在注册表、菜单项表各加一行
- **THEN** 该应用出现在菜单中且可进入/退出，无需修改 `Menu_Task`、`Menu_Operation`、`OLED_Task`

### Requirement: 计算器应用化

计算器 SHALL 作为菜单应用运行，内部状态机与键位定义保持不变：

- `CalcApp_Operation` 只做适配：`vEnter` 调 `vCalcInit()`，`vHandleKey` 把 `emInputKey_KeyPadBase + 键号` 换算成键号后调 `vCalcInputKey()`，`c_pstRender` 把 `c_pstGetCalcDisplayMsg()` 包装为计算器帧
- 板载 UP/DOWN/ENTER 在计算器内忽略；CANCEL 退出应用回 MAIN
- 20 键定义、0~9999 输入上限、连算行为、除零显示 `DIV/0` 全部不变
- 显示布局保持不变：标题 `INT CALC 0-9999`（6×12）、表达式行（8×16）、数值行（12×24）

#### Scenario: 计算器在菜单内正常工作

- **WHEN** MAIN → `CALCULATOR` → 矩阵键盘依次按 1、2、+、3、4、=
- **THEN** 显示 `12+34` 与 `=46`；按 CANCEL 返回 MAIN，再次进入计算器显示初始 `0`

### Requirement: LED 控制应用

LED 应用 SHALL 通过 `vLedSetAction()` 驱动 LED，不新增 Device 层代码：

- 单灯页（`LED0`~`LED8`）：ENTER 依次切换并执行 `ON → OFF → TOGGLE → BREATH` 循环，页面显示当前灯号与动作名
- 全部页：进入即执行对应动作（`ALL ON/OFF/TOGGLE/BREATH`），ENTER 再执行一次同一动作，页面显示动作名
- 页面固定 3 行：状态行、`ENTER: NEXT`（或 `ENTER: AGAIN`）、`CANCEL: BACK`
- 同一时刻只允许一个 LED 应用处于活动状态；退出应用不强制改变 LED 当前状态

#### Scenario: 单灯呼吸

- **WHEN** MAIN → LED CONTROL → SINGLE LED → `LED3`，连按 4 次 ENTER
- **THEN** LED3 依次 点亮、熄灭、翻转、进入呼吸；页面动作名同步更新

### Requirement: 统一输入事件

系统 SHALL 用一个 `uint32_t` 队列项承载两套按键，编码规则固定：

```c
#define INPUT_EVENT_MAKE(emSource, ucCode)	(((uint32_t)(emSource) << 8) | (uint8_t)(ucCode))
#define INPUT_EVENT_SOURCE(ulMsg)			((uint32_t)(ulMsg) >> 8)
#define INPUT_EVENT_CODE(ulMsg)				((uint32_t)(ulMsg) & 0xFFu)
```

- 来源枚举：`emInputSource_Btn = 0`（板载 4 键，code=0~3 对应 UP/DOWN/ENTER/CANCEL）、`emInputSource_KeyPad = 1`（矩阵键号 0~19）
- 归一化按键枚举：`emInputKey_Up/Down/Enter/Cancel`，矩阵键盘统一映射为 `emInputKey_KeyPadBase + 键号`
- 生产者：`vBtnTask`、`vKeyPadTask` 均投递到 `KeyPadMsgQueue`（16 × uint32_t，非阻塞，满则丢弃），不新增队列
- 消费者：`vMenuTask` 独占消费；`Calc_Task` 不再存在

#### Scenario: 两套按键互不干扰

- **WHEN** 在计算器应用中按矩阵键盘 7、再按板载 UP
- **THEN** 计算器收到 7 并退格显示，板载 UP 在计算器内被忽略，界面不异常

### Requirement: 统一显示帧与列表渲染

系统 SHALL 用 tag 联合体承载所有页面，OLED 任务按类型分发渲染：

```c
typedef enum
{
	emDisplayFrameType_Calc		=	0,
	emDisplayFrameType_List,
}emDisplayFrameTypeTdf;

typedef struct
{
	uint8_t	ucTitleLength;									//	标题有效字符数
	uint8_t	ucLineCount;									//	有效行数 1~3
	uint8_t	ucSelectedIndex;								//	选中行号，0xFF 表示无选中
	uint8_t	aucReserved;
	uint8_t	aucTitle[DISPLAY_LINE_LENGTH];					//	标题，6×12 @(0,0)
	uint8_t	aucLine[DISPLAY_LIST_LINE_NUM][DISPLAY_LINE_LENGTH];	//	行文本，8×16 @(0,16/32/48)
}stListDisplayMsgTdf;

typedef struct
{
	emDisplayFrameTypeTdf	emType;
	union
	{
		stCalcDisplayMsgTdf	stCalc;
		stListDisplayMsgTdf	stList;
	}unFrame;
}stDisplayMsgTdf;
```

- `vOledShowFrame()` 按 `emType` 分发：计算器帧走 `vOledShowCalcFrame()`，列表帧走 `vOledShowListFrame()`
- 列表帧渲染：清显存 → 标题（6×12 @(0,0)）→ 最多 3 行 8×16（y=16/32/48）→ 选中行加 `>` 前缀并反显 → 整帧刷新
- 显示队列项改为 `sizeof(stDisplayMsgTdf)`、深度 8，访问器由 `xGetCalcDisplayMsgQueue()` 更名为 `xGetDisplayMsgQueue()`
- 菜单页与各应用页都使用列表帧；计算器页使用计算器帧（保留大字号数值行）

#### Scenario: 帧类型切换

- **WHEN** 从 MAIN 菜单进入计算器，再按 CANCEL 返回 MAIN
- **THEN** 屏幕从列表布局切换为计算器三区布局，再切回列表布局，无残影

### Requirement: CubeMX 与 Keil 配置清单

系统 SHALL 由用户完成以下配置（`.ioc` 与 `.uvprojx` 属禁区，Agent 不修改）：

1. FreeRTOS → Tasks：
   - 新增 `MenuTask`：Entry `vMenuTask`、Priority `osPriorityNormal`、Stack 128 Words、Dynamic、As weak
   - 删除 `CalcTask`、`BtnProcessTask`
2. FreeRTOS → Queues：`KeyPadMsgQueue` 保持 16 × uint32_t（复用为统一输入队列，名称可保持不改，避免再生配置差异）；`BtnMsgQueue` 不再使用，可删除（删除时须同步删除 `User/Task/BtnProcess_Task.c/.h`）
3. FreeRTOS → Config parameters：`TOTAL_HEAP_SIZE` 维持 8192（净变化：少 2 个任务、多 1 个任务，显示队列项由 34B 增至 76B）；若 `MenuTaskHandle == NULL` 再提升至 10240
4. GPIO / Clock / NVIC：无变化
5. Keil：把 `Menu_Task.c`、`Menu_Operation.c`、`App_Operation.c`、`CalcApp_Operation.c`、`LedApp_Operation.c`、`GameApp_Operation.c` 加入 target；从 target 移除 `Calc_Task.c`、`BtnProcess_Task.c`

#### Scenario: 配置漏项可被发现

- **WHEN** CubeMX 中漏建 `MenuTask`
- **THEN** 菜单逻辑不运行、屏幕停留在上电空白，`MenuTaskHandle` 为 NULL 可在调试器中确认

## MODIFIED Requirements

### Requirement: 矩阵键盘扫描任务

原要求：扫描到 Press 事件后把键号投递到 `KeyPadMsgQueue`。

修改为：`vKeyPadTask` 扫描后把键号编码为统一输入事件（`emInputSource_KeyPad`）投递到 `KeyPadMsgQueue`，键号与扫描行为不变，消费者由 `CalcTask` 变为 `MenuTask`。

### Requirement: 板载按键任务

原要求：`vBtnTask` 投递键号到 `BtnMsgQueue`，由 `vBtnProcessTask` 映射为 LED 动作。

修改为：`vBtnTask` 把板载键号编码为统一输入事件（`emInputSource_Btn`）投递到 `KeyPadMsgQueue`；删除 `vBtnProcessTask` 及其 LED 映射逻辑，LED 改由菜单中的 LED 应用控制。

## REMOVED Requirements

### Requirement: 计算器独立任务

**Reason**：计算器已作为菜单应用运行，`vCalcTask` 会造成第二条输入/显示链路，与菜单任务抢队列和屏幕。

**Migration**：`Calculator_Operation` 保持不变，由 `CalcApp_Operation` 适配；用户在 CubeMX 删除 `CalcTask`，在 Keil 移除 `Calc_Task.c`。
