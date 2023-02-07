#include "sys.h"
#include "motor.h"



//定时器1PWM输出初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//入口参数：  arr：自动重装值  psc：时钟预分频数
//这里使用的是定时器1!
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);          //定时器时钟使能
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //使能GPIOA
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);         //使能AFIO时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                 //复用推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;           //GPIOA.8..11
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //

	TIM_TimeBaseStructure.TIM_Period = arr;                       //设置自动重装载值	 
	TIM_TimeBaseStructure.TIM_Prescaler = psc;                    //设置预分频数
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);               //初始化TIMx的时间基数单位
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;               //PWM模式1（两种模式模式1or2）
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;       //比较输出有效极性（高or低）
	TIM_OCInitStructure.TIM_Pulse = 0;                            //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;   //输出使能
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);                       //初始化PWM输出参数
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);                       //初始化PWM输出参数
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	                          //MOE 主输出使能（TIM1/8必须有这句才能输出PWM）	
	  
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);              //使能输出比较预装载寄存器
	TIM_ARRPreloadConfig(TIM1,ENABLE);                            //使能自动重装在的预装载寄存器允许位
	TIM_Cmd(TIM1, ENABLE);                                        //使能TIM1外设
}

void MiniBalance_Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitStructure结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);          //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);          //使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                //推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;          //GPIOB.12..13
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);                         //
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);                  //GPIOB.12..13初始化置高
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                //推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;                       //GPIOA.3
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;               //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                         //
	GPIO_SetBits(GPIOA,GPIO_Pin_3);                               //GPIOA.3初始化置高
	EN=1; //电机默认使能
}



