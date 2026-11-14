#include <stddef.h>
#include "App_Operation.h"
#include "CalcApp_Operation.h"
#include "LedApp_Operation.h"
#include "GameApp_Operation.h"

///	@brief					应用函数表
///
///	@note					下标必须与APP_xxx索引宏一致，表长必须等于APP_NUM
static const stAppInterfaceTdf s_astAppTable[APP_NUM] =
{
	{"CALC", vCalcAppEnter, vCalcAppExit, vCalcAppHandleKey, NULL, c_pstCalcAppRender},
	{"LED1", vLedSingleAppEnter, vLedSingleAppExit, vLedSingleAppHandleKey, NULL, c_pstLedSingleAppRender},
	{"LEDA", vLedAllAppEnter, vLedAllAppExit, vLedAllAppHandleKey, NULL, c_pstLedAllAppRender},
	{"GAME", vGameAppEnter, vGameAppExit, vGameAppHandleKey, NULL, c_pstGameAppRender},
};

///	@brief					按索引获取应用接口
///
///	@param			ucAppIndex		:	应用索引APP_xxx
///
///	@return					应用接口const指针；索引越界返回NULL
///
///	@note
const stAppInterfaceTdf *c_pstGetAppInterface(uint8_t ucAppIndex)
{
	if(ucAppIndex >= APP_NUM)
	{
		return NULL;
	}

	return &s_astAppTable[ucAppIndex];
}
