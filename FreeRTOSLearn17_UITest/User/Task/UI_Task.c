#include "freertos.h"
#include "task.h"
#include "UI_Task.h"
#include "OLEDDevice.h"

///	@brief			[单行文本]数据结构
///
///	@note
typedef struct
{
	uint32_t						x;
	uint32_t						y;
	uint8_t						*ucData;
	emOledFontSizeTdf		emFontSize;
}stSingleLineTestTdf;

static uint8_t	s_aucHello[]		=	"Hello World";
static uint8_t	s_aucLetter[]		=	"ABCD";
static uint8_t	s_aucNumber[]	=	"1234";

///	@brief			[Hellow]单行文本
///
///	@note
static stSingleLineTestTdf s_stSingleLineTest_Hello	=
{
	.x					=	0,
	.y					=	0,
	.ucData		=	s_aucHello,
	.emFontSize	=	emOledFontSize_8x16,
};

///	@brief			[字母]单行文本
///
///	@note
static stSingleLineTestTdf s_stSingleLineTest_Letter	=
{
	.x					=	0,
	.y					=	0,
	.ucData		=	s_aucLetter,
	.emFontSize	=	emOledFontSize_6x12,
};

///	@brief			[数字]单行文本
///
///	@note
static stSingleLineTestTdf s_stSingleLineTest_Number	=
{
	.x					=	20,
	.y					=	0,
	.ucData		=	s_aucNumber,
	.emFontSize	=	emOledFontSize_12x24,
};



///	@brief			OLED初始化
///
///	@note
void vOledInit()
{
	stOledStaticParamTdf stInit;
	
	stInit.pstSclGpioBase	=	GPIOB;
	stInit.usSclGpioPin		=	GPIO_PIN_8;
	stInit.pstSdaGpioBase	=	GPIOB;
	stInit.usSdaGpioPin	 	=	GPIO_PIN_9;
	
	vOledDeviceInit(&stInit,OLED);
}

///	@brief			单行文本指针数组
///
///	@note
static stSingleLineTestTdf	*s_apstSingleLineTest[]	=
{
	&s_stSingleLineTest_Hello,
	&s_stSingleLineTest_Letter,
	&s_stSingleLineTest_Number,
};

void vTest()
{
	uint32_t i;
	
	for(i = 0;i < sizeof(s_apstSingleLineTest) / sizeof(s_apstSingleLineTest[0]);i++)
	{
		vUiWirteStringToBuffer(s_apstSingleLineTest[i] -> x,
												s_apstSingleLineTest[i] -> y,
												s_apstSingleLineTest[i] -> ucData,
												s_apstSingleLineTest[i] -> emFontSize,
												emOledPixelShowMode_Positive,
												OLED);
	}
	
	vOledRefreshFromBuffer(OLED);
}


///	@brief			UI任务
///
///	@note
void vUITask()
{
	vOledInit();
	
	while(1)
	{
//		vUiWirteStringToBuffer(0,0,(const uint8_t*)"123",emOledFontSize_8x16,emOledPixelShowMode_Positive,OLED);
//		
//		vOledRefreshFromBuffer(OLED);
		vTest();
		
		vTaskDelay(1);
	}
}

