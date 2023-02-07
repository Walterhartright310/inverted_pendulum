#include "LinuxUsart.h"
#include "usart.h"         //包含printf
#include "sys.h"


/*--------------------------------发送协议-----------------------------------
//----------------55 aa size 00 00 00 00 00 crc8 0d 0a----------------------
//数据头55aa + 数据字节数size + 数据（利用共用体） + 校验crc8 + 数据尾0d0a
//注意：这里数据中预留了一个字节的控制位，其他的可以自行扩展，更改size和数据
--------------------------------------------------------------------------*/

/*--------------------------------接收协议-----------------------------------
//----------------55 aa size 00 00 00 00 00 crc8 0d 0a----------------------
//数据头55aa + 数据字节数size + 数据（利用共用体） + 校验crc8 + 数据尾0d0a
//注意：这里数据中预留了一个字节的控制位，其他的可以自行扩展，更改size和数据
--------------------------------------------------------------------------*/

/**************************************************************************
函数功能：通过串口中断服务函数，获取上位机发送的左右轮控制速度、预留控制标志位，分别存入参数中
入口参数：左轮轮速控制地址、右轮轮速控制地址、预留控制标志位
返回  值：无特殊意义
**************************************************************************/


/**************************************************************************
通信的发送函数和接收函数必须的一些常量、变量、共用体对象
**************************************************************************/
 
unsigned char  receiveBuff[12] = {0};               
//通信协议常量
const unsigned char header[2]  = {0x55, 0xaa};
//const unsigned char ender[2]   = {0x0d, 0x0a};
const unsigned char ender[3]   = {0x0d, 0x0a,'+'};

//unsigned int Uart_Rx=0;

//发送数据（左轮速、右轮速、角度）共用体（-32767 - +32768）
union sendData
{
	short d;
	unsigned char data[2];
}AngularUnion,AngularVelocityUnion,PositionUnion,PositionVelocityUnion,ControlUnion;

union receiveData
{
	short d;
	unsigned char data[2];
}ControlInputUnion;

//左右轮速控制速度共用体

int usartReceiveOneData(float *p_ControlInput,unsigned char *p_crtlFlag)
{
	unsigned char USART_Receiver              = 0;          //接收数据
	static unsigned char checkSum             = 0;
	static unsigned char USARTBufferIndex     = 0;
	static short j=0,k=0;
	static unsigned char USARTReceiverFront   = 0;
	static unsigned char Start_Flag           = START;      //一帧数据传送开始标志位
	static short dataLength                   = 0;

 //数据接收暂存区
 //receiveBuff[Uart_Rx]= USART_ReceiveData(USART1);
 //Uart_Rx++;
	USART_Receiver = USART_ReceiveData(USART1);   //@@@@@#####如果你使用不是USART1更改成相应的，比如USART3
	//接收消息头
	if(Start_Flag == START)
	{
		if(USART_Receiver == 0xaa)                             //buf[1]
		{  
			if(USARTReceiverFront == 0x55)        //数据头两位 //buf[0]
			{
				Start_Flag = !START;              //收到数据头，开始接收数据
				//printf("header ok\n");
				receiveBuff[0]=header[0];         //buf[0]
				receiveBuff[1]=header[1];         //buf[1]
				USARTBufferIndex = 0;             //缓冲区初始化
				checkSum = 0x00;				          //校验和初始化
			}
		}
		else 
		{
			USARTReceiverFront = USART_Receiver;  
		}
	}
	else
    { 
		switch(USARTBufferIndex)
		{
			case 0://接收控制量的数据长度
				receiveBuff[2] = USART_Receiver;
				dataLength     = receiveBuff[2];            //buf[2]
				USARTBufferIndex++;
				break;
			case 1://接收数据处理，并赋值 
				receiveBuff[j + 3] = USART_Receiver;        //buf[3] - buf[4]					
				j++;
				if(j >= dataLength)                         
				{
					j = 0;
					USARTBufferIndex++;
				}
				break;
			case 2://接收校验信息
				receiveBuff[3 + dataLength] = USART_Receiver;
				checkSum = getCrc8(receiveBuff, 3 + dataLength);
				  // ???????
				if (checkSum != receiveBuff[3 + dataLength]) //buf[5]
				{
					//printf("Received data check sum error!");
					return 0;
				}
				USARTBufferIndex++;
				break;
				
			case 3://接收信息尾
				if(k==0)
				{
					//数据0d不用判断     buf[6]  ????
					k++;
				}
				else if (k==1)
				{
					//数据0a buffer[7]

					//接收控制输入量					
					 for(k = 0; k < 2; k++)
					{
					   ControlInputUnion.data[k]  = receiveBuff[k + 3]; //buf[3]  buf[4]
						
					}				
					
					//输入控制量赋值
					*p_ControlInput  = ControlInputUnion.d;
					// *p_rightSpeedSet = (int)rightVelSet.d;
					
					//ctrlFlag
					*p_crtlFlag = receiveBuff[5];                //buf[5]
					//-----------------------------------------------------------------
					//完成一个数据包的接收，相关变量清零，等待下一字节数据
					USARTBufferIndex   = 0;
					USARTReceiverFront = 0;
					Start_Flag         = START;
					checkSum           = 0;
					dataLength         = 0;
					j = 0;
					k = 0;
					//-----------------------------------------------------------------					
				}
				break;
			 default:break;
		}		
	}
	return 0;
}
/**************************************************************************
函数功能：将左右轮速和角度数据、控制信号进行打包，通过串口发送给Linux
入口参数：实时左轮轮速、实时右轮轮速、实时角度、控制信号（如果没有角度也可以不发）
返回  值：无
**************************************************************************/



void usartSendData(float AngularSend,float AngularVelocitySend,float PositionSend,float PositionVelocitySend,float ControlSend,unsigned char ctrlFlag)
{
	// 协议数据缓存数组
	unsigned char buf[17] = {0};
	int i;
  unsigned char	length = 0;

	// 计算左右轮期望速度
	AngularUnion.d  = AngularSend;
	AngularVelocityUnion.d = AngularVelocitySend;
	PositionUnion.d    = PositionSend;
	PositionVelocityUnion.d=PositionVelocitySend;
	ControlUnion.d=ControlSend;
	// 设置消息头
	for(i = 0; i < 2; i++)
		buf[i] = header[i];                      // buf[0] buf[1] 
	
	// 设置机器人左右轮速度、角度
	length = 11;
	buf[2] = length;                             // buf[2]
	for(i = 0; i < 2; i++)
	{
		buf[i + 3] = AngularUnion.data[i];         // buf[3-6]
		buf[i + 5] = AngularVelocityUnion.data[i];        // buf[7-10]
		buf[i + 7] = PositionUnion.data[i];           // buf[11-14]
    buf[i + 9]	=PositionVelocityUnion.data[i];	// buf[15-18]
		buf[i + 11]	=ControlUnion.data[i];
	}
	// 预留控制指令
	buf[3 + length - 1] = ctrlFlag;              //  
	
	// 设置校验值、消息尾
	buf[3 + length] = getCrc8(buf, 3 + length);  // buf[ ]
	buf[3 + length + 1] = ender[0];              // buf[ ]
	buf[3 + length + 2] = ender[1];              // buf[ ]

	//发送字符串数据
	USART_Send_String(buf,sizeof(buf));
}
/**************************************************************************
函数功能：发送指定大小的字符数组，被usartSendData函数调用
入口参数：数组地址、数组大小
返回  值：无
**************************************************************************/
void USART_Send_String(u8 *p,u16 sendSize)
{ 
	static int length =0;
	while(length<sendSize)
	{   
		//@@@@@#####如果你使用不是USART1更改成相应的，比如USART3，这里有两处修改
		while( !(USART1->SR&(0x01<<7)) );//发送缓冲区为空
		USART1->DR=*p;                   
		p++;
		length++;
	}
	length =0;
}
/**************************************************************************
函数功能：计算八位循环冗余校验，被usartSendData和usartReceiveOneData函数调用
入口参数：数组地址、数组大小
返回  值：无
**************************************************************************/
unsigned char getCrc8(unsigned char *ptr, unsigned short len)
{
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while(len--)
	{
		crc ^= *ptr++;
		for(i = 0; i < 8; i++)
		{
			if(crc&0x01)
                crc=(crc>>1)^0x8C;
			else 
                crc >>= 1;
		}
	}
	return crc;
}




/**********************************END***************************************/







