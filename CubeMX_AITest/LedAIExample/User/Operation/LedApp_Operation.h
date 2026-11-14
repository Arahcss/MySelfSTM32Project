#ifndef _LEDAPP_OPERATION_H_
#define _LEDAPP_OPERATION_H_

#include "Input_Operation.h"
#include "OLED_Operation.h"

///	@brief					进入单灯应用
///
///	@param			ucParam			:	灯号0~8，越界钳到板载灯
///
///	@note					每次进入动作索引复位，首次ENTER执行ON
void vLedSingleAppEnter(uint8_t ucParam);

///	@brief					退出单灯应用
///
///	@param					None
///
///	@note					无操作，LED保持进入时的最终状态
void vLedSingleAppExit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅ENTER有效：循环执行ON/OFF/TOGGLE/BREATH
void vLedSingleAppHandleKey(emInputKeyTdf emKey);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstLedSingleAppRender(void);

///	@brief					进入全部灯应用
///
///	@param			ucParam			:	动作索引0~3，越界钳到0
///
///	@note					进入即对全部灯执行一次该动作
void vLedAllAppEnter(uint8_t ucParam);

///	@brief					退出全部灯应用
///
///	@param					None
///
///	@note					无操作，LED保持进入时的最终状态
void vLedAllAppExit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅ENTER有效：对全部灯重复执行当前动作
void vLedAllAppHandleKey(emInputKeyTdf emKey);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstLedAllAppRender(void);

#endif
