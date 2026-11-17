#include <stddef.h>
#include "Menu_Operation.h"
#include "Project_Config.h"

#define	MENU_INVALID_APP		0xFF

///	@brief					主菜单项表
///
///	@note
static const stMenuItemTdf s_astMainMenuItems[] =
{
	{"CALCULATOR",	emMenuNodeType_App,			APP_CALCULATOR,	0},
	{"LED CONTROL",	emMenuNodeType_SubMenu,		1,				0},
	{"GAME",		emMenuNodeType_SubMenu,		2,				0},
};

///	@brief					LED控制子菜单项表
///
///	@note
static const stMenuItemTdf s_astLedControlMenuItems[] =
{
	{"SINGLE LED",	emMenuNodeType_SubMenu,		3,				0},
	{"ALL LED",		emMenuNodeType_SubMenu,		4,				0},
};

///	@brief					游戏子菜单项表
///
///	@note
static const stMenuItemTdf s_astGameMenuItems[] =
{
	{"SNAKE",		emMenuNodeType_App,			APP_GAME_SNAKE,	0},
};

///	@brief					单灯子菜单项表
///
///	@note					ucParam即灯号；当前硬件仅接入板载LED（emLedDevNum0），
///							故只保留LED0，其余设备槽位未初始化不可用
static const stMenuItemTdf s_astSingleLedMenuItems[] =
{
	{"LED0",		emMenuNodeType_App,			APP_LED_SINGLE,	0},
};

///	@brief					全部灯子菜单项表
///
///	@note					ucParam即动作索引0~3
static const stMenuItemTdf s_astAllLedMenuItems[] =
{
	{"ALL ON",		emMenuNodeType_App,			APP_LED_ALL,	0},
	{"ALL OFF",		emMenuNodeType_App,			APP_LED_ALL,	1},
	{"ALL TOGGLE",	emMenuNodeType_App,			APP_LED_ALL,	2},
	{"ALL BREATH",	emMenuNodeType_App,			APP_LED_ALL,	3},
};

///	@brief					菜单树
///
///	@note					下标即菜单索引，子菜单项ucTarget指向本表下标
static const stMenuTdf s_astMenuTable[5] =
{
	{"MAIN",			3,	s_astMainMenuItems},
	{"LED CONTROL",		2,	s_astLedControlMenuItems},
	{"GAME",			1,	s_astGameMenuItems},
	{"SINGLE LED",		1,	s_astSingleLedMenuItems},
	{"ALL LED",			4,	s_astAllLedMenuItems},
};

static uint8_t				s_aucMenuStack[MENU_MAX_DEPTH];		//	各级菜单索引
static uint8_t				s_aucSelectStack[MENU_MAX_DEPTH];	//	各级选中项
static uint8_t				s_ucDepth;							//	当前深度，0起
static emMenuModeTdf		s_emMode;							//	当前模式
static uint8_t				s_ucActiveApp;						//	当前运行的应用索引
static stDisplayMsgTdf		s_stDisplayFrame;					//	当前显示帧

///	@brief					写入文本并清零剩余字节
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcText		:	源字符串
///
///	@note					最多拷贝DISPLAY_LINE_LENGTH-1个字符，其余字节清0
static void s_vMenuSetText(uint8_t *pucDst, const char *c_pcText)
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

///	@brief					计算文本截断后的有效字符数
///
///	@param			c_pcText		:	源字符串
///
///	@return					有效字符数，最大DISPLAY_LINE_LENGTH-1
///
///	@note
static uint8_t s_ucMenuTextLength(const char *c_pcText)
{
	uint8_t ucIndex = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucIndex] != '\0'))
	{
		ucIndex++;
	}

	return ucIndex;
}

///	@brief					重建当前显示帧
///
///	@param					None
///
///	@note					应用模式拷贝应用帧；菜单模式按栈内菜单与选中项组织列表帧
static void s_vMenuBuildFrame(void)
{
	const stAppInterfaceTdf *c_pstApp = NULL;
	const stDisplayMsgTdf *c_pstAppFrame = NULL;
	const stMenuTdf *c_pstMenu = NULL;
	uint8_t ucSel = 0;
	uint8_t ucStart = 0;
	uint8_t ucLineCount = 0;
	uint8_t ucLine = 0;

	if(s_emMode == emMenuMode_App)
	{
		c_pstApp = c_pstGetAppInterface(s_ucActiveApp);
		if((c_pstApp != NULL) && (c_pstApp->c_pstRender != NULL))
		{
			c_pstAppFrame = c_pstApp->c_pstRender();
			if(c_pstAppFrame != NULL)
			{
				s_stDisplayFrame = *c_pstAppFrame;
			}
		}

		return;
	}

	c_pstMenu = &s_astMenuTable[s_aucMenuStack[s_ucDepth]];
	ucSel = s_aucSelectStack[s_ucDepth];

	s_stDisplayFrame.emType = emDisplayFrameType_List;
	s_stDisplayFrame.unFrame.stList.ucTitleLength = s_ucMenuTextLength(c_pstMenu->c_pcTitle);
	s_vMenuSetText(s_stDisplayFrame.unFrame.stList.aucTitle, c_pstMenu->c_pcTitle);

	ucStart = 0;
	if(ucSel >= MENU_VISIBLE_ITEM_NUM)
	{
		ucStart = (uint8_t)(ucSel - (MENU_VISIBLE_ITEM_NUM - 1));
	}

	ucLineCount = (uint8_t)(c_pstMenu->ucItemCount - ucStart);
	if(ucLineCount > MENU_VISIBLE_ITEM_NUM)
	{
		ucLineCount = MENU_VISIBLE_ITEM_NUM;
	}

	for(ucLine = 0; ucLine < DISPLAY_LIST_LINE_NUM; ucLine++)
	{
		if(ucLine < ucLineCount)
		{
			s_vMenuSetText(s_stDisplayFrame.unFrame.stList.aucLine[ucLine],
					c_pstMenu->c_pstItems[ucStart + ucLine].c_pcLabel);
		}
		else
		{
			s_vMenuSetText(s_stDisplayFrame.unFrame.stList.aucLine[ucLine], "");
		}
	}

	s_stDisplayFrame.unFrame.stList.ucLineCount = ucLineCount;
	s_stDisplayFrame.unFrame.stList.ucSelectedIndex = (uint8_t)(ucSel - ucStart);
	s_stDisplayFrame.unFrame.stList.aucReserved = 0;
}

///	@brief					清空显示帧
///
///	@param					None
///
///	@note					逐字节清零，避免上一次模式的残留内容
static void s_vMenuClearFrame(void)
{
	uint8_t *pucFrame = (uint8_t *)&s_stDisplayFrame;
	uint16_t usIndex = 0;

	for(usIndex = 0; usIndex < (uint16_t)sizeof(stDisplayMsgTdf); usIndex++)
	{
		pucFrame[usIndex] = 0;
	}
}

///	@brief					菜单初始化
///
///	@param					None
///
///	@note					回到主菜单首项并重建显示帧
void vMenuInit(void)
{
	uint8_t ucIndex = 0;

	s_emMode = emMenuMode_Menu;
	s_ucDepth = 0;
	s_ucActiveApp = MENU_INVALID_APP;
	s_aucMenuStack[0] = 0;

	for(ucIndex = 0; ucIndex < MENU_MAX_DEPTH; ucIndex++)
	{
		s_aucSelectStack[ucIndex] = 0;
	}

	s_vMenuClearFrame();
	s_vMenuBuildFrame();
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					菜单模式下做上下选择/进入/返回；应用模式下Cancel退出，其余交给应用
void vMenuHandleKey(emInputKeyTdf emKey)
{
	const stAppInterfaceTdf *c_pstApp = NULL;
	const stMenuTdf *c_pstMenu = NULL;
	const stMenuItemTdf *c_pstItem = NULL;
	uint8_t ucSel = 0;

	if(s_emMode == emMenuMode_App)
	{
		c_pstApp = c_pstGetAppInterface(s_ucActiveApp);

		if(emKey == emInputKey_Cancel)
		{
			if((c_pstApp != NULL) && (c_pstApp->vExit != NULL))
			{
				c_pstApp->vExit();
			}

			s_emMode = emMenuMode_Menu;
			s_ucActiveApp = MENU_INVALID_APP;
		}
		else
		{
			if((c_pstApp != NULL) && (c_pstApp->vHandleKey != NULL))
			{
				c_pstApp->vHandleKey(emKey);
			}
		}

		s_vMenuBuildFrame();
		return;
	}

	c_pstMenu = &s_astMenuTable[s_aucMenuStack[s_ucDepth]];
	ucSel = s_aucSelectStack[s_ucDepth];

	switch(emKey)
	{
		case emInputKey_Up:
		{
			if(ucSel == 0)
			{
				ucSel = (uint8_t)(c_pstMenu->ucItemCount - 1);
			}
			else
			{
				ucSel--;
			}

			s_aucSelectStack[s_ucDepth] = ucSel;
			break;
		}
		case emInputKey_Down:
		{
			ucSel++;
			if(ucSel >= c_pstMenu->ucItemCount)
			{
				ucSel = 0;
			}

			s_aucSelectStack[s_ucDepth] = ucSel;
			break;
		}
		case emInputKey_Enter:
		{
			c_pstItem = &c_pstMenu->c_pstItems[ucSel];
			if(c_pstItem->emType == emMenuNodeType_SubMenu)
			{
				if(s_ucDepth < (MENU_MAX_DEPTH - 1))
				{
					s_ucDepth++;
					s_aucMenuStack[s_ucDepth] = c_pstItem->ucTarget;
					s_aucSelectStack[s_ucDepth] = 0;
				}
			}
			else
			{
				c_pstApp = c_pstGetAppInterface(c_pstItem->ucTarget);
				if(c_pstApp != NULL)
				{
					s_emMode = emMenuMode_App;
					s_ucActiveApp = c_pstItem->ucTarget;

					if(c_pstApp->vEnter != NULL)
					{
						c_pstApp->vEnter(c_pstItem->ucParam);
					}
				}
			}
			break;
		}
		case emInputKey_Cancel:
		{
			if(s_ucDepth > 0)
			{
				s_ucDepth--;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	s_vMenuBuildFrame();
}

///	@brief					周期执行
///
///	@param					None
///
///	@return					非0表示显示帧已变化需要刷新，0表示无需刷新
///
///	@note					仅应用模式下调用应用ucTick（非NULL时），
///							返回非0时重建显示帧
uint8_t ucMenuExecuteTick(void)
{
	const stAppInterfaceTdf *c_pstApp = NULL;
	uint8_t ucFrameChanged = 0;

	if(s_emMode != emMenuMode_App)
	{
		return 0;
	}

	c_pstApp = c_pstGetAppInterface(s_ucActiveApp);
	if((c_pstApp == NULL) || (c_pstApp->ucTick == NULL))
	{
		return 0;
	}

	ucFrameChanged = c_pstApp->ucTick();
	if(ucFrameChanged != 0)
	{
		s_vMenuBuildFrame();
	}

	return ucFrameChanged;
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					显示帧const指针，可直接投递到显示队列
const stDisplayMsgTdf *c_pstMenuGetRender(void)
{
	return &s_stDisplayFrame;
}

///	@brief					获取当前运行的应用接口
///
///	@param					None
///
///	@return					应用模式返回应用接口指针，菜单模式返回NULL
///
///	@note
const stAppInterfaceTdf *c_pstGetActiveApp(void)
{
	if(s_emMode != emMenuMode_App)
	{
		return NULL;
	}

	return c_pstGetAppInterface(s_ucActiveApp);
}
