#ifndef __MBOTLINUXUSART__
#define __MBOTLINUXUSART__
#include <sys.h>	

#define START   0X11



//��linux���ղ��������ݵ�������ַ��
extern int usartReceiveOneData(float *p_ControlInput,unsigned char *p_crtlFlag);  

//��װ���ݣ�����USART1_Send_String�����ݷ��͸�linux
extern void  usartSendData(float AngularSend,float AngularVelocitySend,float PositionSend,float PositionVelocitySend,float ControlSend,unsigned char ctrlFlag); 
//����ָ���ַ�����ĺ���
void USART_Send_String(unsigned char *p,unsigned short sendSize);     
//�����λѭ������У�飬�õ�У��ֵ��һ���̶�����֤���ݵ���ȷ��
unsigned char getCrc8(unsigned char *ptr, unsigned short len); 


#endif
