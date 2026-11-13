# 整数四则运算计算器 Spec

## Why

矩阵键盘（5 行 × 4 列 = 20 键）与 OLED 已分别完成并稳定运行，但两者尚未连通：KeyPadTask 只做扫描，OledTask 只做上电测试显示。本次用队列把"键盘 → 计算 → 显示"串成完整链路，实现一个可用的整数四则运算计算器；运算逻辑独立成模块，为后续扩展（连算、小数、括号）留出空间。

## What Changes

- 新增计算核心模块 `User/Operation/Calculator_Operation.c/.h`：按键映射表、输入状态机、四则运算内核 `slCalcArithmetic()`、显示文本生成；纯软件模块，不包含任何 HAL / Device 头文件
- 新增 `User/Task/Calc_Task.c/.h`：`vCalcTask` 阻塞消费 `KeyPadMsgQueue`，调用计算模块后把显示数据投递到显示帧队列（`xGetCalcDisplayMsgQueue()`）
- 修改 `User/Task/KeyPad_Task.c`：扫描到 Press 事件后把键号（0~19）投递到 `KeyPadMsgQueue`
- 修改 `User/Task/OLED_Task.c`：由"初始化 + 测试显示 + 空转"改为"初始化 + 阻塞消费显示帧队列 + 刷新界面"，并新增 `xGetCalcDisplayMsgQueue()` 按 34 字节队列项创建该队列
- 修改 `User/Operation/OLED_Operation.c/.h`：新增 `vOledShowCalcFrame()` 渲染一帧计算器界面（标题 / 表达式行 / 数值行）
- 修改 `User/Project_Config.h`：新增 `CALC_INPUT_MAX_VALUE`、`CALC_INPUT_MAX_DIGIT`、`CALC_LINE_BUFFER_LENGTH`
- CubeMX 侧新增 1 个任务 + 1 个队列（用户操作），显示帧队列在用户代码中创建，Keil 侧添加 2 个新源文件
- **BREAKING**：无

## Impact

- Affected specs：矩阵键盘驱动（KeyPadTask 行为由"仅扫描、业务层轮询"改为"扫描 + 投递队列"）；OLED 显示任务（OledTask 不再空转，改为显示队列消费者）
- Affected code：
  - 新增：`User/Operation/Calculator_Operation.c/.h`、`User/Task/Calc_Task.c/.h`
  - 修改：`User/Task/KeyPad_Task.c`、`User/Task/OLED_Task.c`、`User/Operation/OLED_Operation.c/.h`、`User/Project_Config.h`
  - 不改动：`User/Device/KeyPadDevice.*`、`User/Device/OLEDDevice.*`、`Core/`、`.ioc`（由用户在 CubeMX 中配置）

## ADDED Requirements

### Requirement: 独立的整数四则运算模块

系统 SHALL 在 Operation 层提供独立的计算模块 `Calculator_Operation`，对外暴露初始化、按键输入、显示数据读取与运算内核，且不依赖任何硬件头文件。

模块内部维护私有上下文（状态、运算符、两个操作数、当前输入位数、错误码），对外只读提供当前显示数据。

#### Scenario: 加法

- **WHEN** 依次按键 1、2、+、3、4、=
- **THEN** 表达式行显示 `12+34`，数值行显示 `=46`

#### Scenario: 整数除法截断

- **WHEN** 依次按键 7、/、2、=
- **THEN** 数值行显示 `=3`（C 语言整数除法，向零截断）

#### Scenario: 除法结果为负

- **WHEN** 依次按键 1、-、5、=
- **THEN** 数值行显示 `=-4`

#### Scenario: 除零

- **WHEN** 依次按键 5、/、0、=
- **THEN** 数值行显示 `DIV/0`，进入错误态；错误态下除"全清"外所有按键均被忽略

#### Scenario: 乘法上限

- **WHEN** 依次按键 9、9、9、9、*、9、9、9、9、=
- **THEN** 数值行显示 `=99980001`（结果用 `int32_t` 承载，操作数上限保证不溢出）

#### Scenario: 运算内核可被独立调用

- **WHEN** 业务代码直接调用 `slCalcArithmetic(6, 3, emCalcOperator_Div, &emError)`
- **THEN** 返回 2，`emError` 为 `emCalcError_None`

### Requirement: 操作数输入范围限制 0~9999

系统 SHALL 将键盘输入的操作数限制为 0~9999 的整数，以便在 128×64 OLED 上完整显示。

- 当前操作数已满 4 位时，再按数字键被忽略
- 当前操作数为 0 时按数字键，用新数字替换（不产生 `0x` 这种前导零）
- 输入过程中的值恒在 0~9999 内，不会出现第 5 位
- 计算结果不受该限制约束（`int32_t` 范围内）

#### Scenario: 第 5 位被忽略

- **WHEN** 依次按键 1、2、3、4、5
- **THEN** 数值行显示 `1234`

#### Scenario: 前导零替换

- **WHEN** 初始状态依次按键 0、7
- **THEN** 数值行显示 `7`

### Requirement: 按键功能映射

系统 SHALL 按以下固定映射解析键号（键号 = 行索引 × 4 + 列索引，与 KeyPad 模块一致）：

| 键号 | 行列 | 功能 |
| --- | --- | --- |
| 0 | (0,0) | 未使用（忽略） |
| 1 | (0,1) | 数字 0 |
| 2 | (0,2) | 未使用（忽略） |
| 3 | (0,3) | 全清 AC |
| 4 | (1,0) | 数字 7 |
| 5 | (1,1) | 数字 8 |
| 6 | (1,2) | 数字 9 |
| 7 | (1,3) | 退格 |
| 8 | (2,0) | 数字 4 |
| 9 | (2,1) | 数字 5 |
| 10 | (2,2) | 数字 6 |
| 11 | (2,3) | `=` |
| 12 | (3,0) | 数字 1 |
| 13 | (3,1) | 数字 2 |
| 14 | (3,2) | 数字 3 |
| 15 | (3,3) | 未使用（忽略） |
| 16 | (4,0) | `+` |
| 17 | (4,1) | `-` |
| 18 | (4,2) | `*` |
| 19 | (4,3) | `/` |

映射实现为计算模块内的 `static const` 表，长度 `KEYPAD_DEV_NUM`，新增/改动键位只改表。

#### Scenario: 未使用键无响应

- **WHEN** 依次按键 0、2、15 号键
- **THEN** 界面与内部状态无任何变化

### Requirement: 计算器输入状态机

系统 SHALL 用三态状态机（输入操作数 1 / 输入操作数 2 / 显示结果）加一个错误态管理输入过程：

| 当前状态 | 按键 | 行为 |
| --- | --- | --- |
| 输入操作数 1 | 数字 | 追加到操作数 1 |
| 输入操作数 1 | 运算符 | 操作数 1 已有输入时记录运算符，转输入操作数 2；无输入时忽略 |
| 输入操作数 1 | `=` | 忽略 |
| 输入操作数 1 | 退格 | 操作数 1 去掉末位数字 |
| 输入操作数 2 | 数字 | 追加到操作数 2 |
| 输入操作数 2 | 运算符 | 操作数 2 已有输入时立即计算，结果作为新的操作数 1（进入错误态则转错误态），并记录新运算符，转输入操作数 2；操作数 2 无输入时仅替换当前运算符 |
| 输入操作数 2 | `=` | 操作数 2 已有输入时计算并显示结果，转结果态；无输入时忽略 |
| 输入操作数 2 | 退格 | 操作数 2 去掉末位数字 |
| 显示结果 | 数字 | 清空上下文，以该数字开始新计算 |
| 显示结果 | 运算符 | 以当前结果作为操作数 1，记录运算符，转输入操作数 2 |
| 显示结果 | `=` / 退格 | 忽略 |
| 错误态 | 全清 | 清空上下文，回到输入操作数 1 |
| 错误态 | 其他任意键 | 忽略 |
| 任意状态 | 全清 AC | 清空上下文，操作数/运算符/错误码全部复位，显示 `0` |

#### Scenario: 运算符连算

- **WHEN** 依次按键 1、+、2、+
- **THEN** 按下第二个 `+` 时立即完成 1+2，表达式行显示 `3+`，数值行显示 `0`（等待新的操作数 2）

#### Scenario: 连算后完成

- **WHEN** 承接上例继续按 3、=
- **THEN** 数值行显示 `=6`

#### Scenario: 未输入操作数 2 时换运算符

- **WHEN** 依次按键 1、2、+、-
- **THEN** 表达式行显示 `12-`，不触发计算

#### Scenario: 连算遇到除零

- **WHEN** 依次按键 5、/、0、+
- **THEN** 按下 `+` 时立即进入错误态，数值行显示 `DIV/0`

#### Scenario: 结果态开始新计算

- **WHEN** 计算 `12+34=` 后按 9
- **THEN** 表达式行显示 `9`，数值行显示 `9`，此前的 12+34 被清空

#### Scenario: 结果态继续运算

- **WHEN** 计算 `12+34=` 后按 +、1、=
- **THEN** 数值行显示 `=47`

#### Scenario: 空输入按运算符被忽略

- **WHEN** 初始状态直接按 +
- **THEN** 界面保持显示 `0`，状态仍为输入操作数 1

#### Scenario: 全清

- **WHEN** 输入 `123+456` 后按 AC
- **THEN** 表达式行显示 `0`，数值行显示 `0`

### Requirement: 模块间队列通信

系统 SHALL 使用两个 FreeRTOS 消息队列在两个方向上解耦模块，禁止跨模块直接调用：

1. `KeyPadMsgQueue`（键盘 → 计算）
   - 生产者：`vKeyPadTask`，在每次扫描后把本周期所有 Press 事件的键号逐条非阻塞投递（队列满则丢弃）
   - 消费者：`vCalcTask`，`osMessageQueueGet` 永久阻塞等待
   - 队列项：`uint32_t`（4 字节），队列长度 16
2. 显示帧队列（计算 → 显示，队列对象由用户代码创建，句柄经 `xGetCalcDisplayMsgQueue()` 获取）
   - 生产者：`vCalcTask`，每次处理完一枚按键后投递一帧 `stCalcDisplayMsgTdf`（非阻塞）
   - 消费者：`vOledTask`，初始化完成后永久阻塞等待并刷新屏幕
   - 队列项：`stCalcDisplayMsgTdf`（34 字节），队列长度 8，在 `OLED_Task.c` 中按 `sizeof(stCalcDisplayMsgTdf)` 创建
   - 不能在 CubeMX 中建：CubeMX 的 Item Size 只能选 `uint8_t/uint16_t/uint32_t` 等基本类型，无法表达 34 字节结构体；若按 2 字节（`int16_t`）创建，每帧只搬运 2 个字节，表现为"输入 3 位数字只显示 2 位、数值行不刷新"

上电时序：`vCalcTask` 先 `vCalcInit()` 投递首帧 `0`；`vOledTask` 初始化完成后从队列取首帧显示，因此任务启动先后顺序不影响显示结果。

#### Scenario: 一次按键产生一帧显示

- **WHEN** 按下一枚有效键并松开
- **THEN** `KeyPadMsgQueue` 中出现一个键号，`vCalcTask` 取出后显示帧队列中出现一帧数据，`vOledTask` 消费后屏幕更新

#### Scenario: 队列满不阻塞

- **WHEN** 显示任务处理较慢，显示帧队列已满
- **THEN** 投递失败被直接丢弃，`vCalcTask` 不阻塞、不丢键

### Requirement: OLED 显示布局

系统 SHALL 每帧按以下固定布局刷新 128×64 屏幕：

| 区域 | 坐标 | 字号 | 内容 |
| --- | --- | --- | --- |
| 标题行 | (0, 0) | 6×12 | 固定文本 `INT CALC 0-9999` |
| 表达式行 | (0, 16) | 8×16 | 见下表 |
| 数值行 | (0, 36) | 12×24 | 见下表 |

| 状态 | 表达式行（≤16 字符） | 数值行（≤10 字符） |
| --- | --- | --- |
| 输入操作数 1 | 当前操作数 1，如 `0`、`12` | 当前操作数 1 |
| 输入操作数 2 | 操作数 1 + 运算符，如 `12+` | 当前操作数 2 |
| 显示结果 | 操作数 1 + 运算符 + 操作数 2，如 `12+34` | `=` + 结果，如 `=46` |
| 错误态 | 出错表达式，如 `12/0` | `DIV/0` |

- 每帧流程：清显存 → 画标题 → 画表达式行 → 画数值行 → 整帧刷新
- 显示模块只接受"已格式化"的文本，不做数值转换；数值转字符串由计算模块完成（工程内未引入 `sprintf`）

#### Scenario: 上电首帧

- **WHEN** 系统上电
- **THEN** 屏幕显示标题、表达式行 `0`、数值行 `0`

#### Scenario: 输入过程实时回显

- **WHEN** 依次按键 1、2、+
- **THEN** 表达式行由 `0` → `1` → `12` → `12+`，数值行在输入 1、2 时分别为 `1`、`12`，按 + 后数值行清为 `0`

### Requirement: CubeMX 配置清单

系统 SHALL 由用户在 CubeMX 中完成以下配置（`.ioc` 属禁区，Agent 不修改），并在 Keil 中把新增源文件加入 target：

1. FreeRTOS → Tasks，新增：

| 参数 | 值 |
| --- | --- |
| Task Name | `CalcTask` |
| Entry Function | `vCalcTask` |
| Priority | `osPriorityNormal`（与 KeyPadTask 同级） |
| Stack Size (Words) | 128 |
| Allocation | Dynamic |
| Generate Under Root / As weak | As weak（由 `User/Task/Calc_Task.c` 覆盖） |

2. FreeRTOS → Queues，新增一个：

| Queue Name | Queue Size（项数） | Item Size（字节） | Allocation |
| --- | --- | --- | --- |
| `KeyPadMsgQueue` | 16 | 4（`uint32_t`） | Dynamic |

   注意：现有 `BtnMsgQueue` 的 Item Size 是 1 字节，新队列不要照抄，`KeyPadMsgQueue` 必须为 4 字节，否则键号被截断。

   显示帧队列**不在此处新增**（CubeMX 无法表达 34 字节结构体队列项）；若已有 `CalcDisplayMsgQueue`，应在 CubeMX 中删除并重新生成，队列改由 `User/Task/OLED_Task.c` 的 `xGetCalcDisplayMsgQueue()` 创建。

3. FreeRTOS → Config parameters：`TOTAL_HEAP_SIZE` 由当前 6144 提升至 8192（新增 1 任务 + 2 队列的余量）
4. GPIO：无需新增。确认现有配置不变——行 PA6/PA5/PA4/PA3/PA2 上拉输入；列 PB10/PB1/PB0/PA7 开漏输出；OLED SCL=PB8、SDA=PB9 推挽输出
5. Clock / NVIC / 串口：无变化
6. 生成代码后：在 Keil 中把 `User/Operation/Calculator_Operation.c`、`User/Task/Calc_Task.c` 加入 target；确认 `freertos.c` 中 `vCalcTask` 为 weak 定义、`KeyPadMsgQueueHandle` 已生成

#### Scenario: 配置漏项可被发现

- **WHEN** CubeMX 中漏建 `KeyPadMsgQueue`
- **THEN** 编译报 `KeyPadMsgQueueHandle` 未定义，属预期失败，需补配置而非改代码绕过

#### Scenario: 显示帧队列项宽度

- **WHEN** 显示帧队列按 `sizeof(stCalcDisplayMsgTdf)`（34 字节）创建
- **THEN** 每帧 34 字节整体搬运，三位及以上数字与数值行均完整刷新；若项大小小于结构体，则表现为只刷新前几个字符

## MODIFIED Requirements

### Requirement: KeyPad 扫描任务

原要求：`vKeyPadTask` 10ms 周期扫描，仅做消抖，业务层用 `emKeyPadGetPressEvent()` 轮询。

修改为：`vKeyPadTask` 在 10ms 扫描后，把本周期所有 Press 事件逐条投递到 `KeyPadMsgQueue`（`uint32_t` 键号、非阻塞），与 `vBtnTask` 的写法保持一致；不再由业务任务轮询键盘。

#### Scenario: 多键先后按下

- **WHEN** 一个扫描周期内有多个键被判定为 Press
- **THEN** 每个键号各投递一条消息，不合并、不丢失

### Requirement: OLED 显示任务

原要求：`vOledTask` 上电 `vOledInitAll()` + `vOledTest()` 后每 1s 空转。

修改为：`vOledTask` 上电 `vOledInitAll()` 后阻塞等待 `CalcDisplayMsgQueue`，每取到一帧调用 `vOledShowCalcFrame()` 刷新；不再调用 `vOledTest()`（`vOledTest()` 函数本身保留，供调试用）。

#### Scenario: 无按键时屏幕保持

- **WHEN** 长时间无按键输入
- **THEN** 任务阻塞在队列上，屏幕内容保持不变，不周期性重绘
