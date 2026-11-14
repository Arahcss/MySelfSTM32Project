#ifndef _GAMEAPP_OPERATION_H_
#define _GAMEAPP_OPERATION_H_

#include "Input_Operation.h"
#include "OLED_Operation.h"

///	@brief					进入游戏应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					占位实现，仅刷新静态提示帧
void vGameAppEnter(uint8_t ucParam);

///	@brief					退出游戏应用
///
///	@param					None
///
///	@note					占位实现，无操作
void vGameAppExit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					占位实现，仅刷新静态提示帧
void vGameAppHandleKey(emInputKeyTdf emKey);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstGameAppRender(void);

#endif
