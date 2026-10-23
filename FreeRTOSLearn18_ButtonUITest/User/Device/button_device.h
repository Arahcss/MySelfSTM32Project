#ifndef _BUTTON_DEVICE_H_
#define _BUTTON_DEVICE_H_

#include "Project_Config.h"
#include STM32_IC_HAL

typedef enum
{
    emBtnDevNum0                                       = 0,                           
    emBtnDevNum1,
    emBtnDevNum2,
    emBtnDevNum3,
    emBtnDevNum4,
    emBtnDevNum5,
    emBtnDevNum6,
    emBtnDevNum7,
    emBtnDevNum8,
    emBtnDevNum9,
    emBtnDevNum10,
} emBtnDevNumTdf;

typedef enum
{
    emBtnActiveLevel_Low                               = 0,                      
    emBtnActiveLevel_High                              = 1,                           
} emBtnActiveLevelTdf;

typedef enum
{
    emBtnStatus_Released                               = 0,                       
    emBtnStatus_Pressed                                = 1,                     
    emBtnStatus_NotModify                              = 0xFFFF,               
} emBtnStatusTdf;

typedef enum
{
    emBtnEvent_None                                    = 0,                       
    emBtnEvent_Click                                   = 1,                          
    emBtnEvent_LongPress                               = 2,                         
    emBtnEvent_DoubleClick                             = 3,                
} emBtnEventTypeTdf;

typedef enum
{
    emBtnFuncErrorCode_None                            = 0,                         
} emBtnFuncErrorCodeTdf;

typedef struct
{
    GPIO_TypeDef                                       *pstGpioBase;         
    uint16_t                                           usGpioPin;                  
    emBtnActiveLevelTdf                                emActiveLevel;               
    uint32_t                                           ulDebounceCountThreshold;   
    uint32_t                                           ulLongPressCountThreshold;    
    uint32_t                                           ulDoubleClickInterval;     
} stBtnStaticParamTdf;

typedef struct
{
    emBtnStatusTdf                                     emCurrentStatus;          
    emBtnStatusTdf                                     emLastStatus;                
    emBtnEventTypeTdf                                  emCurrentEvent;             
    uint32_t                                           ulDebounceCount;               
    uint32_t                                           ulPressCount;                
    uint32_t                                           ulClickCount;                
    uint32_t                                           ulDoubleClickCount;           
} stBtnRunningParamTdf;

typedef struct
{
    stBtnStaticParamTdf                                stStaticParam;                 // 静态参数
    stBtnRunningParamTdf                               stRunningParam;                // 运行参数
} stBtnDeviceParamTdf;

const stBtnRunningParamTdf *c_pstGetBtnRunningParam(emBtnDevNumTdf emDevNum);
emBtnEventTypeTdf emBtnGetEvent(emBtnDevNumTdf emDevNum);
void vBtnClearEvent(emBtnDevNumTdf emDevNum);
void vBtnSetRunningParam(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum);
void vBtnExecute(emBtnDevNumTdf emDevNum);
emBtnFuncErrorCodeTdf emBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum);

#endif
