#ifndef _CALCAPP_OPERATION_H_
#define _CALCAPP_OPERATION_H_

#include "Input_Operation.h"
#include "OLED_Operation.h"

///	@brief					进入计算器应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					复位计算器上下文并刷新首帧
void vCalcAppEnter(uint8_t ucParam);

///	@brief					退出计算器应用
///
///	@param					None
///
///	@note					无操作，计算器上下文在下次进入时复位
void vCalcAppExit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅矩阵键盘键（>=emInputKey_KeyPadBase）有效，板载键忽略
void vCalcAppHandleKey(emInputKeyTdf emKey);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					计算器帧const指针
const stDisplayMsgTdf *c_pstCalcAppRender(void);

#endif
