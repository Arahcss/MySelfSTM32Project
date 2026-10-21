 #ifndef	_UI_WIGHT_SINGLE_LINE_TEXT_H_
#define	_UI_WIGHT_SINGLE_LINE_TEXT_H_

#include "OLEDDevice.h"
#include "Ui_Wight_Button.h"

///	@brief			[单行文本]数据结构
///
///	@note
typedef struct
{
	uint32_t						x;
	uint32_t						y;
	uint8_t						*ucData;
	emOledFontSizeTdf		emFontSize;
}stSingleLineTestTdf;

#endif

