#include "button_device.h"
#include "string.h"

static stBtnDeviceParamTdf s_astBtnDevice[BTN_DEV_NUM];

static void s_vBtnStaticParamCopy(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
    memcpy(&s_astBtnDevice[emDevNum].stStaticParam, pstInit, sizeof(stBtnStaticParamTdf) / sizeof(uint8_t));
}

const stBtnRunningParamTdf *c_pstGetBtnRunningParam(emBtnDevNumTdf emDevNum)
{
    return &s_astBtnDevice[emDevNum].stRunningParam;
}

void vBtnSetRunningParam(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
    if(pstInit->emCurrentStatus != emBtnStatus_NotModify)
    {
        s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus = pstInit->emCurrentStatus;
    }
    
    if(pstInit->emLastStatus != emBtnStatus_NotModify)
    {
        s_astBtnDevice[emDevNum].stRunningParam.emLastStatus = pstInit->emLastStatus;
    }
    
    if(pstInit->ulDebounceCount != 0xFFFFFFFF)
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount = pstInit->ulDebounceCount;
    }
    
    if(pstInit->ulPressCount != 0xFFFFFFFF)
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulPressCount = pstInit->ulPressCount;
    }
    
    if(pstInit->ulClickCount != 0xFFFFFFFF)
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulClickCount = pstInit->ulClickCount;
    }
    
    if(pstInit->ulDoubleClickCount != 0xFFFFFFFF)
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulDoubleClickCount = pstInit->ulDoubleClickCount;
    }
}

emBtnEventTypeTdf emBtnGetEvent(emBtnDevNumTdf emDevNum)
{
    return s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent;
}

void vBtnClearEvent(emBtnDevNumTdf emDevNum)
{
    s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent = emBtnEvent_None;
}

static emBtnStatusTdf s_emBtnReadStatus(emBtnDevNumTdf emDevNum)
{
    GPIO_PinState emPinLevel;
    emBtnStatusTdf emCurrentLevel;
    
    emPinLevel = HAL_GPIO_ReadPin(s_astBtnDevice[emDevNum].stStaticParam.pstGpioBase,
                                 s_astBtnDevice[emDevNum].stStaticParam.usGpioPin);
    
    emCurrentLevel = (emPinLevel == s_astBtnDevice[emDevNum].stStaticParam.emActiveLevel) ? 
                     emBtnStatus_Pressed : emBtnStatus_Released;
    
    if(emCurrentLevel != s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus)
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount++;
        if(s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount >= 
           s_astBtnDevice[emDevNum].stStaticParam.ulDebounceCountThreshold)
        {
            s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus = emCurrentLevel;
            s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount = 0;
        }
    }
    else
    {
        s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount = 0;
    }
    
    return s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus;
}

static void s_vBtnDetectEvent(emBtnDevNumTdf emDevNum)
{
    emBtnStatusTdf emCurrentStatus = s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus;
    emBtnStatusTdf emLastStatus = s_astBtnDevice[emDevNum].stRunningParam.emLastStatus;
    
    if(emCurrentStatus != emLastStatus)
    {
        if(emCurrentStatus == emBtnStatus_Pressed)
        {
            s_astBtnDevice[emDevNum].stRunningParam.ulPressCount = 0;
        }
       else
        {
           if(s_astBtnDevice[emDevNum].stRunningParam.ulPressCount >= 
               s_astBtnDevice[emDevNum].stStaticParam.ulLongPressCountThreshold)
            {
                s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent = emBtnEvent_LongPress;
                s_astBtnDevice[emDevNum].stRunningParam.ulClickCount = 0;
            }
           else
            {
                s_astBtnDevice[emDevNum].stRunningParam.ulClickCount++;
                s_astBtnDevice[emDevNum].stRunningParam.ulDoubleClickCount = 0;
            }
        }
    }
    else
    {
       if(emCurrentStatus == emBtnStatus_Pressed)
        {
            s_astBtnDevice[emDevNum].stRunningParam.ulPressCount++;
        }
        else
        {
            if(s_astBtnDevice[emDevNum].stRunningParam.ulClickCount == 1)
            {
                s_astBtnDevice[emDevNum].stRunningParam.ulDoubleClickCount++;
                if(s_astBtnDevice[emDevNum].stRunningParam.ulDoubleClickCount >= 
                   s_astBtnDevice[emDevNum].stStaticParam.ulDoubleClickInterval)
                {
                    s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent = emBtnEvent_Click;
                    s_astBtnDevice[emDevNum].stRunningParam.ulClickCount = 0;
                }
            }
            else if(s_astBtnDevice[emDevNum].stRunningParam.ulClickCount == 2)
            {
                s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent = emBtnEvent_DoubleClick;
                s_astBtnDevice[emDevNum].stRunningParam.ulClickCount = 0;
            }
        }
    }
    
    s_astBtnDevice[emDevNum].stRunningParam.emLastStatus = emCurrentStatus;
}

void vBtnExecute(emBtnDevNumTdf emDevNum)
{
    s_emBtnReadStatus(emDevNum);
    
    s_vBtnDetectEvent(emDevNum);
}

emBtnFuncErrorCodeTdf emBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
    s_vBtnStaticParamCopy(pstInit, emDevNum);
    
    s_astBtnDevice[emDevNum].stRunningParam.emCurrentStatus = emBtnStatus_Released;
    s_astBtnDevice[emDevNum].stRunningParam.emLastStatus = emBtnStatus_Released;
    s_astBtnDevice[emDevNum].stRunningParam.emCurrentEvent = emBtnEvent_None;
    s_astBtnDevice[emDevNum].stRunningParam.ulDebounceCount = 0;
    s_astBtnDevice[emDevNum].stRunningParam.ulPressCount = 0;
    s_astBtnDevice[emDevNum].stRunningParam.ulClickCount = 0;
    s_astBtnDevice[emDevNum].stRunningParam.ulDoubleClickCount = 0;
    
	return emBtnFuncErrorCode_None;
}
