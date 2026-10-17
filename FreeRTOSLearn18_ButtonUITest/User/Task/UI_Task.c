#include "freertos.h"
#include "task.h"
#include "UI_Task.h"
#include "OLEDDevice.h"
#include "string.h"

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

static uint8_t	s_aucLed0[]			=	"Led0";
static uint8_t	s_aucOn[]			=	"ON";
static uint8_t	s_aucOff[]			=	"OFF";
static uint8_t	s_aucToggle[]		=	"TOGGLE";

///	@brief			[Led0]单行文本
///
///	@note
static stSingleLineTestTdf s_stSingleLineTest_Led0	=
{
	.x					=	64 - (sizeof(s_aucLed0) / sizeof(s_aucLed0[0]) - 1) / 2 * emOledFontSize_8x16 / 2,
	.y					=	0,
	.ucData		=	s_aucLed0,
	.emFontSize	=	emOledFontSize_8x16,
};

///	@brief			按钮回调函数
///
///	@note
typedef void vfButtonCallbackTdf(void);

///	@brief			按钮选中状态
///
///	@note
typedef enum
{
	emButtonSelectStatus_IsNotSelected		=	0,
	emButtonSelectStatus_IsSelected				=	1,
}emButtonSelectStatusTdf;

///	@brief			按钮按下状态
///
///	@note
typedef enum
{
	emButtonPressStatus_IsNotPressed			=	0,
	emButtonPressStatus_IsPressed				=	1,
}emButtonPressStatusTdf;

///	@brief			[按钮]数据结构
///
///	@note
typedef struct
{
	uint32_t									x;
	uint32_t									y;
	uint8_t									*ucData;
	emOledFontSizeTdf					emFontSize;
	emButtonSelectStatusTdf			emSelectedStatus;
	emButtonPressStatusTdf			emPressStatus;
	uint32_t									ulPressedCount;
	uint32_t									ulPressedCountThreshold;
	vfButtonCallbackTdf				*vpCallback;
}stButtonTdf;

///	@brief			[ON]按钮
///
///	@note
static stButtonTdf s_sButton_On = 
{
	.x												=	20,
	.y												=	1.2	*	emOledFontSize_8x16,
	.ucData									=	s_aucOn,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsSelected,
	.emPressStatus							=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpCallback								=	0,
};

///	@brief			[OFF]按钮
///
///	@note
static stButtonTdf s_sButton_Off	=
{
	.x												=	80,
	.y												=	1.2	*	emOledFontSize_8x16,
	.ucData									=	s_aucOff,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsNotSelected,
	.emPressStatus							=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpCallback								=	0,
};

///	@brief			[TOGGLE]按钮
///
///	@note
static stButtonTdf s_sButton_Toggle	=
{
	.x												=	20,
	.y												=	2.5	*	emOledFontSize_8x16,
	.ucData									=	s_aucToggle,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsNotSelected,
	.emPressStatus							=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpCallback								=	0,
};

///	@brief			按钮指针数组
///
///	@note
static stButtonTdf *s_apstButton[]	=
{
	&s_sButton_On,
	&s_sButton_Off,
	&s_sButton_Toggle,
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
	&s_stSingleLineTest_Led0,
};

///	@brief			显示所有单行文本
///
///	@note
void vShowSingleLineTextAll()
{
	uint32_t i;
	
	for(i = 0;i < sizeof(s_apstSingleLineTest) / sizeof(s_apstSingleLineTest[0]);i++)
	{
		vOledWirteStringToBuffer(s_apstSingleLineTest[i] -> x,
												s_apstSingleLineTest[i] -> y,
												s_apstSingleLineTest[i] -> ucData,
												s_apstSingleLineTest[i] -> emFontSize,
												emOledPixelShowMode_Positive,
												OLED);
	}
	
	vOledRefreshFromBuffer(OLED);
}

///	@brief			显示所有按钮
///
///	@note
void vShowButtonAll()
{
	uint32_t i;
	
	for(i = 0;i < sizeof(s_apstButton) / sizeof(s_apstButton[0]);i++)
	{
		if(s_apstButton[i] -> emPressStatus == emButtonPressStatus_IsNotPressed)
		{
			vOledWirteStringToBuffer(s_apstButton[i] -> x,
												s_apstButton[i] -> y,
												s_apstButton[i] -> ucData,
												s_apstButton[i] -> emFontSize,
												emOledPixelShowMode_Positive,
												OLED);
		}
		else
		{
			vOledWirteStringToBuffer(s_apstButton[i] -> x,
												s_apstButton[i] -> y,
												s_apstButton[i] -> ucData,
												s_apstButton[i] -> emFontSize,
												emOledPixelShowMode_Negative,
												OLED);
		}
		
		//选中的按键上方加虚线
		if(s_apstButton[i]->emSelectedStatus == emButtonSelectStatus_IsSelected)
		{
			vOledDrawDotLine(s_apstButton[i]->x,
											s_apstButton[i]->y,
											strlen((char*)s_apstButton[i]-> ucData) * s_apstButton[i]->emFontSize / 2,
											OLED);
		}
		
		//底部加虚线
		vOledDrawDotLine(s_apstButton[i]->x,
											s_apstButton[i]->y + s_apstButton[i]->emFontSize,
											strlen((char*)s_apstButton[i]-> ucData) * s_apstButton[i]->emFontSize / 2,
											OLED);
	}
}

void vTest()
{
	uint32_t i;
	vShowSingleLineTextAll();
	vShowButtonAll();
	
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

		vTest();
		
		vTaskDelay(1);
	}
}

