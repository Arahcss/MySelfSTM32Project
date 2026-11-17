# Tasks

- [x] Task 1: 公共定义与统一输入事件
  - [x] SubTask 1.1: `User/Project_Config.h` 新增 `MENU_VISIBLE_ITEM_NUM 3`、`MENU_MAX_DEPTH 3`、`MENU_TICK_PERIOD_MS 50`、`DISPLAY_LINE_LENGTH 17`、`DISPLAY_LIST_LINE_NUM 3`
  - [x] SubTask 1.2: 新建 `User/Operation/Input_Operation.h`：`emInputSourceTdf`（Btn=0、KeyPad=1）、`emInputKeyTdf`（Up/Down/Enter/Cancel/KeyPadBase）、`INPUT_EVENT_MAKE/SOURCE/CODE` 宏
  - [x] 验证：仅头文件、无 HAL 依赖；宏与枚举命名遵循项目规范

- [x] Task 2: 显示帧扩展与列表渲染
  - [x] SubTask 2.1: `OLED_Operation.h` 新增 `emDisplayFrameTypeTdf`、`stListDisplayMsgTdf`、`stDisplayMsgTdf`（union）与 `vOledShowFrame()`、`vOledShowListFrame()` 声明
  - [x] SubTask 2.2: `OLED_Operation.c` 实现列表渲染：清显存 → 标题 6×12 @(0,0) → ≤3 行 8×16 @(0,16/32/48) → 选中行加 `>` 前缀并反显（`emOledPixelShowMode_Negative`）→ 整帧刷新；行文本超长按屏宽截断
  - [x] SubTask 2.3: 实现 `vOledShowFrame()` 按 `emType` 分发到 `vOledShowCalcFrame()` / `vOledShowListFrame()`
  - [x] 验证：现有计算器页面渲染结果不变；列表页 3 行不越界（48+16=64）

- [x] Task 3: 应用接口与注册表
  - [x] SubTask 3.1: `App_Operation.h` 定义 `stAppInterfaceTdf`（`c_pcName`、`vEnter(uint8_t)`、`vExit`、`vHandleKey`、`ucTick`、`c_pstRender`）与 `APP_*` 应用索引宏
  - [x] SubTask 3.2: `App_Operation.c` 建立静态注册表（`APP_CALCULATOR`、`APP_LED_SINGLE`、`APP_LED_ALL`、`APP_GAME_SNAKE`），实现 `c_pstGetAppInterface(uint8_t)`
  - [x] 验证：注册表为 `static const`，越界索引返回 NULL

- [x] Task 4: 菜单状态机
  - [x] SubTask 4.1: `Menu_Operation.h` 定义 `emMenuNodeTypeTdf`、`stMenuItemTdf`、`stMenuTdf`、`emMenuModeTdf`（Menu/App），声明 `vMenuInit`、`vMenuHandleKey`、`vMenuExecuteTick`、`c_pstMenuGetRender`、`c_pstGetActiveApp`
  - [x] SubTask 4.2: `Menu_Operation.c` 定义菜单树：MAIN（CALCULATOR / LED CONTROL / GAME）、LED CONTROL（SINGLE LED / ALL LED）、SINGLE LED（仅 LED0 → APP_LED_SINGLE，param=0；硬件只接入板载灯）、ALL LED（ALL ON/OFF/TOGGLE/BREATH → APP_LED_ALL，param=动作）、GAME（SNAKE → APP_GAME_SNAKE）
  - [x] SubTask 4.3: 实现导航：UP/DOWN 循环移动、ENTER 进子菜单或启动应用（`vEnter(param)`）、CANCEL 退出应用或回上级；用 `s_aucMenuStack[3]`/`s_aucSelectStack[3]` 记住各级选中项
  - [x] SubTask 4.4: 实现列表帧生成：标题=当前菜单标题，窗口 3 行随选中项滚动，`ucSelectedIndex`=选中行在窗口内位置（应用页填 0xFF）
  - [x] SubTask 4.5: 实现 `ucMenuExecuteTick()`：仅在应用模式且 `ucTick != NULL` 时调用，返回非0才重建显示帧
  - [x] 验证：菜单模式下矩阵键盘事件被忽略；返回上级选中项保持不变

- [x] Task 5: 三个应用实现
  - [x] SubTask 5.1: `CalcApp_Operation.c/.h`：`vEnter` 调 `vCalcInit()`；`vHandleKey` 仅在 `emKey >= emInputKey_KeyPadBase` 时调 `vCalcInputKey(emKey - emInputKey_KeyPadBase)`；`c_pstRender` 把 `c_pstGetCalcDisplayMsg()` 拷入静态 union 帧（`emType=Calc`）；`ucTick = NULL`
  - [x] SubTask 5.2: `LedApp_Operation.c/.h`：单灯页 `vEnter(param=灯号)` 显示灯号与 `PRESS ENTER`，ENTER 依次执行 `ON/OFF/TOGGLE/BREATH`（`vLedSetAction`）；全部页 `vEnter(param=动作)` 立即执行一次，ENTER 重复执行；均生成 3 行列表帧
  - [x] SubTask 5.3: `GameApp_Operation.c/.h`：贪吃蛇应用适配（详见 Task 9），`ucTick = ucGameAppTick`
  - [x] 验证：三应用均通过 `stAppInterfaceTdf` 暴露，未直接操作 OLEDDevice

- [x] Task 6: 菜单任务与显示链路改造
  - [x] SubTask 6.1: `Menu_Task.c/.h`：`vMenuTask` 取显示队列句柄 → `vMenuInit()` → 投递首帧；循环 `osMessageQueueGet(KeyPadMsgQueueHandle, NULL, MENU_TICK_PERIOD_MS)`，收到事件则解码并 `vMenuHandleKey()`，超时则 `vMenuExecuteTick()`；两种情况都投递最新帧（非阻塞）
  - [x] SubTask 6.2: `OLED_Task.c/.h`：队列项改为 `sizeof(stDisplayMsgTdf)`，访问器更名 `xGetDisplayMsgQueue()`，渲染改为 `vOledShowFrame()`
  - [x] 验证：队列项约 124 字节整体搬运（8 项约 1KB 堆）；无消息不重绘

- [x] Task 7: 输入源改造与旧任务清理
  - [x] SubTask 7.1: `KeyPad_Task.c` 投递改为 `INPUT_EVENT_MAKE(emInputSource_KeyPad, emDevNum)`
  - [x] SubTask 7.2: `Btn_Task.c` 投递改为 `INPUT_EVENT_MAKE(emInputSource_Btn, emBtn)`，目标队列改为 `KeyPadMsgQueueHandle`
  - [x] SubTask 7.3: 删除 `User/Task/Calc_Task.c/.h`、`User/Task/BtnProcess_Task.c/.h`
  - [x] 验证：`User/` 全仓无 `CalcDisplayMsgQueueHandle`、`BtnMsgQueueHandle`、`vCalcTask`、`vBtnProcessTask` 残留引用（`Core/Src/main.c` 与 `.uvprojx` 残留由用户在 CubeMX/Keil 处理）

- [x] Task 8: 静态检查与文档收尾
  - [x] SubTask 8.1: 检查命名前缀（v/em/c_pst/s_/st）、Tab 缩进、Doxygen 注释、C89 变量声明在函数首部、switch 带 default、头保护 `_XXX_H_`
  - [x] SubTask 8.2: 检查分层：Task→Operation→Device 单向；仅 Device 含 HAL 头；应用不直接调用 OLEDDevice/LedDevice 的底层函数
  - [x] SubTask 8.3: 按 spec 的 CubeMX/Keil 清单核对用户配置项，更新 `checklist.md`

- [x] Task 9: 贪吃蛇游戏
  - [x] SubTask 9.1: `User/Operation/SnakeGame_Operation.h/.c`：16×6 网格、长度 3、初始 (5,3) 向右、LCG 随机食物 + 线性探测；`ucSnakeGameStep()` 处理移动/增长/撞墙/撞身/胜利，撞身检测豁免尾巴格
  - [x] SubTask 9.2: `OLED_Operation.h/.c` 新增 `emDisplayFrameType_Grid`、`stGridDisplayMsgTdf`（列/行/格边长/起始 y/标题/96 格）与 `vOledShowGridFrame()`：逐点填充实心格，超出 128×64 的格子整格跳过
  - [x] SubTask 9.3: `GameApp_Operation.c/.h` 实现应用适配：键 2/4/6/8 转向、3 重开、11 暂停、ENTER 结束后重开；`ucGameAppTick()` 每 4 个 50ms 走一步，仅在帧变化时返回 1，结束/胜利切换为列表帧
  - [x] SubTask 9.4: `App_Operation.c` 注册表 GAME 行填 `ucGameAppTick`；`Menu_Task.c` 仅 `ucTick` 返回非 0 时投递帧
  - [x] 验证：网格 16×8=128 宽、6×8+16=64 高恰好铺满；`SnakeGame_Operation.c` 需用户在 Keil 手动加入 target

# Task Dependencies

- Task 2、Task 3 依赖 Task 1
- Task 4 依赖 Task 2、Task 3
- Task 5 依赖 Task 3、Task 4（应用接口与菜单项参数）
- Task 6 依赖 Task 2、Task 4、Task 5
- Task 7 依赖 Task 1（编码宏）；可与 Task 5 并行
- Task 8 依赖全部

# 实施备注

- `emInputDecodeKey` 未做成头文件内联函数（Keil C90 兼容性），解码逻辑以 `Menu_Task.c` 的 `static s_emMenuDecodeInput()` 实现，编码宏仍在 `Input_Operation.h`
- 列表帧选中行视觉：经阅读 `OLEDDevice.c` 确认 Negative 模式是"字模为1清点、为0置点"，即白底黑字高亮块，故采用 `>` 前缀 + 反显
- 单灯应用增加 `s_ucSingleActionValid` 标志，用于区分"尚未执行动作（显示 PRESS ENTER）"与"动作索引 0（ON）"
