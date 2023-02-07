#include "sys.h"
#include "exti.h"


 
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitStructure�ṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);          //ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);           //ʹ��AFIOʱ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;                   //��������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;                       //GPIOA.2
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //��ʼ��GPIO2.
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;                   //��������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;                      //GPIOB.14
	GPIO_Init(GPIOB,&GPIO_InitStructure);                         //��ʼ��GPIOB.14
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);    //GPIOA.2 �ж����Լ��жϳ�ʼ������
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;                      //
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;             //
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;         //
	EXTI_Init(&EXTI_InitStructure);                               //GPIOA.2 �ж����Լ��жϳ�ʼ������

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);   //GPIOB.14 �ж����Լ��жϳ�ʼ������
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;                     //
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;             //
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;         //
	EXTI_Init(&EXTI_InitStructure);                               //GPIOB.14 �ж����Լ��жϳ�ʼ������
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_IRQn;                //ʹ����λ����A����ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //������ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //���������ȼ�Ϊ0
	NVIC_Init(&NVIC_InitStructure);                               //ʹ����λ����A��B����ͨ��
	
    NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;            //ʹ����λ����A��B����ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //������ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //���������ȼ�Ϊ0
	NVIC_Init(&NVIC_InitStructure);                               //ʹ����λ����A����ͨ��
}
 
void EXTI2_IRQHandler(void)  //GPIOA.2 ��Ӧͨ���жϷ�����
{ 
	if(KEY_S1==0)			Move_Direction=1;  //�����˶�
	EXTI_ClearITPendingBit(EXTI_Line2);                           //���Line·�߹���λ
}

void EXTI15_10_IRQHandler(void)  //GPIOB.14��Ӧͨ���жϷ�����
{ 
	if(KEY_S2==0)			Move_Direction=2;
	EXTI_ClearITPendingBit(EXTI_Line14);                           //���Line·�߹���λ
}
