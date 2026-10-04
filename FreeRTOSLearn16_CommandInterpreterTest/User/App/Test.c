#include "LedDevice.h"
#include "string.h"
#include "Test.h"
#include "DHT11Device.h"
#include "RingBufferDevice.h"
#include "InterpreterDevice.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

uint8_t aucUartRxBuffer[20];					//UART接收缓存区
uint8_t ucUartRxComplete = FALSE;			//UART接收完成标准

const uint8_t c_aucCmdDht11Read[]	= "DHT11Read\r\n";
const uint8_t c_aucCmdLedOn0[]			="LED_ON_0\r\n";
const uint8_t c_aucCmdLedOff0[]			="LED_OFF_0\r\n";

uint8_t aucAckDht11[] = "Temperature : 00.0			Humidity: 00.0\r\n";

char acListString_Led[]				= "LED";
char acListString_On[]				=	"ON";
char acListString_Off[]				=	"OFF";
char acListString_Toggle[]			=	"TOGGLE";
char acListString_Blink[]			=	"BLINK";
char acListString_Param[]			=	"0_8____1_10000____2_20000";

void vCmdInterpreterExecuteCallback_LedOn(char **p2ucString);
void vCmdInterpreterExecuteCallback_LedOff(char **p2ucString);
void vCmdInterpreterExecuteCallback_LedToggle(char **p2ucString);
void vCmdInterpreterExecuteCallback_LedBlink_Param2(char **p2ucString);
void vCmdInterpreterExecuteCallback_LedBlink_Param3(char **p2ucString);

stInterpreterListTdf astList [] =
{
	{//LED ON
	.c_pcObject 			= acListString_Led , 
	.c_pcCmd 				= acListString_On , 
	.c_pcParam 			= acListString_Param,
	.c_ucParamNum	 	= 1 ,
	.pvfCallBack 			= vCmdInterpreterExecuteCallback_LedOn
	},

	{//LED OFF
	.c_pcObject 			= acListString_Led ,
	.c_pcCmd 				= acListString_Off ,
	.c_pcParam 			= acListString_Param,
	.c_ucParamNum 	= 1,
	.pvfCallBack 			= vCmdInterpreterExecuteCallback_LedOff
	},
	
	{//LED TOGGLE
	.c_pcObject 			= acListString_Led ,
	.c_pcCmd 				= acListString_Toggle ,
	.c_pcParam 			= acListString_Param,
	.c_ucParamNum 	= 1,
	.pvfCallBack 			= vCmdInterpreterExecuteCallback_LedToggle
	},
	
	{//LED BLINK PARAM 2
	.c_pcObject 			= acListString_Led ,
	.c_pcCmd 				= acListString_Blink ,
	.c_pcParam 			= acListString_Param,
	.c_ucParamNum 	= 2,
	.pvfCallBack 			= vCmdInterpreterExecuteCallback_LedBlink_Param2
	},
	
	{//LED BLINK PARAM 3
	.c_pcObject 			= acListString_Led ,
	.c_pcCmd 				= acListString_Blink ,
	.c_pcParam 			= acListString_Param,
	.c_ucParamNum 	= 3,
	.pvfCallBack 			= vCmdInterpreterExecuteCallback_LedBlink_Param3
	},
};

///	@brief			LED初始化
///
///	@note
void vLedInit(void)
{
	stLedStaticParamTdf 		stStaticInit;
	stLedRunningParamTdf	stRunningInit;
	
	stRunningInit.emMode						= emLedMode_Breath;
	stRunningInit.ulCurrentCount			= 0;
	stRunningInit.ulOnCountThreshold	= 100;
	stRunningInit.ulOffCountThreshold	=	100;
	stRunningInit.ulBreathPeriod				= 10000;
	vLedDeviceRunningParamInit(&stRunningInit,LED_BOARD);
	
	stStaticInit.pstGpioBase	= GPIOC;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_13;
	vLedDeviceInit(&stStaticInit,LED_BOARD);
	
	stStaticInit.pstGpioBase	= GPIOB;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_1;
	vLedDeviceInit(&stStaticInit,LED1);
	
	stStaticInit.pstGpioBase	= GPIOB;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_0;
	vLedDeviceInit(&stStaticInit,LED2);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_2;
	vLedDeviceInit(&stStaticInit,LED3);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_3;
	vLedDeviceInit(&stStaticInit,LED4);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_4;
	vLedDeviceInit(&stStaticInit,LED5);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_5;
	vLedDeviceInit(&stStaticInit,LED6);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_6;
	vLedDeviceInit(&stStaticInit,LED7);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_7;
	vLedDeviceInit(&stStaticInit,LED8);
	
	
}

///	@brief			DHT11初始化
///
///	@note
void vDht11Init(void)
{
	stDht11StaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOB;
	stInit.usGpioPin			=	GPIO_PIN_12;
	stInit.ulTimerPeriorUs = 5;
	vDht11DeviceInit(&stInit,DHT11);
}

///	@brief			环形缓冲区初始化
///
///	@note
void vRingBufferInit(void)
{
	stRingBufferStaticParamTdf stInit;
	
	stInit.pvHead 					= aucUartRxBuffer;
	stInit.pvTail						=	(uint8_t *)aucUartRxBuffer + sizeof(aucUartRxBuffer) / sizeof(uint8_t) -1;
	stInit.ulElementLength	= sizeof(aucUartRxBuffer[0]);

	vRingBufferDeviceInit(&stInit,UART_RX_BUFFER); 	
}

///	@brief			命令解释器初始化
///
///	@note
void vCmdInterpreterInit(void)
{
	stInterpreterStaticParamTdf stInit;
	
	stInit.pstList						=	astList;
	stInit.ulListSize				=	sizeof(astList)	/	sizeof(stInterpreterListTdf);
	
	vInterpreterDeviceInit(&stInit,emInterpreterDevNum0);
}

///	@brief				字符串转换为uint32_t型
///
///	@note		
uint32_t ulStringToUint32(char *str)
{
	uint32_t ulResult = 0;
	
	while((*str >= '0') && (*str <= '9'))
	{
		ulResult *= 10;
		ulResult += *str - '0';
		str++;
	}
	return  ulResult;
}

///	@brief				重新定向fputc
///
///	@note		
int fputc(int ch,FILE *f)
{
	while((huart1.Instance->SR & USART_SR_TXE) == 0)
	{
		;
	}
	
	huart1.Instance->DR = *(uint8_t *)&ch;
	
	return ch;
}

///	@brief				命令解释器执行回调[LED_ON]
///
///	@note				
void vCmdInterpreterExecuteCallback_LedOn(char **p2ucString)
{
	char acString[] = "LED_ON_0\r\n";
	
	vLedOn((emLedDevNumTdf)ulStringToUint32(p2ucString[2]));

	acString[7] = p2ucString[2][0];
	
	HAL_UART_Transmit(&huart1,(uint8_t *)acString,sizeof(acString),10);
}

///	@brief				命令解释器执行回调[LED_OFF]
///
///	@note				
void vCmdInterpreterExecuteCallback_LedOff(char **p2ucString)
{
	char acString[] = "LED_OFF_0\r\n";
	
	vLedOFF((emLedDevNumTdf)ulStringToUint32(p2ucString[2]));

	acString[8] = p2ucString[2][0];
	
	HAL_UART_Transmit(&huart1,(uint8_t *)acString,sizeof(acString),10);
}

///	@brief				命令解释器执行回调[LED_TOGGLE]
///
///	@note				
void vCmdInterpreterExecuteCallback_LedToggle(char **p2ucString)
{
	char acString[] = "LED_TOGGLE_0\r\n";

	vLedToggle((emLedDevNumTdf)ulStringToUint32(p2ucString[2]));
	
	acString[11] = p2ucString[2][0];
	
	HAL_UART_Transmit(&huart1,(uint8_t *)acString,sizeof(acString),10);

}

///	@brief				命令解释器执行回调[LED_BLINK_PARAM2]
///
///	@note				
void vCmdInterpreterExecuteCallback_LedBlink_Param2(char **p2cString)
{
	char acString[] = "LED_BLINK";
	
	stLedRunningParamTdf	stRunningInit;
	
	stRunningInit.emMode						= emLedMode_Blink;
	stRunningInit.ulCurrentCount			= 0;
	stRunningInit.ulOnCountThreshold	= ulStringToUint32(p2cString[3]);
	stRunningInit.ulOffCountThreshold	=	ulStringToUint32(p2cString[3]);
	stRunningInit.ulBreathPeriod				= 50000;
	vLedDeviceRunningParamInit(&stRunningInit,(emLedDevNumTdf)ulStringToUint32(p2cString[2]));
	

//	vLedDeviceBlinkExecute();
	
	
	printf(acString);
	printf(" ");
	printf(p2cString[2]);
	printf(" ");
	printf(p2cString[3]);
	//printf("\r\n");
}

///	@brief				命令解释器执行回调[LED_BLINK_PARAM3]
///
///	@note				
void vCmdInterpreterExecuteCallback_LedBlink_Param3(char **p2cString)
{
	char acString[] = "LED_BLINK";
	
	stLedRunningParamTdf	stRunningInit;
	
	stRunningInit.emMode						= emLedMode_Blink;
	stRunningInit.ulCurrentCount			= 0;
	stRunningInit.ulOnCountThreshold	= ulStringToUint32(p2cString[3]);
	stRunningInit.ulOffCountThreshold	=	ulStringToUint32(p2cString[4]);
	stRunningInit.ulBreathPeriod				= 50000;
	vLedDeviceRunningParamInit(&stRunningInit,(emLedDevNumTdf)ulStringToUint32(p2cString[2]));
	
	vLedToggle((emLedDevNumTdf)ulStringToUint32(p2cString[2]));
	
	
}

///	@brief				测试执行
///
///	@note				实现测试功能
void vTaskExecute(void)
{
	void *pvTemp;
	const uint8_t pucEndChar = '\n';
	uint8_t aucCmdBuffer[30];
	uint8_t i;
	
	//轮询是否收到了'\n'
	pvTemp = pvRingBufferFindElementFirstPosition((void *)&pucEndChar , UART_RX_BUFFER);
	
	//接收到了则读出接收到的数据
	if(pvTemp != NULL)
	{
		i=0;
		
		while(c_pstGetRingBufferDeviceParam(UART_RX_BUFFER)->stRunningParam.pvRead != pvTemp)
		{
			emRingBufferReadSingleElement(&aucCmdBuffer[i],UART_RX_BUFFER);
			i++;
		}
		
		emRingBufferReadSingleElement(&aucCmdBuffer[i],UART_RX_BUFFER);
		i++;
		aucCmdBuffer[i] = '\0';
	
//		//解析控制DHT11
		vInterpreterDeviceExecute((char*) aucCmdBuffer,emInterpreterDevNum0);
	}
	
	for(i=0;i<8;i++)
	{
		vLedDevicePeriodExecute((emLedDevNumTdf)i);
	}
	
	HAL_Delay(0);
}

/////	@brief				测试执行
/////
/////	@note				实现测试功能
//void vTaskExecute(void)
//{
//	void *pvTemp;
//	const uint8_t pucEndChar = '\n';
//	uint8_t aucCmdBuffer[30];
//	uint8_t i;
//	
//	//接收首位地址
//	pvTemp = pvRingBufferFindElementFirstPosition((void *)&pucEndChar , UART_RX_BUFFER);
//	
//	if(pvTemp != NULL)
//	{
//		i=0;
//		
//		while(c_pstGetRingBufferDeviceParam(UART_RX_BUFFER)->stRunningParam.pvRead != pvTemp)
//		{
//			emRingBufferReadSingleElement(&aucCmdBuffer[i],UART_RX_BUFFER);
//			i++;
//		}
//		emRingBufferReadSingleElement(&aucCmdBuffer[i],UART_RX_BUFFER);
//		i++;
//		aucCmdBuffer[i] = '\0';
//	
//		//DHT11控制
//		if(memcmp(c_aucCmdDht11Read,aucCmdBuffer,sizeof(c_aucCmdDht11Read) / sizeof(uint8_t) ) == 0)
//		{
//			//发送
//			aucAckDht11[14] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] / 10 + '0';
//			aucAckDht11[15] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] % 10 + '0';
//			aucAckDht11[17] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[1] % 10 + '0';
//			
//			aucAckDht11[31] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] / 10+ '0';
//			aucAckDht11[32] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] % 10+ '0';			
//			HAL_UART_Transmit(&huart1,aucAckDht11,sizeof(aucAckDht11) , 10);
//		}
//		
//		if(memcmp(c_aucCmdLedOn0,aucCmdBuffer,sizeof(c_aucCmdLedOn0) / sizeof(uint8_t) ) == 0)
//		{
//			vLedOn(LED_BOARD);
//			//发送
//			HAL_UART_Transmit(&huart1,c_aucCmdLedOn0,sizeof(c_aucCmdLedOn0) , 10);
//		}
//		
//		if(memcmp(c_aucCmdLedOff0,aucCmdBuffer,sizeof(c_aucCmdLedOff0) / sizeof(uint8_t) ) == 0)
//		{
//			vLedOFF(LED_BOARD);
//			//发送
//			HAL_UART_Transmit(&huart1,c_aucCmdLedOff0,sizeof(c_aucCmdLedOff0) , 10);
//		}
//	}
//	HAL_Delay(5);
//}


/////	@brief				测试执行
/////
/////	@note				实现测试功能
//void vTaskExecute(void)
//{

////	uint8_t i;
////	for(i=1;i<9;i++)
////	{
////		vLedOn(i);
////		HAL_Delay(100);
////		
////		vLedOFF(i);
////		HAL_Delay(100);
////	}
//	
////	for(i = 1 ;i< 9 ;i++)
////	{
////		vLedToggle((emLedDevNumTdf)i);
////		HAL_Delay(100);
////	}
//	
////	vLedDevicePeriodExecute(LED_BOARD);
////	HAL_Delay(0);
//	
////	ucDht11ReadData(DHT11);
////	HAL_Delay(2000);

//	if(ucUartRxComplete ==  TRUE)
//	{
//		ucUartRxComplete = FALSE;
//		
//		//DHT11控制
//		if(memcmp(c_aucCmdDht11Read,aucUartRxBuffer,sizeof(c_aucCmdDht11Read) / sizeof(uint8_t) ) == 0)
//		{
//			//发送
//			aucAckDht11[14] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] / 10 + '0';
//			aucAckDht11[15] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] % 10 + '0';
//			aucAckDht11[17] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[1] % 10 + '0';
//			
//			aucAckDht11[31] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] / 10+ '0';
//			aucAckDht11[32] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] % 10+ '0';			
//			HAL_UART_Transmit(&huart1,aucAckDht11,sizeof(aucAckDht11) , 10);
//		}
//		if(memcmp(c_aucCmdLedOn0,aucUartRxBuffer,sizeof(c_aucCmdLedOn0) / sizeof(uint8_t) ) == 0)
//		{
//			vLedOn(LED_BOARD);
//			//发送
//			HAL_UART_Transmit(&huart1,c_aucCmdLedOn0,sizeof(c_aucCmdLedOn0) , 10);
//		}
//		if(memcmp(c_aucCmdLedOff0,aucUartRxBuffer,sizeof(c_aucCmdLedOff0) / sizeof(uint8_t) ) == 0)
//		{
//			vLedOFF(LED_BOARD);
//			//发送
//			HAL_UART_Transmit(&huart1,c_aucCmdLedOff0,sizeof(c_aucCmdLedOff0) , 10);
//		}
//	}
//}



