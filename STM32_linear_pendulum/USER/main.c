#include "control.h"
#include "LinuxUsart.h"
#include "usart.h"         //����printf
#include "sys.h"

u8 Flag_Stop=1,delay_50,delay_flag,Run_Way=0,Inverted_Flag,Move_Direction;             //ֹͣ��־λ 50ms��׼��ʾ��־λ
long Encoder_A,position_A=200000,Position_Zero=220000; //���������������
long Encoder_B,position_B=200000;                      //���������������
int Moto,Balance_Pwm,Position_Pwm;              //���PWM���� Ӧ��Motor�� ��Moto�¾�	
int Voltage;                                    //��ص�ѹ������صı���
float Angle_Balance,Last_Angle_Balance,D_Angle_Balance;   //��λ�ƴ���������
float Balance_KP=60,Balance_KD=10,Position_KP=10,Position_KD=300;  //PIDϵ��
float show_data;//������ʾ������

float Control;
unsigned char testRece2 = 0x00;



int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //�ж����ȼ�����2
	uart_init(115200); //���ڳ�ʼ��,��115200
	delay_init();//��ʱ��ʼ��
	Encoder_Init_TIM2(); //��ʼ����������TIM2�ı������ӿ�ģʽ�� 
	Encoder_Init_TIM4(); //��ʼ����������TIM4�ı������ӿ�ģʽ�� 
	MiniBalance_Motor_Init();//�����ʼ��
	Adc_Init();//ģ�����ɼ�
	KEY_Init();//������ʼ��
	LED_Init();//LED��ʼ��
	EXTIX_Init();//�ⲿ�жϳ�ʼ��
	OLED_Init();//OLED��ʼ��
	delay_ms(10);//��ʱ�ȴ��ȶ�
	MiniBalance_PWM_Init(7199,0);//PWM��ʼ��
	TIM3_Int_Init(49,7199); ///��ʼ����ʱ�ж�
while(1)
	  {
			//DataScope();	            //===��λ��
			delay_flag=1;	            //===50ms�жϾ�׼��ʱ��־λ
			oled_show();              //===��ʾ����	  	
			//while(delay_flag);        //===50ms�жϾ�׼��ʱ  ��Ҫ�ǲ�����ʾ��λ����Ҫ�ϸ��50ms�������� 
	  }
		return 0;
}


void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
 	 {
		 ControlInput++;
		 USART_ClearITPendingBit(USART1,USART_IT_RXNE);//?????????
		 //?ROS 
		 usartReceiveOneData(&Control,&testRece2);
	 }
}