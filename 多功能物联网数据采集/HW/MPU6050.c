#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx,uint32_t I2C_EVENT)
{
		uint32_t TimOut=10000;
		while( I2C_CheckEvent(I2Cx,I2C_EVENT) != SUCCESS)//因为硬件配置不自带延时所以需要接收EVE5成功条件
		if(TimOut--==0)break;
}
	

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	
	
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);//指定设备
//	MyI2C_ReceiveAck();//应答位，判断是否收到数据
//	MyI2C_SendByte(RegAddress);//指定地址
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(Data);//指定数据
//	MyI2C_ReceiveAck();
//	MyI2C_Stop();
	uint32_t TimOut=10000;
	I2C_GenerateSTART(I2C2,ENABLE);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)//因为硬件配置不自带延时所以需要接收EVE5成功条件
		if(TimOut--==0)break;

	
	I2C_Send7bitAddress(I2C2 , MPU6050_ADDRESS , I2C_Direction_Transmitter);
	MPU6050_WaitEvent( I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ;//EV6

	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent( I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);//EV8
	
	I2C_SendData(I2C2,Data);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//EV8_2

	I2C_GenerateSTOP(I2C2,ENABLE);
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
		uint8_t Data;
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);//指定设备
//	MyI2C_ReceiveAck();//应答位，判断是否收到数据
//	MyI2C_SendByte(RegAddress);//指定地址
//	MyI2C_ReceiveAck();
//	
//	MyI2C_Start();//重复起始
//	MyI2C_SendByte(MPU6050_ADDRESS|0x01);
//	MyI2C_ReceiveAck();
//	Data = MyI2C_ReceiveByte();
//	MyI2C_SendAck(1);//是否继续从机应答
//	MyI2C_Stop();
//	return Data;
	I2C_GenerateSTART(I2C2,ENABLE);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);//因为硬件配置不自带延时所以需要接收EVE5成功条件
	
	I2C_Send7bitAddress(I2C2 , MPU6050_ADDRESS , I2C_Direction_Transmitter);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);//EV6

	I2C_SendData(I2C2,RegAddress);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);//EV8
	
	I2C_GenerateSTART(I2C2,ENABLE);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);//EV5
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);//EV6
	
	I2C_AcknowledgeConfig(I2C2,DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);
	while( I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	Data=I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2,ENABLE);
	
	return Data;
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStrucyure;
	I2C_InitStrucyure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStrucyure.I2C_ClockSpeed = 50000;
	I2C_InitStrucyure.I2C_DutyCycle =I2C_DutyCycle_2;
	I2C_InitStrucyure.I2C_Ack =I2C_Ack_Enable;
	I2C_InitStrucyure.I2C_AcknowledgedAddress =I2C_AcknowledgedAddress_7bit;
	I2C_InitStrucyure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2,&I2C_InitStrucyure);
	
	I2C_Cmd(I2C2,ENABLE);
}

void MPU6050_GetData(int16_t* AccX,int16_t* AccY,int16_t* AccZ,
										int16_t* GyroX,int16_t* GyroY,int16_t* GyroZ)
{
	uint8_t DataH,DataL;
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX=(DataH<<8)|DataL;//通过拼接高八位和低八位成16位数据再返回
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY=(DataH<<8)|DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ=(DataH<<8)|DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX=(DataH<<8)|DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY=(DataH<<8)|DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ=(DataH<<8)|DataL;
}

