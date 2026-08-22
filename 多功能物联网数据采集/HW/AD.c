#include "stm32f10x.h"                  // Device header
void AD1_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//开启ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2 |GPIO_Pin_3;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1ADC2独立转换模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不使用外部触发，用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//启用连续扫描
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//不启用扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;//通道数
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);//开启复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));//重复获取已完成的复位信号
	ADC_StartCalibration(ADC1);//启动校准
	while(ADC_GetCalibrationStatus(ADC1));//

}

uint16_t AD1_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_55Cycles5);//在通道1连频道0
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//自动触发模式只用开头触发一次所以可以直接放到初始化里

	while (!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//因为需要判断是否转换完成所以用EOC
	return 	ADC_GetConversionValue(ADC1);
	
	
}




