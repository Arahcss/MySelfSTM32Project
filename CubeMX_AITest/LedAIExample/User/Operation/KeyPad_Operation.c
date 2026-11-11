#include "KeyPad_Operation.h"

///	@brief			行引脚表（上拉输入，由CubeMX配置）
///
///	@note			改行引脚只改本表，顺序即行索引
static const stKeyPadPinTdf s_astKeyPadRowPin[KEYPAD_ROW_NUM] =
{
	{GPIOA, GPIO_PIN_6},
	{GPIOA, GPIO_PIN_5},
	{GPIOA, GPIO_PIN_4},
	{GPIOA, GPIO_PIN_3},
	{GPIOA, GPIO_PIN_2},
};

///	@brief			列引脚表（开漏输出，由CubeMX配置）
///
///	@note			改列引脚只改本表，顺序即列索引
static const stKeyPadPinTdf s_astKeyPadColPin[KEYPAD_COL_NUM] =
{
	{GPIOB, GPIO_PIN_10},
	{GPIOB, GPIO_PIN_1},
	{GPIOB, GPIO_PIN_0},
	{GPIOA, GPIO_PIN_7},
};

///	@brief					初始化所有矩阵键盘按键
///
///	@param					None
///
///	@note					引脚模式由CubeMX配置，此处只登记行列引脚与按键静态参数；
///							列引脚登记时已被置为空闲高电平
void vKeyPadInitAll(void)
{
	uint8_t ucRowIndex;
	uint8_t ucColIndex;
	stKeyPadStaticParamTdf stInit = {0};

	//登记行引脚
	for(ucRowIndex = 0; ucRowIndex < KEYPAD_ROW_NUM; ucRowIndex++)
	{
		vKeyPadDeviceRowPinInit(&s_astKeyPadRowPin[ucRowIndex], ucRowIndex);
	}

	//登记列引脚
	for(ucColIndex = 0; ucColIndex < KEYPAD_COL_NUM; ucColIndex++)
	{
		vKeyPadDeviceColPinInit(&s_astKeyPadColPin[ucColIndex], ucColIndex);
	}

	//按行列索引逐个登记按键
	for(ucRowIndex = 0; ucRowIndex < KEYPAD_ROW_NUM; ucRowIndex++)
	{
		for(ucColIndex = 0; ucColIndex < KEYPAD_COL_NUM; ucColIndex++)
		{
			stInit.ucRowIndex = ucRowIndex;
			stInit.ucColIndex = ucColIndex;
			stInit.usDebounceThreshold = KEYPAD_DEBOUNCE_THRESHOLD;

			vKeyPadDeviceInit(&stInit, (emKeyPadDevNumTdf)(ucRowIndex * KEYPAD_COL_NUM + ucColIndex));
		}
	}
}

///	@brief					执行一次矩阵键盘扫描
///
///	@param					None
///
///	@note					需以固定周期调用，建议10ms
void vKeyPadExecuteAll(void)
{
	vKeyPadDevicePeriodExecute();
}

///	@brief					采集一个按下事件的按键号
///
///	@param					None
///
///	@return					按下事件的按键设备号；无按下事件返回 KEYPAD_DEV_NUM
///
///	@note					每次调用只读清第一个Press事件，需循环调用以处理多键
emKeyPadDevNumTdf emKeyPadGetPressEvent(void)
{
	emKeyPadDevNumTdf emDevNum;

	for(emDevNum = emKeyPadDevNum0; emDevNum < KEYPAD_DEV_NUM; emDevNum++)
	{
		if(emKeyPadGetEvent(emDevNum) == emKeyPadEvent_Press)
		{
			return emDevNum;
		}
	}

	return KEYPAD_DEV_NUM;
}

///	@brief					获取当前按下的按键位掩码
///
///	@param					None
///
///	@return					位掩码，bit[n]对应设备号n，1为按下
///
///	@note					组合键扩展接口：后续可基于本掩码判断多键同时按下
uint32_t ulKeyPadGetPressedMask(void)
{
	uint32_t ulMask;
	emKeyPadDevNumTdf emDevNum;

	ulMask = 0;

	for(emDevNum = emKeyPadDevNum0; emDevNum < KEYPAD_DEV_NUM; emDevNum++)
	{
		if(emKeyPadGetStatus(emDevNum) == emKeyPadStatus_Pressed)
		{
			ulMask |= ((uint32_t)1 << (uint32_t)emDevNum);
		}
	}

	return ulMask;
}
