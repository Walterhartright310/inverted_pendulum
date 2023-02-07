#include "sys.h"
#include "exti.h"


 
//外部中断初始化函数
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitStructure结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);          //使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);           //使能AFIO时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;                   //下拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;                       //GPIOA.2
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //初始化GPIO2.
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;                   //下拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;                      //GPIOB.14
	GPIO_Init(GPIOB,&GPIO_InitStructure);                         //初始化GPIOB.14
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);    //GPIOA.2 中断线以及中断初始化配置
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;                      //
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;             //
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;         //
	EXTI_Init(&EXTI_InitStructure);                               //GPIOA.2 中断线以及中断初始化配置

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);   //GPIOB.14 中断线以及中断初始化配置
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;                     //
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;             //
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;         //
	EXTI_Init(&EXTI_InitStructure);                               //GPIOB.14 中断线以及中断初始化配置
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI2_IRQn;                //使能限位开关A所在通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //设置抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //设置子优先级为0
	NVIC_Init(&NVIC_InitStructure);                               //使能限位开关A、B所在通道
	
    NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;            //使能限位开关A、B所在通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //设置抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //设置子优先级为0
	NVIC_Init(&NVIC_InitStructure);                               //使能限位开关A所在通道
}
 
void EXTI2_IRQHandler(void)  //GPIOA.2 对应通道中断服务函数
{ 
	if(KEY_S1==0)			Move_Direction=1;  //向右运动
	EXTI_ClearITPendingBit(EXTI_Line2);                           //清除Line路线挂起位
}

void EXTI15_10_IRQHandler(void)  //GPIOB.14对应通道中断服务函数
{ 
	if(KEY_S2==0)			Move_Direction=2;
	EXTI_ClearITPendingBit(EXTI_Line14);                           //清除Line路线挂起位
}
