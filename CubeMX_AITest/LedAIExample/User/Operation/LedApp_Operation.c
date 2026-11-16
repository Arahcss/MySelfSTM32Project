#include "LedApp_Operation.h"
#include "Led_Operation.h"

#define	LED_APP_ACTION_NUM		4				//动作表长度

///	@brief					动作表
///
///	@note					下标即动作索引，单灯与全部灯应用共用
static const emLedActionTdf s_aemSingleActions[] =
{
	emLedAction_On, emLedAction_Off, emLedAction_Toggle, emLedAction_Breath
};

///	@brief					动作名表
///
///	@note					下标与s_aemSingleActions一一对应
static const char *c_apcSingleActionName[] =
{
	"ON", "OFF", "TOGGLE", "BREATH"
};

static uint8_t				s_ucSingleLedIndex;			//	单灯应用灯号
static uint8_t				s_ucSingleActionIndex;		//	单灯应用动作索引
static uint8_t				s_ucSingleActionValid;		//	单灯应用是否已执行过动作
static stDisplayMsgTdf		s_stSingleFrame;			//	单灯应用显示帧

static uint8_t				s_ucAllActionIndex;			//	全部灯应用动作索引
static stDisplayMsgTdf		s_stAllFrame;				//	全部灯应用显示帧

///	@brief					写入文本并清零剩余字节
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcText		:	源字符串
///
///	@note					最多拷贝DISPLAY_LINE_LENGTH-1个字符，其余字节清0
static void s_vLedAppSetText(uint8_t *pucDst, const char *c_pcText)
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

///	@brief					在已有文本末尾追加文本
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcText		:	待追加的源字符串
///
///	@note					超出DISPLAY_LINE_LENGTH-1的部分截断，剩余字节清0
static void s_vLedAppAppendText(uint8_t *pucDst, const char *c_pcText)
{
	uint8_t ucIndex = 0;
	uint8_t ucSrc = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (pucDst[ucIndex] != '\0'))
	{
		ucIndex++;
	}

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucSrc] != '\0'))
	{
		pucDst[ucIndex] = (uint8_t)c_pcText[ucSrc];
		ucIndex++;
		ucSrc++;
	}

	while(ucIndex < DISPLAY_LINE_LENGTH)
	{
		pucDst[ucIndex] = 0;
		ucIndex++;
	}
}

///	@brief					计算文本截断后的有效字符数
///
///	@param			c_pcText		:	源字符串
///
///	@return					有效字符数，最大DISPLAY_LINE_LENGTH-1
///
///	@note
static uint8_t s_ucLedAppTextLength(const char *c_pcText)
{
	uint8_t ucIndex = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucIndex] != '\0'))
	{
		ucIndex++;
	}

	return ucIndex;
}

///	@brief					重建单灯应用显示帧
///
///	@param					None
///
///	@note					三行内容：灯号 / 当前动作 / 返回提示
static void s_vLedSingleBuildFrame(void)
{
	stListDisplayMsgTdf *pstList = &s_stSingleFrame.unFrame.stList;

	s_stSingleFrame.emType = emDisplayFrameType_List;
	pstList->ucTitleLength = s_ucLedAppTextLength("SINGLE LED");
	s_vLedAppSetText(pstList->aucTitle, "SINGLE LED");
	pstList->ucLineCount = DISPLAY_LIST_LINE_NUM;
	pstList->ucSelectedIndex = 0xFF;
	pstList->aucReserved = 0;

	//第一行：灯号
	s_vLedAppSetText(pstList->aucLine[0], "LED");
	pstList->aucLine[0][3] = (uint8_t)('0' + s_ucSingleLedIndex);
	pstList->aucLine[0][4] = '\0';

	//第二行：动作名或提示
	if(s_ucSingleActionValid != 0)
	{
		s_vLedAppSetText(pstList->aucLine[1], "ACTION: ");
		s_vLedAppAppendText(pstList->aucLine[1], c_apcSingleActionName[s_ucSingleActionIndex]);
	}
	else
	{
		s_vLedAppSetText(pstList->aucLine[1], "PRESS ENTER");
	}

	//第三行：返回提示
	s_vLedAppSetText(pstList->aucLine[2], "CANCEL: BACK");
}

///	@brief					重建全部灯应用显示帧
///
///	@param					None
///
///	@note					三行内容：当前动作 / 重复执行提示 / 返回提示
static void s_vLedAllBuildFrame(void)
{
	stListDisplayMsgTdf *pstList = &s_stAllFrame.unFrame.stList;

	s_stAllFrame.emType = emDisplayFrameType_List;
	pstList->ucTitleLength = s_ucLedAppTextLength("ALL LED");
	s_vLedAppSetText(pstList->aucTitle, "ALL LED");
	pstList->ucLineCount = DISPLAY_LIST_LINE_NUM;
	pstList->ucSelectedIndex = 0xFF;
	pstList->aucReserved = 0;

	//第一行：动作名
	s_vLedAppSetText(pstList->aucLine[0], "ACTION: ");
	s_vLedAppAppendText(pstList->aucLine[0], c_apcSingleActionName[s_ucAllActionIndex]);

	//第二行：重复执行提示
	s_vLedAppSetText(pstList->aucLine[1], "ENTER: AGAIN");

	//第三行：返回提示
	s_vLedAppSetText(pstList->aucLine[2], "CANCEL: BACK");
}

///	@brief					进入单灯应用
///
///	@param			ucParam			:	灯号0~8，越界钳到板载灯
///
///	@note					每次进入动作索引复位，首次ENTER执行ON
void vLedSingleAppEnter(uint8_t ucParam)
{
	if(ucParam < LED_DEV_NUM)
	{
		s_ucSingleLedIndex = ucParam;
	}
	else
	{
		s_ucSingleLedIndex = (uint8_t)LED_BOARD;
	}

	s_ucSingleActionIndex = (uint8_t)(LED_APP_ACTION_NUM - 1);
	s_ucSingleActionValid = 0;
	s_vLedSingleBuildFrame();
}

///	@brief					退出单灯应用
///
///	@param					None
///
///	@note					无操作，LED保持进入时的最终状态
void vLedSingleAppExit(void)
{
	/*	无操作	*/
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅ENTER有效：循环执行ON/OFF/TOGGLE/BREATH
void vLedSingleAppHandleKey(emInputKeyTdf emKey)
{
	if(emKey == emInputKey_Enter)
	{
		s_ucSingleActionIndex = (uint8_t)((s_ucSingleActionIndex + 1) % LED_APP_ACTION_NUM);
		s_ucSingleActionValid = 1;
		vLedSetAction((emLedDevNumTdf)s_ucSingleLedIndex, s_aemSingleActions[s_ucSingleActionIndex]);
		s_vLedSingleBuildFrame();
	}
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstLedSingleAppRender(void)
{
	return &s_stSingleFrame;
}

///	@brief					对所有已接入的LED执行一次动作
///
///	@param			emAction		:	目标动作
///
///	@note				当前硬件只接入板载LED（emLedDevNum0），LED1~LED8 设备槽位
///						未初始化（GpioBase为NULL），对其调用vLedSetAction会写空指针
///						触发HardFault，故这里只作用于已初始化的设备
static void s_vLedAllAppApply(emLedActionTdf emAction)
{
	vLedSetAction(emLedDevNum0, emAction);
}

///	@brief					进入全部灯应用
///
///	@param			ucParam			:	动作索引0~3，越界钳到0
///
///	@note					进入即对全部灯执行一次该动作
void vLedAllAppEnter(uint8_t ucParam)
{
	if(ucParam < LED_APP_ACTION_NUM)
	{
		s_ucAllActionIndex = ucParam;
	}
	else
	{
		s_ucAllActionIndex = 0;
	}

	s_vLedAllAppApply(s_aemSingleActions[s_ucAllActionIndex]);

	s_vLedAllBuildFrame();
}

///	@brief					退出全部灯应用
///
///	@param					None
///
///	@note					无操作，LED保持进入时的最终状态
void vLedAllAppExit(void)
{
	/*	无操作	*/
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					仅ENTER有效：对全部灯重复执行当前动作
void vLedAllAppHandleKey(emInputKeyTdf emKey)
{
	if(emKey == emInputKey_Enter)
	{
		s_vLedAllAppApply(s_aemSingleActions[s_ucAllActionIndex]);

		s_vLedAllBuildFrame();
	}
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					列表帧const指针
const stDisplayMsgTdf *c_pstLedAllAppRender(void)
{
	return &s_stAllFrame;
}
