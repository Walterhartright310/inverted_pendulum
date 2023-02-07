#include "control.h"
#include "LinuxUsart.h"
#include "usart.h"         //包含printf
#include "sys.h"

u8 Flag_Stop=1,delay_50,delay_flag,Run_Way=0,Inverted_Flag,Move_Direction;             //停止标志位 50ms精准演示标志位
long Encoder_A,position_A=200000,Position_Zero=220000; //编码器的脉冲计数
long Encoder_B,position_B=200000;                      //编码器的脉冲计数
int Moto,Balance_Pwm,Position_Pwm;              //电机PWM变量 应是Motor的 向Moto致敬	
int Voltage;                                    //电池电压采样相关的变量
float Angle_Balance,Last_Angle_Balance,D_Angle_Balance;   //角位移传感器数据
float Balance_KP=60,Balance_KD=10,Position_KP=10,Position_KD=300;  //PID系数
float show_data;//用于显示的数据

float Control;
unsigned char testRece2 = 0x00;



int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //中断优先级分组2
	uart_init(115200); //串口初始化,波115200
	delay_init();//延时初始化
	Encoder_Init_TIM2(); //初始化编码器（TIM2的编码器接口模式） 
	Encoder_Init_TIM4(); //初始化编码器（TIM4的编码器接口模式） 
	MiniBalance_Motor_Init();//电机初始化
	Adc_Init();//模拟量采集
	KEY_Init();//按键初始化
	LED_Init();//LED初始化
	EXTIX_Init();//外部中断初始化
	OLED_Init();//OLED初始化
	delay_ms(10);//延时等待稳定
	MiniBalance_PWM_Init(7199,0);//PWM初始化
	TIM3_Int_Init(49,7199); ///初始化定时中断
while(1)
	  {
			//DataScope();	            //===上位机
			delay_flag=1;	            //===50ms中断精准延时标志位
			oled_show();              //===显示屏打开	  	
			//while(delay_flag);        //===50ms中断精准延时  主要是波形显示上位机需要严格的50ms传输周期 
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