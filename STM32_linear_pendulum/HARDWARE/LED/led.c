#include "sys.h"
#include "led.h"


void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                       //����һ��GPIO_InitStructure�ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);       //ʹ��GPIOA
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;             //�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;                    //GPIOA.4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;            //50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);                      //
	GPIO_SetBits(GPIOA,GPIO_Pin_4);                            //GPIOA.4��ʼ���ø�
}

void LED_Flash(void)
{
 LED_blue=~LED_blue;
 delay_ms(100);
}
