# Checklist

## 菜单与导航

- [x] MAIN 菜单含 3 项：`CALCULATOR`、`LED CONTROL`、`GAME`，上电默认选中第 1 项
- [x] 三级路径可达：MAIN → LED CONTROL → SINGLE LED → LED0~LED8；MAIN → LED CONTROL → ALL LED → 4 个动作项
- [x] UP/DOWN 选择循环（首项再 UP 到末项，末项再 DOWN 到首项）
- [x] ENTER 进入子菜单 / 启动应用；CANCEL 在子菜单返回上级、在应用内返回进入前的菜单
- [x] 根菜单按 CANCEL 无动作、不崩溃
- [x] 每级菜单独立记住选中项，返回上级后选中位置恢复
- [x] 列表项超过 3 项时窗口滚动，选中项始终可见

## 应用框架

- [x] `stAppInterfaceTdf` 六个成员齐全（`c_pcName`、`vEnter`、`vExit`、`vHandleKey`、`vTick`、`c_pstRender`）
- [x] 注册表为 `static const` 数组，`c_pstGetAppInterface()` 越界返回 NULL
- [x] 新增应用只需：实现接口 + 注册表加一行 + 菜单项表加一项（无需改 Menu_Task/Menu_Operation/OLED_Task）
- [x] 应用模式下 CANCEL 由菜单统一处理，应用自身不处理返回
- [x] `vTick` 为 NULL 时不产生周期刷新；非 NULL 时按 `MENU_TICK_PERIOD_MS` 被调用
- [x] 贪吃蛇为占位应用，显示 `SNAKE` / `COMING SOON` / `CANCEL: BACK`

## 计算器应用

- [x] 20 键映射与 `add-integer-calculator` 完全一致（数字、运算符、AC、退格、=）
- [x] `12+34=` 显示 `=46`；`7/2=` 显示 `=3`；`5/0=` 显示 `DIV/0`
- [x] 输入 3 位及以上数字完整显示（不被截断），输入上限 9999
- [x] 连算行为不变：`1+2+3=` 显示 `=6`
- [x] 板载 UP/DOWN/ENTER 在计算器内被忽略，界面不变化
- [x] CANCEL 返回 MAIN，再次进入计算器从 `0` 开始
- [x] 计算器页仍是三区布局：标题 6×12、表达式 8×16、数值 12×24

## LED 控制

- [x] 单灯页 ENTER 循环切换 `ON → OFF → TOGGLE → BREATH`，每次切换立即生效
- [x] 单灯页正确显示当前灯号与动作名
- [x] 全部页进入即执行动作（ON/OFF/TOGGLE/BREATH），ENTER 可重复执行
- [x] LED 应用只调用 `vLedSetAction()`，未直接操作 LedDevice 的 GPIO 层
- [x] 退出 LED 应用不改变 LED 当前状态，LED 仍由 LedTask 1ms 驱动正常呼吸

## 输入与显示链路

- [x] `vBtnTask`、`vKeyPadTask` 均投递到 `KeyPadMsgQueue`（16 × uint32_t），编码符合 `INPUT_EVENT_MAKE`
- [x] 板载键 code 0~3 解码为 Up/Down/Enter/Cancel，矩阵键号 0~19 解码为 `emInputKey_KeyPadBase + 键号`
- [x] 菜单模式下矩阵键盘按键被忽略，不误触发菜单项
- [x] 显示队列项为 `sizeof(stDisplayMsgTdf)`（76 字节），8 项，经 `xGetDisplayMsgQueue()` 获取
- [x] 计算器帧与列表帧切换无残影、无越界绘制
- [x] 无消息时 OLED 任务阻塞、不重绘、不空转

## 工程与规范

- [x] `Project_Config.h` 新增宏齐全：`MENU_VISIBLE_ITEM_NUM`、`MENU_MAX_DEPTH`、`MENU_TICK_PERIOD_MS`、`DISPLAY_LINE_LENGTH`、`DISPLAY_LIST_LINE_NUM`
- [x] 头文件保护为 `_XXX_H_`；命名前缀 v/em/c_pst/s_/st 规范；Tab 缩进；Doxygen 注释；switch 带 default
- [x] C89 风格：变量声明位于函数体首部
- [x] 未修改 `Core/`、`.ioc`、`User/Device/`、`MDK-ARM/*.uvprojx`
- [x] 已删除 `Calc_Task.c/.h`、`BtnProcess_Task.c/.h`，`User/` 全仓无残留引用
- [x] Task→Operation→Device 单向调用；仅 Device 层包含 HAL 头
- [ ] 用户已在 CubeMX 新增 `MenuTask`（128 Words、As weak、Dynamic）并删除 `CalcTask`、`BtnProcessTask`
- [ ] 用户已在 Keil 加入 6 个新源文件、移除 2 个旧源文件，编译无错误无警告

## 实物联调（用户执行）

- [ ] 上电显示 MAIN 菜单，板载 UP/DOWN/ENTER/CANCEL 导航正常
- [ ] 进入计算器完成一次四则运算后 CANCEL 返回菜单
- [ ] 进入 LED 单灯页驱动 LED3 呼吸，返回后 LED 状态保持
- [ ] 进入 GAME → SNAKE 显示占位页并可返回
- [ ] 连续快速按板载键不丢键、显示不卡死
