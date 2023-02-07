#include "sys.h"
#include "led.h"


void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                       //定义一个GPIO_InitStructure结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);       //使能GPIOA
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;             //推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;                    //GPIOA.4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;            //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                      //
	GPIO_SetBits(GPIOA,GPIO_Pin_4);                            //GPIOA.4初始化置高
}

void LED_Flash(void)
{
 LED_blue=~LED_blue;
 delay_ms(100);
}
