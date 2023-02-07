#include "sys.h"
#include "motor.h"



//��ʱ��1PWM�����ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//��ڲ�����  arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��1!
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);          //��ʱ��ʱ��ʹ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //ʹ��GPIOA
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);         //ʹ��AFIOʱ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                 //�����������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;           //GPIOA.8..11
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //

	TIM_TimeBaseStructure.TIM_Period = arr;                       //�����Զ���װ��ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler = psc;                    //����Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);               //��ʼ��TIMx��ʱ�������λ
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //PWMģʽ1������ģʽģʽ1or2��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;       //�Ƚ������Ч���ԣ���or�ͣ�
	TIM_OCInitStructure.TIM_Pulse = 0;                            //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;   //���ʹ��
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);                       //��ʼ��PWM�������
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);                       //��ʼ��PWM�������
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	                          //MOE �����ʹ�ܣ�TIM1/8���������������PWM��	
	  
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);              //ʹ������Ƚ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM1,ENABLE);                            //ʹ���Զ���װ�ڵ�Ԥװ�ؼĴ�������λ
	TIM_Cmd(TIM1, ENABLE);                                        //ʹ��TIM1����
}

void MiniBalance_Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitStructure�ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);          //ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                //�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;          //GPIOB.12..13
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);                         //
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);                  //GPIOB.12..13��ʼ���ø�
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                //�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;                       //GPIOA.3
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //
	GPIO_SetBits(GPIOA,GPIO_Pin_3);                               //GPIOA.3��ʼ���ø�
	EN=1; //���Ĭ��ʹ��
}



