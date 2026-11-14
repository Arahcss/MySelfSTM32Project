#include "GameApp_Operation.h"

static stDisplayMsgTdf		s_stGameAppFrame;			//	游戏应用显示帧

///	@brief					写入文本并清零剩余字节
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcText		:	源字符串
///
///	@note					最多拷贝DISPLAY_LINE_LENGTH-1个字符，其余字节清0
static void s_vGameAppSetText(uint8_t *pucDst, const char *c_pcText)
{
	uint8_t ucIndex = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucIndex] != '\0'))
	{
		pucDst[ucIndex] = (uint8_t)c_pcText[ucIndex];
		ucIndex++;
	}

	while(ucIndex < DISPLAY_LINE_LENGTH)
	{
		pucDst[ucIndex] = 0;
		ucIndex++;
	}
}

///	@brief					重建游戏应用显示帧
///
///	@param					None
///
///	@note					三行内容：标题行提示 / 开发中 / 返回提示
static void s_vGameAppBuildFrame(void)
{
	stListDisplayMsgTdf *pstList = &s_stGameAppFrame.unFrame.stList;

	s_stGameAppFrame.emType = emDisplayFrameType_List;
	pstList->ucTitleLength = 4;
	s_vGameAppSetText(pstList->aucTitle, "GAME");
	pstList->ucLineCount = DISPLAY_LIST_LINE_NUM;
	pstList->ucSelectedIndex = 0xFF;
	pstList->aucReserved = 0;

	s_vGameAppSetText(pstList->aucLine[0], "SNAKE");
	s_vGameAppSetText(pstList->aucLine[1], "COMING SOON");
	s_vGameAppSetText(pstList->aucLine[2], "CANCEL: BACK");
}

///	@brief					进入游戏应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					占位实现，仅刷新静态提示帧
void vGameAppEnter(uint8_t ucParam)
{
	(void)ucParam;

	s_vGameAppBuildFrame();
}

///	@brief					退出游戏应用
///
///	@param					None
///
///	@note					占位实现，无操作
void vGameAppExit(void)
{
	/*	无操作	*/
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					占位实现，仅刷新静态提示帧
void vGameAppHandleKey(emInputKeyTdf emKey)
{
	(void)emKey;

	s_vGameAppBuildFrame();
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstGameAppRender(void)
{
	return &s_stGameAppFrame;
}
