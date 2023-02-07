#include "key.h"
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                       //定义一个GPIO_InitStructure结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);       //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);       //使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;                //下拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_5;         //GPIOA.2..5
	GPIO_Init(GPIOA,&GPIO_InitStructure);                      //初始化GPIOA.2..5
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;                //下拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_14;                   //GPIOB.9 14
	GPIO_Init(GPIOB,&GPIO_InitStructure);                      //初始化GPIOB.14
}

/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
u8 click_N_Double (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEY2==0)  Forever_count++;   //长按标志位未置1
     else        Forever_count=0;
		if(0==KEY2&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return 2;//双击执行的指令
				}
		}
		if(1==KEY2)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return 1;//单击执行的指令
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			}
		}	
		return 0;
}
/**************************************************************************
函数功能：按键扫描
入口参数：无
返回  值：按键状态 0：无动作 1：单击 
**************************************************************************/
u8 click(void)
{
			static u8 flag_key=1;//按键按松开标志
			if(flag_key&&KEY2==0)
			{
			flag_key=0;
			return 1;	// 按键按下
			}
			else if(1==KEY2)			flag_key=1;
			return 0;//无按键按下
}
/**************************************************************************
函数功能：长按检测
入口参数：无
返回  值：按键状态 0：无动作 1：长按2s
**************************************************************************/
u8 Long_Press(void)
{
			static u16 Long_Press_count,Long_Press;
	    if(Long_Press==0&&KEY2==0)  Long_Press_count++;   //长按标志位未置1
      else                       Long_Press_count=0; 
		  if(Long_Press_count>800)		
			{
				Long_Press=1;	
				Long_Press_count=0;
				return 1;
			}				
			 if(Long_Press==1)     //长按标志位置1
			{
				  Long_Press=0;
			}
			return 0;
}
