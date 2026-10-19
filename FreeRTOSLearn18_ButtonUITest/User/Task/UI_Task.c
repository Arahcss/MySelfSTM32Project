#include "freertos.h"
#include "task.h"
#include "UI_Task.h"
#include "OLEDDevice.h"
#include "LedDevice.h"
#include "button_device.h"
#include "string.h"

void vButtonCallback_On(void);
void vButtonCallback_Off(void);
void vButtonCallback_Toggle(void);

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

///	@brief			按钮触发状态
///
///	@note
typedef enum
{
	emButtonTriggerStatus_IsNotTriggered	=	0,
	emButtonTriggerStatus_IsTriggered		=	1,
}emButtonTriggerStatusTdf;

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
	emButtonPressStatusTdf			emPressStatusCurrent;
	emButtonPressStatusTdf 			emPressStatusLast;
	uint32_t									ulPressedCount;
	uint32_t									ulPressedCountThreshold;
	vfButtonCallbackTdf				*vpfCallback;
}stButtonTdf;

///	@brief			[ON]按钮
///
///	@note
static stButtonTdf s_stButton_On = 
{
	.x												=	20,
	.y												=	1.2	*	emOledFontSize_8x16,
	.ucData									=	s_aucOn,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsSelected,
	.emPressStatusCurrent			=	emButtonPressStatus_IsNotPressed,
	.emPressStatusLast					=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpfCallback								=	vButtonCallback_On,
};

///	@brief			[OFF]按钮
///
///	@note
static stButtonTdf s_stButton_Off	=
{
	.x												=	80,
	.y												=	1.2	*	emOledFontSize_8x16,
	.ucData									=	s_aucOff,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsNotSelected,
	.emPressStatusCurrent			=	emButtonPressStatus_IsNotPressed,
	.emPressStatusLast					=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpfCallback								=	vButtonCallback_Off,
};

///	@brief			[TOGGLE]按钮
///
///	@note
static stButtonTdf s_stButton_Toggle	=
{
	.x												=	20,
	.y												=	2.5	*	emOledFontSize_8x16,
	.ucData									=	s_aucToggle,
	.emFontSize								=	emOledFontSize_8x16,
	.emSelectedStatus					=	emButtonSelectStatus_IsNotSelected,
	.emPressStatusCurrent			=	emButtonPressStatus_IsNotPressed,
	.emPressStatusLast					=	emButtonPressStatus_IsNotPressed,
	.ulPressedCount						=	0,
	.ulPressedCountThreshold		=	5,
	.vpfCallback								=	vButtonCallback_Toggle,
};

///	@brief			按钮指针数组
///
///	@note
static stButtonTdf *s_apstButton[]	=
{
	&s_stButton_On,
	&s_stButton_Off,
	&s_stButton_Toggle,
};

///	@brief			LED初始化
///
///	@note
void vLedInit()
{
	stLedStaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_13;
	stInit.emOnLevel			=	emLedOnLevel_Low;
	
	vLedDeviceInit(&stInit,LED_BOARD);
}

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

///	@brief			BUTTON初始化
///
///	@note
void vButtonInit()
{
	stBtnStaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOA;
	stInit.usGpioPin			=	GPIO_PIN_1;
	stInit.emActiveLevel			=	emBtnActiveLevel_High;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_UP);
	
	stInit.pstGpioBase		=	GPIOA;
	stInit.usGpioPin			=	GPIO_PIN_0;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_DOWN);
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_15;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_ENTER);
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_14;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_CANCEL);
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
		if(s_apstButton[i] -> emPressStatusCurrent == emButtonPressStatus_IsNotPressed)
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

///	@brief			按钮触发状态扫描
///
///	@note
emButtonTriggerStatusTdf emButtonTriggerStatusScan(stButtonTdf *pstButton)
{
	emButtonTriggerStatusTdf emTriggerStatus = emButtonTriggerStatus_IsNotTriggered;
	//判断是否触发
	if((pstButton->emPressStatusCurrent == emButtonPressStatus_IsPressed) && 
			(pstButton->emPressStatusLast == emButtonPressStatus_IsNotPressed))
	{
		emTriggerStatus = emButtonTriggerStatus_IsTriggered;
		pstButton->ulPressedCount	= 0;
	}
	
	//判断按下时间,超过阈值按下状态复位
	pstButton->ulPressedCount++;
	if(pstButton->ulPressedCount > pstButton->ulPressedCountThreshold)
	{
		pstButton->ulPressedCount = 0;
		pstButton->emPressStatusCurrent = emButtonPressStatus_IsNotPressed;
	}
	
	 pstButton->emPressStatusLast = pstButton->emPressStatusCurrent;
	
	return emTriggerStatus;
}

///	@brief			[ON]按钮回调
///
///	@note
void vButtonCallback_On()
{
	if(emButtonTriggerStatusScan(&s_stButton_On) == emButtonTriggerStatus_IsTriggered)
	{
		vLedOn(LED_BOARD);
	}
}

///	@brief			[OFF]按钮回调
///
///	@note
void vButtonCallback_Off()
{
	if(emButtonTriggerStatusScan(&s_stButton_Off) == emButtonTriggerStatus_IsTriggered)
	{
		vLedOFF(LED_BOARD);
	}
}

///	@brief			[TOGGLE]按钮回调
///
///	@note
void vButtonCallback_Toggle()
{
	if(emButtonTriggerStatusScan(&s_stButton_Toggle) == emButtonTriggerStatus_IsTriggered)
	{
		vLedToggle(LED_BOARD);
	}
}

///	@brief			执行所有按钮的回调函数
///
///	@note
void vButtonCallbackExecuteAll()
{
	uint32_t i;
	
	for(i=0; i< sizeof(s_apstButton) / sizeof(s_apstButton[0]);i++)
	{
		if(s_apstButton[i]->vpfCallback == 0)
		{
			continue;
		}
		
		s_apstButton[i] -> vpfCallback();
	}
}

///	@brief			执行所有物理按键的行为
///
///	@note
void vPhyButtonExecuteAll()
{
	uint8_t i;
	static uint8_t s_ucSelectIndex	=	0;
	
	for(i = 0; i < BTN_DEV_NUM;i++)
	{
		vBtnExecute((emBtnDevNumTdf)i);
	}
	
	//按键UP
	if(emBtnGetEvent(BUTTON_UP) == emBtnEvent_Click)
	{
		vBtnClearEvent(BUTTON_UP);
		
		s_apstButton[s_ucSelectIndex]->emSelectedStatus = emButtonSelectStatus_IsNotSelected;
		
		s_ucSelectIndex--;
		if(s_ucSelectIndex > (sizeof(s_apstButton) / sizeof(s_apstButton[0])-1))
		{
			s_ucSelectIndex = (sizeof(s_apstButton) / sizeof(s_apstButton[0])-1);
		}
		s_apstButton[s_ucSelectIndex]-> emSelectedStatus = emButtonSelectStatus_IsSelected;
	}
	
	//按键DOWN
	if(emBtnGetEvent(BUTTON_DOWN) == emBtnEvent_Click)
	{
		vBtnClearEvent(BUTTON_DOWN);
		
		s_apstButton[s_ucSelectIndex]->emSelectedStatus = emButtonSelectStatus_IsNotSelected;
		
		s_ucSelectIndex++;
		if(s_ucSelectIndex > (sizeof(s_apstButton) / sizeof(s_apstButton[0])-1))
		{
			s_ucSelectIndex = 0;
		}
		s_apstButton[s_ucSelectIndex]-> emSelectedStatus = emButtonSelectStatus_IsSelected;
	}
	
	//按键ENTER
	if(emBtnGetEvent(BUTTON_ENTER) == emBtnEvent_Click)
	{
		vBtnClearEvent(BUTTON_ENTER);

		s_apstButton[s_ucSelectIndex]-> emSelectedStatus = emButtonSelectStatus_IsSelected;
	}
	
}

void vTest()
{
	vShowSingleLineTextAll();
	vShowButtonAll();
	
	vButtonCallbackExecuteAll();
	
	vPhyButtonExecuteAll();
	
	vOledRefreshFromBuffer(OLED);
}


///	@brief			UI任务
///
///	@note
void vUITask()
{
	vOledInit();
	vLedInit();
	vButtonInit();
	
	while(1)
	{

		vTest();
		
		vTaskDelay(1);
	}
}

