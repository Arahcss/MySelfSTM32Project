#include "page_led_ctrl.h"
#include "Ui_Wight_Button.h"
#include "string.h"
#include "LedDevice.h"
#include "button_device.h"
#include "Project_Config.h"



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

///	@brief			按键处理
///
///	@note
void vButtonProcess()
{
	static uint8_t i=0;
	
	vBtnExecute(BUTTON_UP);
	vBtnExecute(BUTTON_DOWN);
	vBtnExecute(BUTTON_ENTER);
	vBtnExecute(BUTTON_CANCEL);
	
	if(emBtnGetEvent(BUTTON_UP))
	{
		vBtnClearEvent(BUTTON_UP);
		s_apstButton[i] -> emSelectedStatus = emButtonSelectStatus_IsNotSelected;
		
		i--;
		if(i>2)
		{
			i=2;
		}
		s_apstButton[i] -> emSelectedStatus = emButtonSelectStatus_IsSelected;
	}
	
	if(emBtnGetEvent(BUTTON_DOWN))
	{
		vBtnClearEvent(BUTTON_DOWN);
		s_apstButton[i] -> emSelectedStatus = emButtonSelectStatus_IsNotSelected;
		
		i++;
		if(i>2)
		{
			i=0;
		}
		s_apstButton[i] -> emSelectedStatus = emButtonSelectStatus_IsSelected;
	}
	
	if(emBtnGetEvent(BUTTON_ENTER))
	{
		vBtnClearEvent(BUTTON_DOWN);
				s_apstButton[i] -> emPressStatusCurrent = emButtonPressStatus_IsPressed;

	}
}

void vTest()
{
	vShowSingleLineTextAll();
	vShowButtonAll();
	
	vButtonCallbackExecuteAll();
	
	vButtonProcess();
	
	vOledRefreshFromBuffer(OLED);
}
