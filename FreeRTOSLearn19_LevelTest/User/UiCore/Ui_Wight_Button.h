 #ifndef	_UI_WIGHT_BUTTON_H_
#define	_UI_WIGHT_BUTTON_H_ 

#include "stdint.h"
#include "OLEDDevice.h"

///	@brief			按钮回调函数
///
///	@note
typedef void vfButtonCallbackTdf(void);

///	@brief			按钮选中状态
///
///	@note
typedef enum
{
	emButtonSelectStatus_IsNotSelected		=	0,
	emButtonSelectStatus_IsSelected				=	1,
}emButtonSelectStatusTdf;

///	@brief			按钮按下状态
///
///	@note
typedef enum
{
	emButtonPressStatus_IsNotPressed			=	0,
	emButtonPressStatus_IsPressed				=	1,
}emButtonPressStatusTdf;

///	@brief			按钮触发状态
///
///	@note
typedef enum
{
	emButtonTriggerStatus_IsNotTriggered	=	0,
	emButtonTriggerStatus_IsTriggered		=	1,
}emButtonTriggerStatusTdf;

///	@brief			[按钮]数据结构
///
///	@note
typedef struct
{
	uint32_t									x;
	uint32_t									y;
	uint8_t									*ucData;
	emOledFontSizeTdf					emFontSize;
	emButtonSelectStatusTdf			emSelectedStatus;
	emButtonPressStatusTdf			emPressStatusCurrent;
	emButtonPressStatusTdf 			emPressStatusLast;
	uint32_t									ulPressedCount;
	uint32_t									ulPressedCountThreshold;
	vfButtonCallbackTdf				*vpfCallback;
}stButtonTdf;

#endif
