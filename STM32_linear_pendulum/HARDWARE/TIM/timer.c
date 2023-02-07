#include "sys.h"
#include "timer.h"
 
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//入口参数：  arr：自动重装值  psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);          //定时器时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr;                       //设置自动重装载值	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                     //设置预分频数
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //使能TIM3更新中断
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //TIM3中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;      //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;      //抢占优先级1，原来程序抢占优先级挝0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;             //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                                //初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);                                         //使能TIMx外设						 
}






