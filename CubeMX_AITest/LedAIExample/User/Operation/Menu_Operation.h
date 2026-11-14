#ifndef _MENU_OPERATION_H_
#define _MENU_OPERATION_H_

#include "Input_Operation.h"
#include "App_Operation.h"

///	@brief					菜单节点类型枚举
///
///	@note
typedef enum
{
	emMenuNodeType_SubMenu		=	0,		//	子菜单
	emMenuNodeType_App,						//	应用
}emMenuNodeTypeTdf;

///	@brief					菜单项
///
///	@note
typedef struct
{
	const char *c_pcLabel;					//	菜单项文本
	emMenuNodeTypeTdf emType;				//	节点类型
	uint8_t ucTarget;						//	SubMenu：子菜单索引；App：应用索引
	uint8_t ucParam;						//	启动App时传给vEnter
}stMenuItemTdf;

///	@brief					菜单
///
///	@note
typedef struct
{
	const char *c_pcTitle;					//	菜单标题
	uint8_t ucItemCount;					//	菜单项数量
	const stMenuItemTdf *c_pstItems;		//	菜单项表
}stMenuTdf;

///	@brief					菜单当前模式枚举
///
///	@note
typedef enum
{
	emMenuMode_Menu				=	0,		//	菜单浏览
	emMenuMode_App,							//	应用运行
}emMenuModeTdf;

///	@brief					菜单初始化
///
///	@param					None
///
///	@note					回到主菜单首项并重建显示帧
void vMenuInit(void);

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					菜单模式下做上下选择/进入/返回；应用模式下Cancel退出，其余交给应用
void vMenuHandleKey(emInputKeyTdf emKey);

///	@brief					周期执行
///
///	@param					None
///
///	@note					仅应用模式下调用应用vTick（非NULL时）并重建显示帧
void vMenuExecuteTick(void);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					显示帧const指针，可直接投递到显示队列
const stDisplayMsgTdf *c_pstMenuGetRender(void);

///	@brief					获取当前运行的应用接口
///
///	@param					None
///
///	@return					应用模式返回应用接口指针，菜单模式返回NULL
///
///	@note
const stAppInterfaceTdf *c_pstGetActiveApp(void);

#endif
