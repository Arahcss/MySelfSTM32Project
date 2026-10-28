# User 文件夹代码规范审计与修复计划

## 1. 概述（Summary）

以《STM32 工程编码规范》（主索引 + 编码规范/ 01~09 章）为基准，对 `User/` 目录下全部 7 个源文件进行逐章审计，修复发现的不合规项。本次审计发现的问题**全部为格式/注释风格类**，不涉及逻辑改动；另含 1 项接口可见性收敛（`static` 化）。修复后同步登记 08 文档。

涉及文件：

- `User/Device/LedDevice.c`
- `User/Device/LedDevice.h`
- `User/Operation/Led_Operation.c`
- `User/Task/Led_Task.c`
- `MDK-ARM/编码规范/08_已知不一致项.md`（登记，非代码）

合规无需改动的文件：`Led_Operation.h`、`Led_Task.h`、`Project_Config.h`（guard 已统一 `_X_H_`，命名、宏风格均合规）。

## 2. 现状分析（Current State Analysis）

### 2.1 已合规项（无需改动）

| 规范章节 | 审计结果 |
|----------|----------|
| 01 分层架构 | Task→Operation→Device 单向调用 ✓；仅 Device 层包含 HAL 头文件 ✓ |
| 02 文件规范 | 4 个头文件 guard 全部为 `_X_H_` ✓ |
| 03 命名规范 | `Tdf` 后缀、匈牙利前缀（`v/st/em/ul/us/uc/pst/a/s_`）、枚举显式赋初值、宏全大写，全部符合 ✓ |
| 06 数据组织 | 静态/运行参数分离、全局数组 + const 只读访问、memcpy 初始化、状态与电平解耦 ✓ |
| 07 RTOS 任务 | 原型已改为 `void vLedTask(void *pvParameters)`，骨架结构正确 ✓ |
| 09 修改范围 | 本次修复全部位于 `User/` 内，合规 ✓ |

### 2.2 不合规项清单

#### LedDevice.c

| # | 位置 | 问题 | 违反章节 |
|---|------|------|----------|
| C1 | L20 | `. ulOnCountThreshold` 成员访问符后多余空格 | 05 |
| C2 | L59 | `ulCurrentCount++;//闪烁时计数++` 行尾注释（规范要求注释置于代码上方一行） | 04.2 |
| C3 | L65 | `= 0;//清零Count` 行尾注释，同上 | 04.2 |
| C4 | L114 | `memcpy` 行多余一层缩进（2 Tab） | 05 |
| C5 | L164-165 | `vLedOff` 函数体内有多余空白行，`//设置当前状态` 注释行缩进错位（2 Tab） | 05 |
| C6 | L180 | `vLedToggle` 中 `=  ` 双空格、`(emLedStatusTdf)!` 强转后无空格，运算符间距不统一 | 05 |
| C7 | L126、L129 | 注释术语"电频"应为"电平"（与新增注释及规范用字一致） | 04 |
| C8 | L11 | `vLedDeviceBreathExecute` 未在头文件声明、仅被本文件 `vLedDevicePeriodExecute` 调用，却为非 `static`（规范 01：头文件只暴露接口函数，模块内部函数应收敛可见性） | 01（延伸） |
| C9 | 全文多处（L10、L46、L49、L68 等） | 行尾残留空白字符（`@note` 后、空行上的 Tab/空格） | 05 |

#### LedDevice.h

| # | 位置 | 问题 | 违反章节 |
|---|------|------|----------|
| H1 | L95-99 | `stLedDeviceParamTdf` 是同文件中唯一缺少 Doxygen 注释块的类型定义 | 04.1 |
| H2 | L97-98 | 成员注释列未对齐（`stRunningParam` 与 `stStaticParam` 的注释列错位） | 04.3 |
| H3 | L75 | 注释"电频"应为"电平" | 04 |
| H4 | L101 | `vLedOn` 声明行尾多余空格 | 05 |

#### Led_Operation.c

| # | 位置 | 问题 | 违反章节 |
|---|------|------|----------|
| O1 | 全文 | 函数体使用 4 空格缩进，规范要求 Tab | 05 |
| O2 | L19-21 | `vLedExecuteAll` 的 Doxygen 注释块为单行空格风格，与 04.1 模板（`///`+Tab 对齐列）不一致；`@param`/`@note` 后有多余空格 | 04.1 |
| O3 | L16 | `vLedDeviceInit(&stInit,emLedDevNum0);` 逗号后无空格 | 05 |
| O4 | L11、L24 | 空白行含残留空格 | 05 |

#### Led_Task.c

| # | 位置 | 问题 | 违反章节 |
|---|------|------|----------|
| T1 | 全文 | 函数体使用 4 空格缩进，规范要求 Tab | 05 |
| T2 | L6-7 | Doxygen 注释块缺 `@note`，`@param` 后无参数名说明，格式与模板不一致 | 04.1 |
| T3 | L12 | `while(1)` 与规范示例 `while (1)` 不一致 | 05 |
| T4 | L17 | 多余空白行含残留空格 | 05 |

## 3. 修改方案（Proposed Changes）

### 3.1 `User/Device/LedDevice.c`

- **C1**：`. ulOnCountThreshold` → `.ulOnCountThreshold`
- **C2**：L59 改为注释上移：
  ```c
  //闪烁时计数++
  astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount++;
  ```
- **C3**：L65 改为注释上移（if 块内，缩进 2 Tab）：
  ```c
  //清零Count
  astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount = 0;
  ```
- **C4**：`memcpy` 行缩进由 2 Tab 改为 1 Tab
- **C5**：删除 `vLedOff` 体内 L164 空白行，`//设置当前状态` 注释缩进修为 1 Tab，与 `vLedOn` 结构对齐
- **C6**：`emCurrentStatus  =  (emLedStatusTdf)!ast...` → `emCurrentStatus = (emLedStatusTdf)!ast...`（运算符两侧单空格）
- **C7**：两处 `//计算输出引脚电频`、`//更新LED输出电频` 中"电频"→"电平"
- **C8**：`vLedDeviceBreathExecute` 定义加 `static`：`static void vLedDeviceBreathExecute(emLedDevNumTdf emDevNum)`（其 Doxygen 注释块保留）
- **C9**：清理全文行尾空白（空白行、`@note` 后残留 Tab/空格）

### 3.2 `User/Device/LedDevice.h`

- **H1**：`stLedDeviceParamTdf` 上方补 Doxygen 注释块（风格同文件内其他 typedef）：
  ```c
  ///	@brief			LED设备参数总结构
  ///	
  ///	@note
  ```
- **H2**：成员声明与注释列对齐：
  ```c
  typedef struct
  {
      stLedRunningParamTdf stRunningParam;    //运行参数
      stLedStaticParamTdf  stStaticParam;     //静态参数
  }
  stLedDeviceParamTdf;
  ```
- **H3**：L75 `//	LED点亮时的电频` → `//	LED点亮时的电平`
- **H4**：清除 L101 行尾空格

### 3.3 `User/Operation/Led_Operation.c`

- **O1**：全文 4 空格缩进 → Tab
- **O2**：`vLedExecuteAll` 注释块改为规范模板：
  ```c
  ///	@brief					执行所有LED设备
  ///
  ///	@param					None
  ///
  ///	@note
  ```
  `vLedInitAll` 注释块列宽与之统一
- **O3**：`vLedDeviceInit(&stInit,emLedDevNum0);` → `vLedDeviceInit(&stInit, emLedDevNum0);`
- **O4**：清理空白行残留空格

### 3.4 `User/Task/Led_Task.c`

- **T1**：全文 4 空格缩进 → Tab
- **T2**：注释块规范化：
  ```c
  ///	@brief					LED任务
  ///
  ///	@param					pvParameters	:	FreeRTOS任务参数（未使用）
  ///
  ///	@note
  ```
- **T3**：`while(1)` → `while (1)`
- **T4**：清理多余空白行

### 3.5 文档登记

- 在 `MDK-ARM/编码规范/08_已知不一致项.md` 的"已修复"表追加本轮修复条目（修复日期 2026-10-29），涵盖 C1~C9、H1~H4、O1~O4、T1~T4。

## 4. 假设与决策（Assumptions & Decisions）

1. **C8 `static` 化决策**：`vLedDeviceBreathExecute` 仅被 `vLedDevicePeriodExecute`（同文件）调用且未在头文件声明，加 `static` 不改变任何外部行为，是规范 01"头文件只暴露接口函数"的自然延伸。若未来需在模块外调用，再移除 `static` 并补头文件声明。
2. **头文件函数声明不补 Doxygen 注释**：工程现行模式为注释集中在 `.c` 定义处（所有模块一致），本次保持该模式，仅对 `.h` 中缺失注释的**类型定义**（H1）补齐。
3. **续行超长 Tab 对齐保留**：`vLedDeviceBlinkExecute` 中多行条件的续行对齐（L54-55、L62-63）属原作者排版风格，规范未禁止，保持不动，避免产生无意义 diff。
4. **注释中的死代码保留**：`Led_Operation.c` L25 `// vLedDevicePeriodExecute(emLedDevNum0);` 为调试备用注释，规范未覆盖，保留。
5. 本轮修复**不改变任何运行行为**：仅格式、注释、`static` 可见性改动。

## 5. 验证（Verification）

1. Grep 全文检查无残留违规模式：
   - `astLedDeviceParam \[`、`\. ul`（成员符空格）
   - `[^/];//`（行尾注释紧跟分号）
   - `^    \S`（4 空格缩进残留）
   - `while\(`（关键字后无空格）
   - `电频`
2. 重读 4 个改动文件，人工核对缩进、注释块、对齐。
3. 由用户在 Keil 中编译确认无 warning/error（行为不变，预期编译结果与修复前一致）。
