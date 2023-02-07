#include "control.h"
#include "LinuxUsart.h"
#include "usart.h"         //包含printf
#include "sys.h"
/**************************************************************************
函数功能：所有的控制代码都在这里面 TIM3定时器5ms定时中断函数			 
作   者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Target,Position_Target;
long Run_Way_Count1,Run_Way_Count2,Run_Way_Flag2,Inverted_Count,Inverted_Count2,Run_Way_Flag1;
long Target_Position,D_Count;
float Ratio=1,Basics_Position_KD=0,Basics_Position_KP=0;


/*计算摆角的角度和角速度*/
static float Last_Position,Position_Bias,Position_Differential;
static float Position_Least;    //最新速度
/*计算滑块的位置和速度*/			
float Bias;   //倾角偏差
static float Last_Bias, D_Bias;   //PID相关变量			
/*需要发送的数据*/			 
float AngularSend, AngularVelocitySend, PositionSend, PositionVelocitySend; 	
unsigned char ctrlFlagSend=0x05;

/*#define Max_BUFF_Len 7
unsigned char  receiveBuff[Max_BUFF_Len];
unsigned int Uart1_Rx=0;*/
float ControlInput=1000;

 int ControlNum;




void TIM3_IRQHandler(void)    //TIM3定时中断服务函数
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		   TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
			     if(delay_flag==1)
			   {
				  if(++delay_50==10)	 delay_50=0,delay_flag=0;  //===给主函数提供50ms的精准延时
			 }
			 Encoder_A=Read_Encoder(4);  //获取电机A编码器的值
			 position_A+=Encoder_A;  //速度积分得到位置
			 Angle_Balance=Get_Adc_Average(8,10); //获取角度 取多次平均值
			 Get_D_Angle_Balance();
			 Key();//获取角度信息
			 EN=1;//电机使能
	
	 if(Flag_Stop==0)  //===等待按键开启平衡
	    {		
			 /*计算倒立摆的四个参数并传给Ros */
	     Bias=Angle_Balance-ANGLE_MIDDLE;   //倒立摆的偏角偏差
	     D_Bias=Bias-Last_Bias; //倒立摆的角速度
	
	     Last_Bias=Bias;  //保存上一次的偏差
			 
			 if(++Position_Target>4)
			 {
			 
       Position_Least =position_A-Position_Zero; //===得到偏差
       Position_Bias *=0.8;	
				 
       Position_Bias += Position_Least*0.2;	  //===一阶低通滤波器  位置变大小
		   Position_Differential=Position_Bias-Last_Position;//偏差的微分 位置的速度大小
				 
	  	 Last_Position=Position_Bias;//保存上一次的偏差
			 Position_Target=0;
			 }	
        AngularSend= Bias;
        AngularVelocitySend= D_Bias;
			  PositionSend= Position_Bias;
			  PositionVelocitySend= Position_Differential; 
			 
			 usartSendData(AngularSend,AngularVelocitySend,PositionSend,PositionVelocitySend,Control,ctrlFlagSend);
	     delay_ms(4);
			
				// if(Run_Way==1)  //进入自动起摆模式 （是否自动起摆，取决于启动的时候角位移传感器的数据）
				// {
				//	Auto_run();//执行自动起摆
				//  }
				//	else	if(Run_Way==2)  //手动起摆，手扶摆杆到平衡位置再开启
				//  {
				//	Balance_Pwm=balance(Angle_Balance-ANGLE_MIDDLE); //倾角PD控制
				//	Basics_Position_KD=200;//人为修改PID参数
				//	Basics_Position_KP=20;
			
			
				//	if(++Position_Target>4)  Position_Pwm=Position(position_A),Position_Target=0;//位置控制
			      Moto= (int) Control;
				//	Moto=Balance_Pwm-Position_Pwm;  //读取PD控制器输出PWM
			 	//	}
					  Xianfu_Pwm();  //===PWM限幅 防止占空比100%带来的系统不稳定因素
					 if(Turn_Off()==0)//判断有没有异常
					  Set_Pwm(Moto); // 没有则驱动电机
					else
				  	Set_Pwm(0); //否则就关闭输出
	      }
		  else Set_Pwm(0); //如果Flag_Stop置1，关闭电机
	 		//Find_Zero(); //找到限位开关和参考位置
			Voltage=Get_Adc(ADC_Channel_9)*3.3*11*100/4096;            //获取电池电压			
		}	 
}




/*void USART1_IRQHandler()
{
 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   {
		 
		 int j;
		 ControlNum+=1;
		 

     USART_ClearITPendingBit(USART1,USART_IT_RXNE);//首先清除中断标志位
  
		 receiveBuff[Uart1_Rx] = USART_ReceiveData(USART1);   //@@@@@#####如果你使用不是USART1更改成相应的，比如USART3
	   Uart1_Rx++;
		 
		 if (receiveBuff[Uart1_Rx-1]==0x0a || Uart1_Rx==Max_BUFF_Len)
		 {
		   if (receiveBuff[0]=='+')
			 {
				 for(j=0;j<4;j++)
				 {
				 ControlInputUnion.data[j]=receiveBuff[j+1];
				 }
				 ControlInput=ControlInputUnion.d;
				   Moto= (int) ControlInput;
				//	Moto=Balance_Pwm-Position_Pwm;  //读取PD控制器输出PWM
			 	//	}
					 Xianfu_Pwm();  //===PWM限幅 防止占空比100%带来的系统不稳定因素
					 if(Turn_Off()==0)//判断有没有异常
					 Set_Pwm(Moto); // 没有则驱动电机
				 
				 Uart1_Rx=0;
			 }
			 else
			 {
			   Uart1_Rx=0;
			 }
		 }
	
  }

}*/

//#define USART_REC_LEN 7

//u8 USART_RX_BUF[USART_REC_LEN];     //????,??USART_REC_LEN???.
//????
//bit15,	??????
//bit14,	???0x0d
//bit13~0,	??????????

//u16 USART_RX_STA=0;       //??????	
 
//?????????

/**************************************************************************
函数功能：倾角PD控制
入口参数：角度
返回  值：倾角控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float angle)
{
	float Bias;   //倾角偏差
	static float Last_Bias, D_Bias;   //PID相关变量
	int balance; //返回值PWM
	Bias=angle;   //偏差
	D_Bias=Bias-Last_Bias; //求出偏差微分
	balance=-Balance_KP*Bias-D_Bias*Balance_KD;  //PD控制，计算倾角控制电机的PWM
	Last_Bias=Bias;  //保存上一次的偏差
	return balance;  //返回PWM
}

/**************************************************************************
函数功能：位置PD控制 
入口参数：编码器
返回  值：位置控制PWM
作    者：平衡小车之家
**************************************************************************/
int Position(int Encoder)
{  
    static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
    static float Position_Least;//最新速度
  	Position_Least =Encoder-Position_Zero; //===得到偏差
    Position_Bias *=0.8;		   
    Position_Bias += Position_Least*0.2;	  //===一阶低通滤波器  
		Position_Differential=Position_Bias-Last_Position;//偏差的微分
		Last_Position=Position_Bias;//保存上一次的偏差
    Position_PWM=Position_Bias*(Position_KP+Basics_Position_KP)/100+Position_Differential*(Position_KD+Basics_Position_KD)/100; //===位置控制	
		return Position_PWM;//返回值
}
/**************************************************************************
函数功能：顺摆位置PD控制 
入口参数：编码器
返回  值：位置控制PWM
作    者：平衡小车之家
**************************************************************************/
int Pre_Position(int Encoder)
{  
    static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
  	Position_Bias =Encoder-Position_Zero; //===得到偏差
		Position_Differential=Position_Bias-Last_Position;//偏差积分
		Last_Position=Position_Bias;//保存上一次偏差
    Position_PWM=Position_Bias*Position_KP/20+Position_Differential*Position_KD/20; //===位置控制	
		return Position_PWM;//返回值
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
    	if(moto>0)		PWMA=moto,        AIN2=1;
			else 	        PWMA=7200+moto,  AIN2=0;
}
/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=4500;    //===PWM满幅是7200 限制在4500
	  if(Moto<-Amplitude) Moto=-Amplitude;	
	  if(Moto>Amplitude)  Moto=Amplitude;		
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(void)
{
	    u8 temp; 
	    static u8 count;
		  	if(position_A<(POSITION_MIDDLE-20000)||position_A>(POSITION_MIDDLE+20000)||LIMITB==0) count++; else count=0;//滑块到两端时或者限位开关闭合并持续600ms自动关闭电机
			if(count==230)
			{
				count=0;//计数器清零
				Flag_Stop=1;//关闭标志位置1
				temp=1;//返回1
			}	
			else temp=0; //否则返回0*/
			
	    //if (LIMITB==0) Flag_Stop=1;
			if((Inverted_Flag||Run_Way==2)&&( Angle_Balance<(ANGLE_MIDDLE-300)||Angle_Balance>(ANGLE_MIDDLE+300)) )//平衡之后如果摆杆下垂，自动关闭电机
			{
				Flag_Stop=1;//关闭标志位置1
				temp=1;//返回1
			}
      return temp;//返回
}

/**************************************************************************
函数功能：绝对值函数
入口参数：无
返回  值：无
**************************************************************************/
int abs(int a)
{int temp;
	if(a<0)  temp=-a;
	else     temp=a;
	return   temp;
}
/**************************************************************************
函数功能：按键修改小车运行状态  控制摆杆的位置
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{	
	int Position=5000; //目标位置 
	static int tmp;
	//static int count;
	tmp=click_N_Double(100); //检测按键
	if(tmp==1)
	{
	   if(Flag_Stop==0)//处于开启状态
			Flag_Stop=1;//那么按键就关闭电机
		 else //处于关闭状态
			{
			    if(Angle_Balance<(ANGLE_MIDDLE+300)&&Angle_Balance>(ANGLE_MIDDLE-300)) //如果摆杆接近平衡位置，则直接开启平衡系统
					Run_Way=2;//手动起摆
					//else if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)&&Inverted_Flag) //如果摆杆自然下垂，则进行自动起摆
					//Run_Way=1;//自动起摆
					Flag_Stop=0;
			}
	}	

	
/*	//	if(Move_Direction==1) //摆杆向右运动
  {
		Position_Zero+=20;
		count+=20;	
		if(count==Position) 	Move_Direction=0,count=0;
	}	
		if(Move_Direction==2) //摆杆向左运动
	{
		Position_Zero-=20;
		count+=20;	
		if(count==Position) 	Move_Direction=0,count=0;
	}	*/
}

/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                   //计算偏差
	 Integral_bias+=Bias;	                                  //求出偏差的积分
		Pwm=Ratio*Bias*0.55+0.00*Integral_bias+8*(Bias-Last_Bias);   //位置式PID控制器
	 Last_Bias=Bias;                                        //保存上一次偏差 
	 return Pwm;                                            //增量输出
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=10*(Bias-Last_bias)/20+10*Bias/20;   //增量式PI控制器
	 Last_bias=Bias;	                                     //保存上一次偏差 
	 return Pwm;                                           //增量输出
}
/**************************************************************************
函数功能：平滑 滤波
入口参数：速度
返回  值：滤波后的数据
**************************************************************************/
int Mean_Filter(int sensor)
{
  u8 i;
  s32 Sum_Speed = 0;     
	s16 Filter_Speed;   
  static  s16 Speed_Buf[FILTERING_TIMES]={0};
  for(i = 1 ; i<FILTERING_TIMES; i++)
  {
    Speed_Buf[i - 1] = Speed_Buf[i];
  }
  Speed_Buf[FILTERING_TIMES - 1] =sensor;

  for(i = 0 ; i < FILTERING_TIMES; i++)
  {
    Sum_Speed += Speed_Buf[i];
  }
  Filter_Speed = (s16)(Sum_Speed / FILTERING_TIMES);//
	return Filter_Speed;
}
/**************************************************************************
函数功能：零位标定
入口参数：无
返回  值：无
**************************************************************************/
void Find_Zero(void)
{
	 static u8 flag;
	 static float count;
	 if(Run_Way==0)  //回到中位
		 {
			if(flag==0)//触碰行程开关
			 {
				 Moto=Incremental_PI(Encoder_A,70); //位置闭环控制
				 if(LIMITB==0)flag=1,position_A=POSITION_RIGHT;//行程开关闭合 就更新位置
			 }
			else if(flag==1)//进入下一步
			{
				 Position_Zero=POSITION_MIDDLE;//设置目标中间位置
				 Balance_Pwm=balance(Angle_Balance-ANGLE_ORIGIN)/8;   //倾角PD控制						
				 Position_Pwm=Pre_Position(position_A);
				 if(position_A>230000) Moto=Incremental_PI(Encoder_A,-70); //位置闭环控制;//离开边缘地方再开始PID控制
				 else	Moto=-Balance_Pwm+Position_Pwm;     //读取PD控制器输出PWM
				 if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)&&(position_A>(POSITION_MIDDLE-2000)&&position_A<(POSITION_MIDDLE+2000)))  count++;
				 if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)) count+=0.1;
				 if(count>200)	Run_Way=1,flag=0,Moto=0,Target_Position=POSITION_MIDDLE-18000;//摆杆运动到中间位置，停止
			}
			if(Moto>2500)	Moto=2500;   //控制位置闭环控制过程的速度
			if(Moto<-2500)Moto=-2500;
			Set_Pwm(Moto);//控制电机
		}
 }		
/**************************************************************************
函数功能：自动起摆
入口参数：无
返回  值：无
**************************************************************************/
void Auto_run(void)
{
	 static float Ratio_Count;
		if(Inverted_Flag==0)  //自动起摆第一步
				{	
					  Ratio_Count++;
					  if(Ratio_Count>1000)Ratio=1+(Ratio_Count-1000)/5000;
					   if(Ratio>2)Ratio=2;
						if(D_Angle_Balance>-15&&D_Angle_Balance<15&&Run_Way_Flag2==1)//摆杆至最高处时候，反方向运动
						{
						 if(Angle_Balance>ANGLE_ORIGIN&&Angle_Balance<(ANGLE_MIDDLE))Target_Position=POSITION_MIDDLE+12000;//控制运动方向
						 else Target_Position=POSITION_MIDDLE-4000;//另一个方向
							Run_Way_Flag2=0;//等待下次换向指令
						} 
						if(Run_Way_Flag2==0)
						{
						if((Angle_Balance>(ANGLE_ORIGIN-300)&&Angle_Balance<(ANGLE_ORIGIN+300)))Run_Way_Flag2=1;//摆杆回中 可以在摆杆角速度接近0的时候执行下一次换向
						}	
						Moto=Position_PID(position_A,Target_Position); //位置闭环控制
						if(Angle_Balance<(ANGLE_MIDDLE+150)&&Angle_Balance>(ANGLE_MIDDLE-150)&&(position_A>210000&&position_A<230000)) Inverted_Count2++; 
						else Inverted_Count2=0; //为防止误检测 多检测几次
						if(Inverted_Count2>8)Inverted_Flag=1;	//摆动的过程如果摆杆接近平衡位置 滑块不在边缘位置 即可开启平衡	
            if(Moto>3000)	Moto=3000;   //控制位置闭环控制过程的速度
					  if(Moto<-4500)Moto=-4500;						
				}
					else	if(Inverted_Flag==1)//到最高的，起摆
				{	
						Balance_Pwm=balance(Angle_Balance-ANGLE_MIDDLE);   //倾角PD控制
						if(Inverted_Count++<30)Position_Zero=position_A;	//延时100ms再开启位置控制							
					  else		if(++Position_Target>4)  Position_Pwm=Position(position_A),Position_Target=0;
						if(Inverted_Count>500&&Inverted_Count<1200)  //稳定之后 让摆杆慢慢回中
						{
							 if(Position_Zero>220000)Position_Zero-=20;
							 if(Position_Zero<220000)Position_Zero+=20;
						}
						if(Inverted_Count>30)Basics_Position_KD=200,Basics_Position_KP=20;
							Moto=Balance_Pwm-Position_Pwm;     //读取PD控制器输出PWM
				}
}
/**************************************************************************
函数功能：获取摆杆角度变化率
入口参数：无
返回  值：无
**************************************************************************/
void Get_D_Angle_Balance(void)
{
		if(++D_Count>5) //获取角度变化率，差分 时间常数25ms
		{
			D_Angle_Balance=Mean_Filter(Angle_Balance-Last_Angle_Balance);	//平滑滤波得到噪声更小的摆杆角速度信息		
			Last_Angle_Balance=Angle_Balance; //保存历史数据
			D_Count=0;	//计数器清零
		}
}
