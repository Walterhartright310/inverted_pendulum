#include "LinuxUsart.h"
#include "usart.h"         //����printf
#include "sys.h"


/*--------------------------------����Э��-----------------------------------
//----------------55 aa size 00 00 00 00 00 crc8 0d 0a----------------------
//����ͷ55aa + �����ֽ���size + ���ݣ����ù����壩 + У��crc8 + ����β0d0a
//ע�⣺����������Ԥ����һ���ֽڵĿ���λ�������Ŀ���������չ������size������
--------------------------------------------------------------------------*/

/*--------------------------------����Э��-----------------------------------
//----------------55 aa size 00 00 00 00 00 crc8 0d 0a----------------------
//����ͷ55aa + �����ֽ���size + ���ݣ����ù����壩 + У��crc8 + ����β0d0a
//ע�⣺����������Ԥ����һ���ֽڵĿ���λ�������Ŀ���������չ������size������
--------------------------------------------------------------------------*/

/**************************************************************************
�������ܣ�ͨ�������жϷ���������ȡ��λ�����͵������ֿ����ٶȡ�Ԥ�����Ʊ�־λ���ֱ���������
��ڲ������������ٿ��Ƶ�ַ���������ٿ��Ƶ�ַ��Ԥ�����Ʊ�־λ
����  ֵ������������
**************************************************************************/


/**************************************************************************
ͨ�ŵķ��ͺ����ͽ��պ��������һЩ���������������������
**************************************************************************/
 
unsigned char  receiveBuff[12] = {0};               
//ͨ��Э�鳣��
const unsigned char header[2]  = {0x55, 0xaa};
//const unsigned char ender[2]   = {0x0d, 0x0a};
const unsigned char ender[3]   = {0x0d, 0x0a,'+'};

//unsigned int Uart_Rx=0;

//�������ݣ������١������١��Ƕȣ������壨-32767 - +32768��
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

//�������ٿ����ٶȹ�����

int usartReceiveOneData(float *p_ControlInput,unsigned char *p_crtlFlag)
{
	unsigned char USART_Receiver              = 0;          //��������
	static unsigned char checkSum             = 0;
	static unsigned char USARTBufferIndex     = 0;
	static short j=0,k=0;
	static unsigned char USARTReceiverFront   = 0;
	static unsigned char Start_Flag           = START;      //һ֡���ݴ��Ϳ�ʼ��־λ
	static short dataLength                   = 0;

 //���ݽ����ݴ���
 //receiveBuff[Uart_Rx]= USART_ReceiveData(USART1);
 //Uart_Rx++;
	USART_Receiver = USART_ReceiveData(USART1);   //@@@@@#####�����ʹ�ò���USART1���ĳ���Ӧ�ģ�����USART3
	//������Ϣͷ
	if(Start_Flag == START)
	{
		if(USART_Receiver == 0xaa)                             //buf[1]
		{  
			if(USARTReceiverFront == 0x55)        //����ͷ��λ //buf[0]
			{
				Start_Flag = !START;              //�յ�����ͷ����ʼ��������
				//printf("header ok\n");
				receiveBuff[0]=header[0];         //buf[0]
				receiveBuff[1]=header[1];         //buf[1]
				USARTBufferIndex = 0;             //��������ʼ��
				checkSum = 0x00;				          //У��ͳ�ʼ��
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
			case 0://���տ����������ݳ���
				receiveBuff[2] = USART_Receiver;
				dataLength     = receiveBuff[2];            //buf[2]
				USARTBufferIndex++;
				break;
			case 1://�������ݴ�������ֵ 
				receiveBuff[j + 3] = USART_Receiver;        //buf[3] - buf[4]					
				j++;
				if(j >= dataLength)                         
				{
					j = 0;
					USARTBufferIndex++;
				}
				break;
			case 2://����У����Ϣ
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
				
			case 3://������Ϣβ
				if(k==0)
				{
					//����0d�����ж�     buf[6]  ????
					k++;
				}
				else if (k==1)
				{
					//����0a buffer[7]

					//���տ���������					
					 for(k = 0; k < 2; k++)
					{
					   ControlInputUnion.data[k]  = receiveBuff[k + 3]; //buf[3]  buf[4]
						
					}				
					
					//�����������ֵ
					*p_ControlInput  = ControlInputUnion.d;
					// *p_rightSpeedSet = (int)rightVelSet.d;
					
					//ctrlFlag
					*p_crtlFlag = receiveBuff[5];                //buf[5]
					//-----------------------------------------------------------------
					//���һ�����ݰ��Ľ��գ���ر������㣬�ȴ���һ�ֽ�����
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
�������ܣ����������ٺͽǶ����ݡ������źŽ��д����ͨ�����ڷ��͸�Linux
��ڲ�����ʵʱ�������١�ʵʱ�������١�ʵʱ�Ƕȡ������źţ����û�нǶ�Ҳ���Բ�����
����  ֵ����
**************************************************************************/



void usartSendData(float AngularSend,float AngularVelocitySend,float PositionSend,float PositionVelocitySend,float ControlSend,unsigned char ctrlFlag)
{
	// Э�����ݻ�������
	unsigned char buf[17] = {0};
	int i;
  unsigned char	length = 0;

	// ���������������ٶ�
	AngularUnion.d  = AngularSend;
	AngularVelocityUnion.d = AngularVelocitySend;
	PositionUnion.d    = PositionSend;
	PositionVelocityUnion.d=PositionVelocitySend;
	ControlUnion.d=ControlSend;
	// ������Ϣͷ
	for(i = 0; i < 2; i++)
		buf[i] = header[i];                      // buf[0] buf[1] 
	
	// ���û������������ٶȡ��Ƕ�
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
	// Ԥ������ָ��
	buf[3 + length - 1] = ctrlFlag;              //  
	
	// ����У��ֵ����Ϣβ
	buf[3 + length] = getCrc8(buf, 3 + length);  // buf[ ]
	buf[3 + length + 1] = ender[0];              // buf[ ]
	buf[3 + length + 2] = ender[1];              // buf[ ]

	//�����ַ�������
	USART_Send_String(buf,sizeof(buf));
}
/**************************************************************************
�������ܣ�����ָ����С���ַ����飬��usartSendData��������
��ڲ����������ַ�������С
����  ֵ����
**************************************************************************/
void USART_Send_String(u8 *p,u16 sendSize)
{ 
	static int length =0;
	while(length<sendSize)
	{   
		//@@@@@#####�����ʹ�ò���USART1���ĳ���Ӧ�ģ�����USART3�������������޸�
		while( !(USART1->SR&(0x01<<7)) );//���ͻ�����Ϊ��
		USART1->DR=*p;                   
		p++;
		length++;
	}
	length =0;
}
/**************************************************************************
�������ܣ������λѭ������У�飬��usartSendData��usartReceiveOneData��������
��ڲ����������ַ�������С
����  ֵ����
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







