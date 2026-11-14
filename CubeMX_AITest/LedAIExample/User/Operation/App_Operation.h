#ifndef _APP_OPERATION_H_
#define _APP_OPERATION_H_

#include <stdint.h>
#include "Input_Operation.h"
#include "OLED_Operation.h"

///	@brief					应用索引
///
///	@note					与App_Operation.c内函数表下标一一对应
#define	APP_CALCULATOR			0
#define	APP_LED_SINGLE			1
#define	APP_LED_ALL				2
#define	APP_GAME_SNAKE			3
#define	APP_NUM					4

///	@brief					应用接口表
///
///	@note					菜单只通过本结构体驱动应用，应用内部状态由各App模块自行维护
typedef struct
{
	const char *c_pcName;								//	应用名（调试用）
	void (*vEnter)(uint8_t ucParam);					//	进入应用，参数来自菜单项
	void (*vExit)(void);								//	退出应用
	void (*vHandleKey)(emInputKeyTdf emKey);			//	处理归一化按键
	void (*vTick)(void);								//	周期回调，可为NULL
	const stDisplayMsgTdf *(*c_pstRender)(void);		//	返回当前显示帧
}stAppInterfaceTdf;

///	@brief					按索引获取应用接口
///
///	@param			ucAppIndex		:	应用索引APP_xxx
///
///	@return					应用接口const指针；索引越界返回NULL
///
///	@note
const stAppInterfaceTdf *c_pstGetAppInterface(uint8_t ucAppIndex);

#endif
