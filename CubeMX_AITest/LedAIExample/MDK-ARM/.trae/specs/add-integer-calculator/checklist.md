# Checklist

## 计算模块（Calculator_Operation）

- [x] 按键映射表 20 项与需求逐条对应：0/2/15 号键忽略；1=数字0、3=AC、7=退格、11=`=`、4~6=7/8/9、8~10=4/5/6、12~14=1/2/3、16~19=+/-/*//
- [x] `slCalcArithmetic()` 对加、减、乘、整数除法均返回正确结果，且可被独立调用
- [x] 除零返回错误码 `emCalcError_DivideByZero`，界面显示 `DIV/0`，不进入死循环、不崩溃
- [x] 操作数输入限制 0~9999：满 4 位后再按数字被忽略，输入值不会超过 9999
- [x] 前导零被替换（`0` 后按 `7` 显示 `7`）
- [x] 状态机行为与 spec 表格一致：输入操作数 1/2、结果态、错误态的每个按键分支均有对应处理
- [x] 结果态按数字键开始新计算；结果态按运算符以结果继续运算
- [x] 支持连算：`1+2+3=` 依次得到中间结果 3、最终 6；连算途中除零立即进入错误态
- [x] 输入操作数 2 为空时按运算符只替换运算符（`12+` 后按 `-` 显示 `12-`）
- [x] 空输入按运算符被忽略；输入操作数 2 为空时按 `=` 被忽略
- [x] 计算模块不包含 `stm32f1xx_hal.h` 或任何 Device 头文件，仅依赖 `stdint.h` 与 `Project_Config.h`
- [x] 数值转字符串由计算模块自实现，工程内未引入 `sprintf`/`snprintf`

## 队列通信

- [x] `vCalcTask` 阻塞读取 `KeyPadMsgQueue`，取出键号后调用 `vCalcInputKey()`
- [x] `vCalcTask` 每次按键处理后向显示帧队列（`xGetCalcDisplayMsgQueue()`）投递一帧，投递为非阻塞（超时 0）
- [x] `vKeyPadTask` 扫描后逐条投递所有 Press 事件键号，队列满时丢弃且不阻塞
- [x] `vOledTask` 阻塞读取显示帧队列，无消息时不重绘、不空转延时
- [x] 无任务直接读取对方模块的内部状态（除只读 `c_pstGetCalcDisplayMsg()` 由 Calc 自己调用）

## 显示

- [x] 上电首帧显示标题 `INT CALC 0-9999`、表达式行 `0`、数值行 `0`
- [x] 每帧渲染顺序为：清显存 → 标题 → 表达式行 → 数值行 → 刷新
- [x] 表达式行用 8×16、数值行用 12×24，`99980001`/`-99980001` 等最长结果不超出 128 像素宽
- [x] 输入过程实时回显：按 1、2、+ 时表达式行为 `1`→`12`→`12+`

## 工程与规范

- [x] `Project_Config.h` 已新增 `CALC_INPUT_MAX_VALUE`、`CALC_INPUT_MAX_DIGIT`、`CALC_LINE_BUFFER_LENGTH`
- [x] 新增文件头保护为 `_CALCULATOR_OPERATION_H_`、`_CALC_TASK_H_`
- [x] 命名遵循 `v/em/ul/sl/c_pst/st/uc/us` 前缀，Tab 缩进，Doxygen 注释，switch 带 default
- [x] C89 风格：所有变量声明位于函数体首部
- [x] 未修改 `Core/`、`.ioc`、`User/Device/`、`MDK-ARM/*.uvprojx`
- [ ] 用户已在 CubeMX 完成：新增 `CalcTask`（128 Words、As weak、Dynamic）、`KeyPadMsgQueue`（16×4B）、堆提升至 8192；并在 CubeMX 中删除 `CalcDisplayMsgQueue`
- [ ] 显示帧队列由 `xGetCalcDisplayMsgQueue()` 按 `sizeof(stCalcDisplayMsgTdf)`（34 字节）× 8 创建，未沿用 CubeMX 的 2 字节队列项
- [ ] 用户已在 Keil 把 `Calculator_Operation.c`、`Calc_Task.c` 加入 target，编译通过且无警告
- [ ] `KeyPadMsgQueue` Item Size 为 4 字节（未沿用 BtnMsgQueue 的 1 字节）

## 实物联调（用户执行）

- [ ] 20 键按键映射与面板丝印一致：数字 0~9、AC、退格、`=`、`+ - * /` 位置正确
- [ ] 典型用例通过：`12+34=` 显示 `=46`；`7/2=` 显示 `=3`；`5/0=` 显示 `DIV/0`
- [ ] 长按单键不重复输入（依赖键盘层"读清"事件，不重复触发）
- [ ] 快速连按不丢键（队列未溢出）
- [ ] AC 后界面回到 `0`，可连续进行下一轮计算
