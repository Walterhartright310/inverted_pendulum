#include "sys.h"
#include "timer.h"
 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//��ڲ�����  arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);          //��ʱ��ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr;                       //�����Զ���װ��ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                     //����Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //��ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //ʹ��TIM3�����ж�
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //TIM3�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;      //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;      //��ռ���ȼ�1��ԭ��������ռ���ȼ���0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;             //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                                //��ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);                                         //ʹ��TIMx����						 
}






