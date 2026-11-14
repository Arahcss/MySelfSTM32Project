#include "OLED_Operation.h"

///	@brief				初始化所有OLED设备
///
///	@param				None
///
///	@note				SCL=PB8, SDA=PB9，推挽输出由CubeMX配置；
///						登记静态参数→初始化SSD1306→清显存→刷新；
///						运行参数全局数组为BSS段已清零，无需再memcpy初始化
void vOledInitAll(void)
{
	stOledStaticParamTdf stInit = {0};

	stInit.pstSclGpioBase = GPIOB;
	stInit.usSclGpioPin = GPIO_PIN_8;
	stInit.pstSdaGpioBase = GPIOB;
	stInit.usSdaGpioPin = GPIO_PIN_9;

	vOledDeviceInit(&stInit, OLED);

	vOledClearBuffer(OLED);
	vOledRefreshFromBuffer(OLED);
}

///	@brief				OLED显示测试
///
///	@param				None
///
///	@note				依次输出三种字号字符串与一条水平虚线，刷新到屏幕
void vOledTest(void)
{
	vOledClearBuffer(OLED);

	vOledWriteStringToBuffer(0, 0, (const uint8_t *)"Hello OLED",
			emOledFontSize_6x12, emOledPixelShowMode_Positive, OLED);

	vOledWriteStringToBuffer(0, 16, (const uint8_t *)"1234567890",
			emOledFontSize_8x16, emOledPixelShowMode_Positive, OLED);

	vOledDrawDotLine(0, 36, 128, OLED);

	vOledWriteStringToBuffer(0, 40, (const uint8_t *)"TEST",
			emOledFontSize_12x24, emOledPixelShowMode_Positive, OLED);

	vOledRefreshFromBuffer(OLED);
}

///	@brief			渲染一帧计算器界面
///
///	@param			pstMsg			:	已格式化的显示帧
///
///	@note			清显存→标题→表达式行→数值行→整帧刷新；
///					文本由计算模块生成，本函数不做数值转换
void vOledShowCalcFrame(const stCalcDisplayMsgTdf *pstMsg)
{
	vOledClearBuffer(OLED);

	vOledWriteStringToBuffer(0, 0, (const uint8_t *)"INT CALC 0-9999",
			emOledFontSize_6x12, emOledPixelShowMode_Positive, OLED);

	vOledWriteStringToBuffer(0, 16, pstMsg->aucExpression,
			emOledFontSize_8x16, emOledPixelShowMode_Positive, OLED);

	vOledWriteStringToBuffer(0, 36, pstMsg->aucValue,
			emOledFontSize_12x24, emOledPixelShowMode_Positive, OLED);

	vOledRefreshFromBuffer(OLED);
}

///	@brief			渲染一帧列表界面
///
///	@param			pstMsg			:	已格式化的列表显示帧
///
///	@note			清显存→标题→逐行→整帧刷新；
///					行文本缓冲固定DISPLAY_LINE_LENGTH字节，可能不含'\0'，
///					写屏前先拷贝到局部缓冲并按屏宽截断（8x16每字符8像素，
///					整行最多16字符；选中行'>'占一个字符位，故最多15字符）；
///					选中行采用反显：字模为1的点清除、为0的点置亮，
///					即笔画变黑、字符格底变白，形成明显的白色高亮块
void vOledShowListFrame(const stListDisplayMsgTdf *pstMsg)
{
	uint8_t i;
	uint8_t ucLineCount;
	uint8_t ucMaxCharNum;
	uint8_t ucCharNum;
	uint16_t usY;
	uint8_t aucLineBuf[DISPLAY_LINE_LENGTH];

	if(pstMsg == NULL)
	{
		return;
	}

	vOledClearBuffer(OLED);

	vOledWriteStringToBuffer(0, 0, pstMsg->aucTitle,
			emOledFontSize_6x12, emOledPixelShowMode_Positive, OLED);

	ucLineCount = pstMsg->ucLineCount;
	if(ucLineCount > DISPLAY_LIST_LINE_NUM)
	{
		ucLineCount = DISPLAY_LIST_LINE_NUM;
	}

	for(i = 0; i < ucLineCount; i++)
	{
		usY = (uint16_t)(16 + (i * 16));

		if(pstMsg->ucSelectedIndex == i)
		{
			ucMaxCharNum = 15;		//x=8起画，末字符右边界不超过127
		}
		else
		{
			ucMaxCharNum = 16;		//x=0起画，整行16字符
		}

		//按固定长度缓冲取有效字符，遇'\0'或缓冲区末尾即停止
		ucCharNum = 0;
		while((ucCharNum < ucMaxCharNum) && (ucCharNum < (DISPLAY_LINE_LENGTH - 1))
				&& (pstMsg->aucLine[i][ucCharNum] != '\0'))
		{
			aucLineBuf[ucCharNum] = pstMsg->aucLine[i][ucCharNum];
			ucCharNum++;
		}
		aucLineBuf[ucCharNum] = '\0';

		if(pstMsg->ucSelectedIndex == i)
		{
			vOledWriteStringToBuffer(0, usY, (const uint8_t *)">",
					emOledFontSize_8x16, emOledPixelShowMode_Positive, OLED);

			vOledWriteStringToBuffer(8, usY, aucLineBuf,
					emOledFontSize_8x16, emOledPixelShowMode_Negative, OLED);
		}
		else
		{
			vOledWriteStringToBuffer(0, usY, aucLineBuf,
					emOledFontSize_8x16, emOledPixelShowMode_Positive, OLED);
		}
	}

	vOledRefreshFromBuffer(OLED);
}

///	@brief			渲染一帧统一显示帧
///
///	@param			pstMsg			:	已格式化的显示帧
///
///	@note			按帧类型分发，未知类型不刷新屏幕
void vOledShowFrame(const stDisplayMsgTdf *pstMsg)
{
	if(pstMsg == NULL)
	{
		return;
	}

	switch(pstMsg->emType)
	{
		case emDisplayFrameType_Calc:
			vOledShowCalcFrame(&pstMsg->unFrame.stCalc);
			break;

		case emDisplayFrameType_List:
			vOledShowListFrame(&pstMsg->unFrame.stList);
			break;

		default:
			break;
	}
}
