#include "24cxx.h"
#include "common.h"

//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC1_Init();
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
	
    IIC1_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC1_Send_Byte(0XA0);	   		//发送写命令
		IIC1_Wait_Ack();
		IIC1_Send_Byte(ReadAddr>>8);	//发送高地址
		IIC1_Wait_Ack();
	}

    IIC1_Send_Byte(ReadAddr%256);   	//发送低地址
	IIC1_Wait_Ack();
	IIC1_Start();
	IIC1_Send_Byte(0XA1);           	//进入接收模式
	IIC1_Wait_Ack();
    temp=IIC1_Read_Byte(0);
    IIC1_Stop();						//产生一个停止条件
	
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	u8 data = 0;
	u8 times = 0;

	do
	{
		IIC1_Start();
		if(EE_TYPE>AT24C16)
		{
			IIC1_Send_Byte(0XA0);	    	//发送写命令
			IIC1_Wait_Ack();
			IIC1_Send_Byte(WriteAddr>>8);	//发送高地址
		}
		else
		{
			IIC1_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据
		}
		IIC1_Wait_Ack();
		IIC1_Send_Byte(WriteAddr%256);   	//发送低地址
		IIC1_Wait_Ack();
		IIC1_Send_Byte(DataToWrite);     	//发送字节
		IIC1_Wait_Ack();
		IIC1_Stop();						//产生一个停止条件
		delay_ms(10);

 		data = AT24CXX_ReadOneByte(WriteAddr);

		times ++;
		if(times >= 5)
		{
			times = times;
			return;
		}
	}
    while(data != DataToWrite);
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
	}
	return temp;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}











