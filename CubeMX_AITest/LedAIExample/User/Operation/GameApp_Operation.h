#ifndef _GAMEAPP_OPERATION_H_
#define _GAMEAPP_OPERATION_H_

#include "Input_Operation.h"
#include "OLED_Operation.h"

///	@brief					进入游戏应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					重开一局贪吃蛇并构建网格帧
void vGameAppEnter(uint8_t ucParam);

///	@brief					退出游戏应用
///
///	@param					None
///
///	@note					无操作，蛇局状态保留到下次进入时重置
void vGameAppExit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					矩阵键盘2/4/6/8转向、11暂停、3重开；
///							Enter在结束/胜利时重开；Cancel由菜单框架处理
void vGameAppHandleKey(emInputKeyTdf emKey);

///	@brief					周期回调
///
///	@param					None
///
///	@return					1表示显示帧已变化需要刷新，0表示无变化
///
///	@note					每SNAKE_STEP_TICK_NUM个周期走一步，按状态重建帧
uint8_t ucGameAppTick(void);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					网格帧或列表帧const指针
const stDisplayMsgTdf *c_pstGameAppRender(void);

#endif
