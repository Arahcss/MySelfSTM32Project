#include "KeyPadDevice.h"
#include "string.h"

///	@brief			列拉低后到读取行电平的稳定等待循环次数
///
///	@note			72MHz下约10us，开漏加内部上拉驱动能力弱，不能立即读取
#define KEYPAD_SCAN_SETTLE_COUNT	200

stKeyPadDeviceParamTdf	astKeyPadDeviceParam[KEYPAD_DEV_NUM];
stKeyPadPinTdf			astKeyPadRowPin[KEYPAD_ROW_NUM];
stKeyPadPinTdf			astKeyPadColPin[KEYPAD_COL_NUM];

///	@brief				单键消抖与边沿事件生成
///
///	@param			emDevNum		:	设备编号
///	@param			emRawStatus		:	本次扫描得到的原始状态
///
///	@note				以本函数被调用的次数为消抖时间基准
static void s_vKeyPadDebounceUpdate(emKeyPadDevNumTdf emDevNum, emKeyPadStatusTdf emRawStatus)
{
	//原始状态与稳定状态一致则清零消抖计数
	if(emRawStatus == astKeyPadDeviceParam[emDevNum].stRunningParam.emCurrentStatus)
	{
		astKeyPadDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;
	}
	else
	{
		//消抖计数累加
		astKeyPadDeviceParam[emDevNum].stRunningParam.usDebounceCount++;

		//达到阈值后更新稳定状态
		if(astKeyPadDeviceParam[emDevNum].stRunningParam.usDebounceCount >= astKeyPadDeviceParam[emDevNum].stStaticParam.usDebounceThreshold)
		{
			astKeyPadDeviceParam[emDevNum].stRunningParam.emCurrentStatus = emRawStatus;

			astKeyPadDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;

			//生成边沿事件
			if(emRawStatus == emKeyPadStatus_Pressed)
			{
				astKeyPadDeviceParam[emDevNum].stRunningParam.emEvent = emKeyPadEvent_Press;
			}
			else
			{
				astKeyPadDeviceParam[emDevNum].stRunningParam.emEvent = emKeyPadEvent_Release;
			}
		}
	}
}

///	@brief				矩阵键盘周期执行（整屏扫描）
///
///	@param			None
///
///	@note				逐列拉低、逐行读取，行被上拉时读到低电平即该键按下；
///						需以固定周期调用，调用周期即消抖的时间基准；
///						多键跨行列同时按下时存在鬼键，本版本不做处理
void vKeyPadDevicePeriodExecute(void)
{
	uint8_t ucRowIndex;
	uint8_t ucColIndex;
	uint16_t usDelayIndex;
	GPIO_PinState emRowLevel;
	emKeyPadStatusTdf emRawStatus;
	emKeyPadDevNumTdf emDevNum;

	for(ucColIndex = 0; ucColIndex < KEYPAD_COL_NUM; ucColIndex++)
	{
		//当前列拉低，其余列保持开漏高（空闲高由内部上拉维持）
		HAL_GPIO_WritePin(astKeyPadColPin[ucColIndex].pstGpioBase, astKeyPadColPin[ucColIndex].usGpioPin, GPIO_PIN_RESET);

		//等待列电平稳定后再读取行
		for(usDelayIndex = 0; usDelayIndex < KEYPAD_SCAN_SETTLE_COUNT; usDelayIndex++)
		{
			__NOP();
		}

		//读取本列下所有行
		for(ucRowIndex = 0; ucRowIndex < KEYPAD_ROW_NUM; ucRowIndex++)
		{
			emRowLevel = HAL_GPIO_ReadPin(astKeyPadRowPin[ucRowIndex].pstGpioBase, astKeyPadRowPin[ucRowIndex].usGpioPin);

			//行被上拉，读到低电平说明该键按下
			if(emRowLevel == GPIO_PIN_RESET)
			{
				emRawStatus = emKeyPadStatus_Pressed;
			}
			else
			{
				emRawStatus = emKeyPadStatus_Released;
			}

			emDevNum = (emKeyPadDevNumTdf)(ucRowIndex * KEYPAD_COL_NUM + ucColIndex);

			s_vKeyPadDebounceUpdate(emDevNum, emRawStatus);
		}

		//释放当前列（开漏输出高为高阻，由内部上拉恢复空闲高）
		HAL_GPIO_WritePin(astKeyPadColPin[ucColIndex].pstGpioBase, astKeyPadColPin[ucColIndex].usGpioPin, GPIO_PIN_SET);
	}
}

///	@brief				获取按键设备参数
///
///	@param			emDevNum		:	设备编号
///
///	@note				返回const指针，仅供只读访问
const stKeyPadDeviceParamTdf	*c_pstGetKeyPadDeviceParam(emKeyPadDevNumTdf emDevNum)
{
	return &astKeyPadDeviceParam[emDevNum];
}

///	@brief				获取按键稳定状态
///
///	@param			emDevNum		:	设备编号
///
///	@note				消抖后的状态，不受 emKeyPadGetEvent 影响
emKeyPadStatusTdf emKeyPadGetStatus(emKeyPadDevNumTdf emDevNum)
{
	return astKeyPadDeviceParam[emDevNum].stRunningParam.emCurrentStatus;
}

///	@brief				读取并清除按键边沿事件
///
///	@param			emDevNum		:	设备编号
///
///	@note				读取后事件自动清零，避免重复处理
emKeyPadEventTdf emKeyPadGetEvent(emKeyPadDevNumTdf emDevNum)
{
	emKeyPadEventTdf emEvent;

	emEvent = astKeyPadDeviceParam[emDevNum].stRunningParam.emEvent;

	astKeyPadDeviceParam[emDevNum].stRunningParam.emEvent = emKeyPadEvent_None;

	return emEvent;
}

///	@brief				按键设备初始化
///
///	@param			pstInit			:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///
///	@note				本函数只记录静态参数，引脚模式由CubeMX配置
void vKeyPadDeviceInit(const stKeyPadStaticParamTdf *pstInit, emKeyPadDevNumTdf emDevNum)
{
	memcpy(&astKeyPadDeviceParam[emDevNum].stStaticParam, pstInit, sizeof(stKeyPadStaticParamTdf) / sizeof(uint8_t));
}

///	@brief				行引脚登记
///
///	@param			pstInit			:	引脚参数结构体的首地址
///	@param			ucRowIndex		:	行索引
///
///	@note				行引脚为上拉输入，模式由CubeMX配置
void vKeyPadDeviceRowPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucRowIndex)
{
	memcpy(&astKeyPadRowPin[ucRowIndex], pstInit, sizeof(stKeyPadPinTdf) / sizeof(uint8_t));
}

///	@brief				列引脚登记并置为空闲高电平
///
///	@param			pstInit			:	引脚参数结构体的首地址
///	@param			ucColIndex		:	列索引
///
///	@note				列引脚为开漏输出，模式由CubeMX配置
void vKeyPadDeviceColPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucColIndex)
{
	memcpy(&astKeyPadColPin[ucColIndex], pstInit, sizeof(stKeyPadPinTdf) / sizeof(uint8_t));

	HAL_GPIO_WritePin(astKeyPadColPin[ucColIndex].pstGpioBase, astKeyPadColPin[ucColIndex].usGpioPin, GPIO_PIN_SET);
}
