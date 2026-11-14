#include <stddef.h>
#include "CalcApp_Operation.h"
#include "Calculator_Operation.h"

static stDisplayMsgTdf		s_stCalcAppFrame;			//	计算器应用显示帧

///	@brief					从计算器模块刷新显示帧
///
///	@param					None
///
///	@note					计算器模块的显示帧直接拷入统一帧的stCalc成员
static void s_vCalcAppRefresh(void)
{
	const stCalcDisplayMsgTdf *c_pstMsg = c_pstGetCalcDisplayMsg();

	if(c_pstMsg != NULL)
	{
		s_stCalcAppFrame.emType = emDisplayFrameType_Calc;
		s_stCalcAppFrame.unFrame.stCalc = *c_pstMsg;
	}
}

///	@brief					进入计算器应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					复位计算器上下文并刷新首帧
void vCalcAppEnter(uint8_t ucParam)
{
	(void)ucParam;

	vCalcInit();
	s_vCalcAppRefresh();
}

///	@brief					退出计算器应用
///
///	@param					None
///
///	@note					无操作，计算器上下文在下次进入时复位
void vCalcAppExit(void)
{
	/*	无操作	*/
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅矩阵键盘键（>=emInputKey_KeyPadBase）有效，板载键忽略
void vCalcAppHandleKey(emInputKeyTdf emKey)
{
	if(emKey >= emInputKey_KeyPadBase)
	{
		vCalcInputKey((uint8_t)(emKey - emInputKey_KeyPadBase));
		s_vCalcAppRefresh();
	}
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					计算器帧const指针
const stDisplayMsgTdf *c_pstCalcAppRender(void)
{
	return &s_stCalcAppFrame;
}
