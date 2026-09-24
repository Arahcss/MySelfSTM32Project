#include "LedDevice.h"
#include "string.h"
#include "Test.h"
#include "DHT11Device.h"

extern UART_HandleTypeDef huart1;

uint8_t aucUartRxBuffer[20];					//UART接收缓存区
uint8_t ucUartRxComplete = FALSE;			//UART接收完成标准

const uint8_t c_aucCmdDht11Read[]	= "DHT11Read\r\n";
const uint8_t c_aucCmdLedOn0[]			="LED_ON0\r\n";
const uint8_t c_aucCmdLedOff0[]			="LED_OFF0\r\n";

uint8_t aucAckDht11[] = "Temperature : 00.0			Humidity: 00.0\r\n";

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
	
		vLedOFF(0);//板载LED好闪眼睛。。。
}

void vDht11Init(void)
{
	stDht11StaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOB;
	stInit.usGpioPin			=	GPIO_PIN_12;
	stInit.ulTimerPeriorUs = 5;
	vDht11DeviceInit(&stInit,DHT11);
}

///	@brief				测试执行
///
///	@note				实现测试功能
void vTaskExecute(void)
{

//	uint8_t i;
//	for(i=1;i<9;i++)
//	{
//		vLedOn(i);
//		HAL_Delay(100);
//		
//		vLedOFF(i);
//		HAL_Delay(100);
//	}
	
//	for(i = 1 ;i< 9 ;i++)
//	{
//		vLedToggle((emLedDevNumTdf)i);
//		HAL_Delay(100);
//	}
	
//	vLedDevicePeriodExecute(LED_BOARD);
//	HAL_Delay(0);
	
//	ucDht11ReadData(DHT11);
//	HAL_Delay(2000);

	if(ucUartRxComplete ==  TRUE)
	{
		ucUartRxComplete = FALSE;
		
		//DHT11控制
		if(memcmp(c_aucCmdDht11Read,aucUartRxBuffer,sizeof(c_aucCmdDht11Read) / sizeof(uint8_t) ) == 0)
		{
			//发送
			aucAckDht11[14] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] / 10 + '0';
			aucAckDht11[15] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[0] % 10 + '0';
			aucAckDht11[17] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acTemperature[1] % 10 + '0';
			
			aucAckDht11[31] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] / 10+ '0';
			aucAckDht11[32] = c_pstGetDht11DeviceParam(DHT11)->stRunningParam.acHumidity[0] % 10+ '0';			
			HAL_UART_Transmit(&huart1,aucAckDht11,sizeof(aucAckDht11) , 10);
		}
		if(memcmp(c_aucCmdLedOn0,aucUartRxBuffer,sizeof(c_aucCmdLedOn0) / sizeof(uint8_t) ) == 0)
		{
			vLedOn(LED_BOARD);
			//发送
			HAL_UART_Transmit(&huart1,c_aucCmdLedOn0,sizeof(c_aucCmdLedOn0) , 10);
		}
		if(memcmp(c_aucCmdLedOff0,aucUartRxBuffer,sizeof(c_aucCmdLedOff0) / sizeof(uint8_t) ) == 0)
		{
			vLedOFF(LED_BOARD);
			//发送
			HAL_UART_Transmit(&huart1,c_aucCmdLedOff0,sizeof(c_aucCmdLedOff0) , 10);
		}
	}
}

