# OLED 驱动规范化重写 + 上层测试实现计划

## 1. 需求概述

将工程根目录 `Example/Device/` 下已有的 OLED（SSD1306 128×64，软件 I2C）字库与驱动，按本工程 `编码规范/01~09` 重写，并落到 `User/Example/` 下（规范 09：源码仅允许改 `User/` 内）。同时编写 Operation 层测试文件，上电初始化 OLED 并显示测试内容。

**硬件**：SCL=PB6，SDA=PB7（软件 I2C 位操作，CubeMX 需配为推挽输出），I2C 写地址 0x78。

## 2. 现状分析（Repository Research）

### 2.1 源文件

| 文件 | 位置 | 内容 |
|------|------|------|
| `OLEDDevice.h` | `Example/Device/` | 枚举（设备号/字号/像素模式）、静态/运行参数结构、显存、API 声明 |
| `OLEDDevice.c` | `Example/Device/` | 软件 I2C 位操作、显存读写、字符/字符串绘制、整屏刷新、命令初始化 |
| `OLEDFont.h` | `Example/Device/` | 三个字模数组 extern 声明 |
| `OLEDFont.c` | `Example/Device/` | ASCII 6×12 / 8×16 / 12×24 点阵数据（纯查表数据） |

### 2.2 不符合规范的点

| 类别 | 问题 | 处理 |
|------|------|------|
| 命名 | `vOledWirteStringToBuffer` 拼写错误（Wirte） | 改为 `vOledWriteStringToBuffer` |
| 宏 | `#define PI` 与 [LedDevice.h](file:///D:/zhuomian/32project/CubeMX_AITest/LedAIExample/User/Device/LedDevice.h#L7) 重复定义，且 OLED 未用 | 删除 |
| 头文件 | `#include "math.h"` 未使用 | 删除 |
| 注释 | 接口函数无 Doxygen 块；`/// @brief` 用空格而非 Tab | 补 `///\t@brief` 块 |
| 封装 | `vOledCmdInit` 非 static 但仅内部调用 | 改 `static` |
| 声明 | `vOledDeviceRunningParamInit`、`c_pstGetOledDeviceParam` 未在头文件声明 | 补声明 |
| 注释 | `stOledDeviceParamTdf` 内 `stRunningParam`/`stStaticParam` 注释写反 | 修正 |
| 风格 | 局部变量 `ii/jj/kk` 非匈牙利命名 | 改为 `usPage/usSeg/usMask` 等 |
| 数据 | `OLEDFont.c` 头部带第三方版权块 | 精简为规范文件头，点阵数据原样保留 |

### 2.3 保留不变的部分

- 软件 I2C 时序逻辑（Start/Stop/SendByte/WaitAck）
- SSD1306 初始化命令序列
- 显存布局（page×seg，128×8=1024 字节）
- 字模点阵数据（OLEDFont.c 三个数组逐字节复制，避免出错）

## 3. 文件清单（Files and Modules）

> 全部新建于 `User/Example/`，原 `Example/` 文件不动（规范 09）。

| 目标文件 | 改动 |
|----------|------|
| `User/Example/Device/OLEDDevice.h` | **重写**：删 PI/math.h、修正结构注释、补接口 Doxygen、补缺失声明、改函数名拼写 |
| `User/Example/Device/OLEDDevice.c` | **重写**：内部函数改 static、Tab 注释、匈牙利变量名、保持 I2C/绘制逻辑 |
| `User/Example/Device/OLEDFont.h` | **精简**：规范 guard + 数组声明 |
| `User/Example/Device/OLEDFont.c` | **复制数据**：精简文件头，点阵数组原样保留 |
| `User/Example/Operation/OLED_Operation.h` | **新增**：`vOledInitAll`、`vOledTest` 声明 |
| `User/Example/Operation/OLED_Operation.c` | **新增**：初始化（PB6/PB7）+ 测试显示（三行字符串 + 虚线 + 刷新） |

## 4. 实现步骤（Dependency-ordered）

1. **字库层**：重写 `OLEDFont.h`（规范头）；`OLEDFont.c` 复制点阵数据，仅改文件头注释。
2. **Device 头**：重写 `OLEDDevice.h`——类型/宏/结构/API 声明全部规范化。
3. **Device 实现**：重写 `OLEDDevice.c`——保留 I2C 与绘制算法，修正封装/命名/注释/变量。
4. **Operation 层**：新建 `OLED_Operation.h/.c`——`vOledInitAll` 登记 PB6(SCL)/PB7(SDA) 并执行命令初始化+清屏；`vOledTest` 绘制测试内容并刷新。
5. **CubeMX**（你操作）：PB6、PB7 配置为 `GPIO_Output`、推挽、无上拉、低速 → Generate Code。
6. **Keil**（你操作）：将 `User/Example/Device/` 下 4 个 `.c` 和 `User/Example/Operation/OLED_Operation.c` Add 进 target → 编译。

## 5. 关键设计细节

### 5.1 Device 层接口（OLEDDevice.h）

```c
// 设备号
typedef enum { emOledDevNum0 = 0 } emOledDevNumTdf;
#define OLED_DEV_NUM   1
#define OLED           emOledDevNum0

// 字号
typedef enum {
    emOledFontSize_6x12  = 12,
    emOledFontSize_8x16  = 16,
    emOledFontSize_12x24 = 24,
} emOledFontSizeTdf;

// 像素模式
typedef enum {
    emOledPixelShowMode_Positive = 0,
    emOledPixelShowMode_Negative,
} emOledPixelShowModeTdf;

// 静态参数：SCL/SDA 的 GPIO 端口与引脚
typedef struct {
    GPIO_TypeDef *pstSclGpioBase;
    uint16_t      usSclGpioPin;
    GPIO_TypeDef *pstSdaGpioBase;
    uint16_t      usSdaGpioPin;
} stOledStaticParamTdf;

// 运行参数：显存
typedef struct {
    uint8_t aucOledBuffer[OLED_BUFFER_WIDTH * OLED_BUFFER_HEIGHT];
} stOledRunningParamTdf;

typedef struct {
    stOledRunningParamTdf stRunningParam;
    stOledStaticParamTdf  stStaticParam;
} stOledDeviceParamTdf;

void vOledDeviceInit(stOledStaticParamTdf *pstInit, emOledDevNumTdf emDevNum);
void vOledDeviceRunningParamInit(stOledRunningParamTdf *pstInit, emOledDevNumTdf emDevNum);
const stOledDeviceParamTdf *c_pstGetOledDeviceParam(emOledDevNumTdf emDevNum);
void vOledDrawOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum);
void vOledClearOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum);
void vOledWriteOneCharToBuffer(uint32_t ulX, uint32_t ulY, uint8_t ucChar,
                               emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode,
                               emOledDevNumTdf emDevNum);
void vOledWriteStringToBuffer(uint16_t usX, uint16_t usY, const uint8_t *c_pucChar,
                              emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode,
                              emOledDevNumTdf emDevNum);
void vOledDrawDotLine(uint16_t usX, uint16_t usY, uint32_t ulLength, emOledDevNumTdf emDevNum);
void vOledRefreshFromBuffer(emOledDevNumTdf emDevNum);
void vOledClearBuffer(emOledDevNumTdf emDevNum);   // 新增：清显存
```

> `vOledClearBuffer` 为新增接口（原驱动只有画点无清屏，测试需要）。

### 5.2 Operation 层（OLED_Operation.c）

```c
void vOledInitAll(void)
{
    stOledStaticParamTdf stInit = {0};
    stOledRunningParamTdf stRunningInit = {0};

    stInit.pstSclGpioBase = GPIOB;
    stInit.usSclGpioPin   = GPIO_PIN_6;
    stInit.pstSdaGpioBase = GPIOB;
    stInit.usSdaGpioPin   = GPIO_PIN_7;

    vOledDeviceInit(&stInit, OLED);
    vOledDeviceRunningParamInit(&stRunningInit, OLED);
    vOledClearBuffer(OLED);
    vOledRefreshFromBuffer(OLED);
}

void vOledTest(void)
{
    vOledClearBuffer(OLED);
    vOledWriteStringToBuffer(0, 0,  (uint8_t *)"Hello OLED",
                             emOledFontSize_6x12, emOledPixelShowMode_Positive, OLED);
    vOledWriteStringToBuffer(0, 16, (uint8_t *)"1234567890",
                             emOledFontSize_8x16, emOledPixelShowMode_Positive, OLED);
    vOledWriteStringToBuffer(0, 40, (uint8_t *)"TEST",
                             emOledFontSize_12x24, emOledPixelShowMode_Positive, OLED);
    vOledDrawDotLine(0, 36, 128, OLED);
    vOledRefreshFromBuffer(OLED);
}
```

## 6. 依赖与注意事项

- **软件 I2C**：驱动用 GPIO 寄存器直接位操作（BSRR），无需 HAL 延时；72MHz 下时序满足 SSD1306。
- **PB6/PB7**：CubeMX 必须配为 `GPIO_MODE_OUTPUT_PP`，切勿配成 I2C1 外设（否则被 HAL 接管）。
- **显存 1024 字节**：`stOledRunningParamTdf` 含 `uint8_t [1024]`，作为全局参数数组的一部分占 RAM，F103C8（20KB RAM）可接受。
- **字库数据**：`OLEDFont.c` 三个数组约 95×(12+16+36)≈6KB Flash，原样复制不改动。
- **测试调用位置**：`vOledInitAll` + `vOledTest` 可在 `main.c` 的 `MX_FREERTOS_Init` 之前（`USER CODE BEGIN 2`）调用，或新建 OLED 任务；本计划先提供可直接调用的接口，由你决定挂接位置。

## 7. 验证

1. **编译**：Keil 0 error 0 warning。
2. **静态检查**：Grep 确认无 4 空格缩进（字库数据区除外）、无行尾空白、guard 配对、无 `PI` 重复定义。
3. **运行验证**：
   - 调用 `vOledInitAll()` 后 OLED 全屏熄灭。
   - 调用 `vOledTest()` 后显示：第 1 行 "Hello OLED"（6×12）、第 2 行 "1234567890"（8×16）、一条虚线、第 4 行 "TEST"（12×24）。

## 8. 风险与处理

| 风险 | 处理 |
|------|------|
| 字库数据复制出错导致显示乱码 | 逐字节原样复制，编译后用模拟器/硬件验证字形 |
| PB6/PB7 被 CubeMX 配成 I2C1 | 配置时选 `GPIO_Output`，不选 I2C 外设 |
| 软件 I2C 时序过快导致不识别 | F103 72MHz 下位操作约几十 ns，SSD1306 支持；如异常可在 SCL 翻转间加 `__NOP()` |
| `vOledClearBuffer` 新增接口需同步声明 | 头文件与实现同步添加 |
