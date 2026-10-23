 #ifndef	_PAGE_LED_CTRL_H_
#define	_PAGE_LED_CTRL_H_

#include "OLEDDevice.h"
#include "Ui_Wight_Single_Line_Text.h"

void vButtonCallback_On(void);
void vButtonCallback_Off(void);
void vButtonCallback_Toggle(void);

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

///	@brief			单行文本指针数组
///
///	@note
static stSingleLineTestTdf	*s_apstSingleLineTest[]	=
{
	&s_stSingleLineTest_Led0,
};

void vShowSingleLineTextAll(void);
void vShowButtonAll(void);
emButtonTriggerStatusTdf emButtonTriggerStatusScan(stButtonTdf *pstButton);
void vButtonCallback_On(void);
void vButtonCallback_Off(void);
void vButtonCallback_Toggle(void);
void vButtonCallbackExecuteAll(void);
void vPhyButtonExecuteAll(void);
void vButtonProcess(void);
void vTest(void);



#endif
