# Tasks

- [x] Task 1: 配置宏与计算模块头文件：定义计算器对外契约
  - [x] SubTask 1.1: `User/Project_Config.h` 追加 `CALC_INPUT_MAX_VALUE`(9999)、`CALC_INPUT_MAX_DIGIT`(4)、`CALC_LINE_BUFFER_LENGTH`(17)
  - [x] SubTask 1.2: 新建 `User/Operation/Calculator_Operation.h`：状态枚举(`emCalcState_InputOperand1/InputOperand2/Result/Error`)、运算符枚举(`None/Add/Sub/Mul/Div`)、错误枚举(`None/DivideByZero`)、`stCalcDisplayMsgTdf`（两行各 17 字节）、`vCalcInit` / `vCalcInputKey` / `c_pstGetCalcDisplayMsg` / `slCalcArithmetic` 声明
  - [x] 验证：头文件不含 `stm32f1xx_hal.h` 等硬件头；头保护为 `_CALCULATOR_OPERATION_H_`

- [x] Task 2: 实现计算核心 `User/Operation/Calculator_Operation.c`
  - [x] SubTask 2.1: `slCalcArithmetic()`：int32 加/减/乘/整数除法，除零返回 0 并置 `emCalcError_DivideByZero`
  - [x] SubTask 2.2: `static const` 20 项按键映射表（键号 0~19 → 数字/运算符/AC/退格/`=`/忽略）
  - [x] SubTask 2.3: 输入状态机 `vCalcInputKey()`，覆盖 spec 中状态×按键全表，含 4 位上限、前导零替换、结果态开新算、**运算符连算（`1+2+3=` 逐级结算）与空操作数时换运算符**、错误态仅 AC 有效
  - [x] SubTask 2.4: 文本生成：`uint32_t`/`int32_t` 转十进制字符串（不依赖 `sprintf`），按 spec 填写表达式行与数值行
  - [x] SubTask 2.5: `c_pstGetCalcDisplayMsg()` 返回内部静态显示帧；`vCalcInit()` 复位上下文并生成首帧 `0`
  - [x] 验证：静态自查 — 输入 `9999*9999` 结果 99980001；`5/0` 进入错误态；C89 变量声明在函数首部

- [x] Task 3: 实现计算任务 `User/Task/Calc_Task.c/.h`
  - [x] SubTask 3.1: `vCalcTask()`：`vCalcInit()` → 投递首帧到 `CalcDisplayMsgQueue` → 循环阻塞 `osMessageQueueGet(KeyPadMsgQueueHandle)` → `vCalcInputKey((uint8_t)ulKey)` → 投递 `c_pstGetCalcDisplayMsg()` 到 `CalcDisplayMsgQueue`
  - [x] SubTask 3.2: `extern osMessageQueueId_t KeyPadMsgQueueHandle / CalcDisplayMsgQueueHandle;`，包含 `cmsis_os.h`、`FreeRTOS.h`、`task.h`
  - [x] 验证：无 `vTaskDelay` 空转，任务完全由队列驱动；头保护 `_CALC_TASK_H_`

- [x] Task 4: 修改 `User/Task/KeyPad_Task.c` 投递键号
  - [x] SubTask 4.1: 增加 `extern osMessageQueueId_t KeyPadMsgQueueHandle;` 与 `cmsis_os.h` 等包含
  - [x] SubTask 4.2: 扫描后 `while((emDev = emKeyPadGetPressEvent()) < KEYPAD_DEV_NUM)` 逐条 `osMessageQueuePut`（`uint32_t`，非阻塞 0 超时）
  - [x] 验证：与 `Btn_Task.c` 写法一致；不改 `vKeyPadInitAll/vKeyPadExecuteAll` 调用顺序

- [x] Task 5: 显示链路 `User/Operation/OLED_Operation.c/.h` + `User/Task/OLED_Task.c`
  - [x] SubTask 5.1: `OLED_Operation.h` 声明 `void vOledShowCalcFrame(const stCalcDisplayMsgTdf *pstMsg);`，并包含 `Calculator_Operation.h`
  - [x] SubTask 5.2: 实现帧渲染：清显存 → 标题 `INT CALC 0-9999`（6×12 @ (0,0)）→ 表达式行（8×16 @ (0,16)）→ 数值行（12×24 @ (0,36)）→ `vOledRefreshFromBuffer`
  - [x] SubTask 5.3: `vOledTask` 去掉 `vOledTest()` 调用，改为循环阻塞取 `CalcDisplayMsgQueue` 并调用 `vOledShowCalcFrame()`
  - [x] 验证：`vOledTest()` 函数保留；OledTask 不再 `vTaskDelay(1000)`

- [x] Task 6: 规范与一致性静态检查
  - [x] SubTask 6.1: 头保护、命名前缀（v/em/ul/sl/c_pst/st/uc/us）、Tab 缩进、Doxygen 注释、switch 带 default
  - [x] SubTask 6.2: 分层校验：`Calculator_Operation.c` 不含 HAL/Device 头；Task 层不直接调用 Device API
  - [x] SubTask 6.3: 队列项宽校验：`KeyPadMsgQueue` 按 `uint32_t` 存取；`stCalcDisplayMsgTdf` 大小与 CubeMX 配置的 34 字节一致（static assert 或注释说明）
  - [x] 验证：输出检查结论清单，指出需用户在 CubeMX/Keil 完成的步骤

# Task Dependencies

- Task 2 依赖 Task 1
- Task 3 依赖 Task 2
- Task 5 依赖 Task 2（需要 `stCalcDisplayMsgTdf`）
- Task 4 与 Task 3/5 无代码依赖，可并行
- Task 6 依赖 Task 1~5 全部完成
