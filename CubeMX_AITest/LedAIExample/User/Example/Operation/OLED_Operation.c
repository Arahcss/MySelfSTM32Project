#include "OLED_Operation.h"

///	@brief				初始化所有OLED设备
///
///	@param				None
///
///	@note				SCL=PB6, SDA=PB7，推挽输出由CubeMX配置；
///						登记静态参数→初始化SSD1306→清显存→刷新
void vOledInitAll(void)
{
	stOledStaticParamTdf stInit = {0};
	stOledRunningParamTdf stRunningInit = {0};

	stInit.pstSclGpioBase = GPIOB;
	stInit.usSclGpioPin = GPIO_PIN_6;
	stInit.pstSdaGpioBase = GPIOB;
	stInit.usSdaGpioPin = GPIO_PIN_7;

	vOledDeviceInit(&stInit, OLED);
	vOledDeviceRunningParamInit(&stRunningInit, OLED);

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
