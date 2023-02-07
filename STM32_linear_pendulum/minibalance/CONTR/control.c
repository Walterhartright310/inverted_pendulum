#include "control.h"
#include "LinuxUsart.h"
#include "usart.h"         //����printf
#include "sys.h"
/**************************************************************************
�������ܣ����еĿ��ƴ��붼�������� TIM3��ʱ��5ms��ʱ�жϺ���			 
��   �ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Target,Position_Target;
long Run_Way_Count1,Run_Way_Count2,Run_Way_Flag2,Inverted_Count,Inverted_Count2,Run_Way_Flag1;
long Target_Position,D_Count;
float Ratio=1,Basics_Position_KD=0,Basics_Position_KP=0;


/*����ڽǵĽǶȺͽ��ٶ�*/
static float Last_Position,Position_Bias,Position_Differential;
static float Position_Least;    //�����ٶ�
/*���㻬���λ�ú��ٶ�*/			
float Bias;   //���ƫ��
static float Last_Bias, D_Bias;   //PID��ر���			
/*��Ҫ���͵�����*/			 
float AngularSend, AngularVelocitySend, PositionSend, PositionVelocitySend; 	
unsigned char ctrlFlagSend=0x05;

/*#define Max_BUFF_Len 7
unsigned char  receiveBuff[Max_BUFF_Len];
unsigned int Uart1_Rx=0;*/
float ControlInput=1000;

 int ControlNum;




void TIM3_IRQHandler(void)    //TIM3��ʱ�жϷ�����
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		   TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
			     if(delay_flag==1)
			   {
				  if(++delay_50==10)	 delay_50=0,delay_flag=0;  //===���������ṩ50ms�ľ�׼��ʱ
			 }
			 Encoder_A=Read_Encoder(4);  //��ȡ���A��������ֵ
			 position_A+=Encoder_A;  //�ٶȻ��ֵõ�λ��
			 Angle_Balance=Get_Adc_Average(8,10); //��ȡ�Ƕ� ȡ���ƽ��ֵ
			 Get_D_Angle_Balance();
			 Key();//��ȡ�Ƕ���Ϣ
			 EN=1;//���ʹ��
	
	 if(Flag_Stop==0)  //===�ȴ���������ƽ��
	    {		
			 /*���㵹���ڵ��ĸ�����������Ros */
	     Bias=Angle_Balance-ANGLE_MIDDLE;   //�����ڵ�ƫ��ƫ��
	     D_Bias=Bias-Last_Bias; //�����ڵĽ��ٶ�
	
	     Last_Bias=Bias;  //������һ�ε�ƫ��
			 
			 if(++Position_Target>4)
			 {
			 
       Position_Least =position_A-Position_Zero; //===�õ�ƫ��
       Position_Bias *=0.8;	
				 
       Position_Bias += Position_Least*0.2;	  //===һ�׵�ͨ�˲���  λ�ñ��С
		   Position_Differential=Position_Bias-Last_Position;//ƫ���΢�� λ�õ��ٶȴ�С
				 
	  	 Last_Position=Position_Bias;//������һ�ε�ƫ��
			 Position_Target=0;
			 }	
        AngularSend= Bias;
        AngularVelocitySend= D_Bias;
			  PositionSend= Position_Bias;
			  PositionVelocitySend= Position_Differential; 
			 
			 usartSendData(AngularSend,AngularVelocitySend,PositionSend,PositionVelocitySend,Control,ctrlFlagSend);
	     delay_ms(4);
			
				// if(Run_Way==1)  //�����Զ����ģʽ ���Ƿ��Զ���ڣ�ȡ����������ʱ���λ�ƴ����������ݣ�
				// {
				//	Auto_run();//ִ���Զ����
				//  }
				//	else	if(Run_Way==2)  //�ֶ���ڣ��ַ��ڸ˵�ƽ��λ���ٿ���
				//  {
				//	Balance_Pwm=balance(Angle_Balance-ANGLE_MIDDLE); //���PD����
				//	Basics_Position_KD=200;//��Ϊ�޸�PID����
				//	Basics_Position_KP=20;
			
			
				//	if(++Position_Target>4)  Position_Pwm=Position(position_A),Position_Target=0;//λ�ÿ���
			      Moto= (int) Control;
				//	Moto=Balance_Pwm-Position_Pwm;  //��ȡPD���������PWM
			 	//	}
					  Xianfu_Pwm();  //===PWM�޷� ��ֹռ�ձ�100%������ϵͳ���ȶ�����
					 if(Turn_Off()==0)//�ж���û���쳣
					  Set_Pwm(Moto); // û�����������
					else
				  	Set_Pwm(0); //����͹ر����
	      }
		  else Set_Pwm(0); //���Flag_Stop��1���رյ��
	 		//Find_Zero(); //�ҵ���λ���غͲο�λ��
			Voltage=Get_Adc(ADC_Channel_9)*3.3*11*100/4096;            //��ȡ��ص�ѹ			
		}	 
}




/*void USART1_IRQHandler()
{
 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   {
		 
		 int j;
		 ControlNum+=1;
		 

     USART_ClearITPendingBit(USART1,USART_IT_RXNE);//��������жϱ�־λ
  
		 receiveBuff[Uart1_Rx] = USART_ReceiveData(USART1);   //@@@@@#####�����ʹ�ò���USART1���ĳ���Ӧ�ģ�����USART3
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
				//	Moto=Balance_Pwm-Position_Pwm;  //��ȡPD���������PWM
			 	//	}
					 Xianfu_Pwm();  //===PWM�޷� ��ֹռ�ձ�100%������ϵͳ���ȶ�����
					 if(Turn_Off()==0)//�ж���û���쳣
					 Set_Pwm(Moto); // û�����������
				 
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
�������ܣ����PD����
��ڲ������Ƕ�
����  ֵ����ǿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float angle)
{
	float Bias;   //���ƫ��
	static float Last_Bias, D_Bias;   //PID��ر���
	int balance; //����ֵPWM
	Bias=angle;   //ƫ��
	D_Bias=Bias-Last_Bias; //���ƫ��΢��
	balance=-Balance_KP*Bias-D_Bias*Balance_KD;  //PD���ƣ�������ǿ��Ƶ����PWM
	Last_Bias=Bias;  //������һ�ε�ƫ��
	return balance;  //����PWM
}

/**************************************************************************
�������ܣ�λ��PD���� 
��ڲ�����������
����  ֵ��λ�ÿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Position(int Encoder)
{  
    static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
    static float Position_Least;//�����ٶ�
  	Position_Least =Encoder-Position_Zero; //===�õ�ƫ��
    Position_Bias *=0.8;		   
    Position_Bias += Position_Least*0.2;	  //===һ�׵�ͨ�˲���  
		Position_Differential=Position_Bias-Last_Position;//ƫ���΢��
		Last_Position=Position_Bias;//������һ�ε�ƫ��
    Position_PWM=Position_Bias*(Position_KP+Basics_Position_KP)/100+Position_Differential*(Position_KD+Basics_Position_KD)/100; //===λ�ÿ���	
		return Position_PWM;//����ֵ
}
/**************************************************************************
�������ܣ�˳��λ��PD���� 
��ڲ�����������
����  ֵ��λ�ÿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Pre_Position(int Encoder)
{  
    static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
  	Position_Bias =Encoder-Position_Zero; //===�õ�ƫ��
		Position_Differential=Position_Bias-Last_Position;//ƫ�����
		Last_Position=Position_Bias;//������һ��ƫ��
    Position_PWM=Position_Bias*Position_KP/20+Position_Differential*Position_KD/20; //===λ�ÿ���	
		return Position_PWM;//����ֵ
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto)
{
    	if(moto>0)		PWMA=moto,        AIN2=1;
			else 	        PWMA=7200+moto,  AIN2=0;
}
/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=4500;    //===PWM������7200 ������4500
	  if(Moto<-Amplitude) Moto=-Amplitude;	
	  if(Moto>Amplitude)  Moto=Amplitude;		
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(void)
{
	    u8 temp; 
	    static u8 count;
		  	if(position_A<(POSITION_MIDDLE-20000)||position_A>(POSITION_MIDDLE+20000)||LIMITB==0) count++; else count=0;//���鵽����ʱ������λ���رպϲ�����600ms�Զ��رյ��
			if(count==230)
			{
				count=0;//����������
				Flag_Stop=1;//�رձ�־λ��1
				temp=1;//����1
			}	
			else temp=0; //���򷵻�0*/
			
	    //if (LIMITB==0) Flag_Stop=1;
			if((Inverted_Flag||Run_Way==2)&&( Angle_Balance<(ANGLE_MIDDLE-300)||Angle_Balance>(ANGLE_MIDDLE+300)) )//ƽ��֮������ڸ��´����Զ��رյ��
			{
				Flag_Stop=1;//�رձ�־λ��1
				temp=1;//����1
			}
      return temp;//����
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�������
����  ֵ����
**************************************************************************/
int abs(int a)
{int temp;
	if(a<0)  temp=-a;
	else     temp=a;
	return   temp;
}
/**************************************************************************
�������ܣ������޸�С������״̬  ���ưڸ˵�λ��
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	
	int Position=5000; //Ŀ��λ�� 
	static int tmp;
	//static int count;
	tmp=click_N_Double(100); //��ⰴ��
	if(tmp==1)
	{
	   if(Flag_Stop==0)//���ڿ���״̬
			Flag_Stop=1;//��ô�����͹رյ��
		 else //���ڹر�״̬
			{
			    if(Angle_Balance<(ANGLE_MIDDLE+300)&&Angle_Balance>(ANGLE_MIDDLE-300)) //����ڸ˽ӽ�ƽ��λ�ã���ֱ�ӿ���ƽ��ϵͳ
					Run_Way=2;//�ֶ����
					//else if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)&&Inverted_Flag) //����ڸ���Ȼ�´���������Զ����
					//Run_Way=1;//�Զ����
					Flag_Stop=0;
			}
	}	

	
/*	//	if(Move_Direction==1) //�ڸ������˶�
  {
		Position_Zero+=20;
		count+=20;	
		if(count==Position) 	Move_Direction=0,count=0;
	}	
		if(Move_Direction==2) //�ڸ������˶�
	{
		Position_Zero-=20;
		count+=20;	
		if(count==Position) 	Move_Direction=0,count=0;
	}	*/
}

/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ���������������λ����Ϣ��Ŀ��λ��
����  ֵ�����PWM
����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_PID (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                   //����ƫ��
	 Integral_bias+=Bias;	                                  //���ƫ��Ļ���
		Pwm=Ratio*Bias*0.55+0.00*Integral_bias+8*(Bias-Last_Bias);   //λ��ʽPID������
	 Last_Bias=Bias;                                        //������һ��ƫ�� 
	 return Pwm;                                            //�������
}
/**************************************************************************
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
��������ʽ��ɢPID��ʽ 
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  �Դ����� 
pwm�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //����ƫ��
	 Pwm+=10*(Bias-Last_bias)/20+10*Bias/20;   //����ʽPI������
	 Last_bias=Bias;	                                     //������һ��ƫ�� 
	 return Pwm;                                           //�������
}
/**************************************************************************
�������ܣ�ƽ�� �˲�
��ڲ������ٶ�
����  ֵ���˲��������
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
�������ܣ���λ�궨
��ڲ�������
����  ֵ����
**************************************************************************/
void Find_Zero(void)
{
	 static u8 flag;
	 static float count;
	 if(Run_Way==0)  //�ص���λ
		 {
			if(flag==0)//�����г̿���
			 {
				 Moto=Incremental_PI(Encoder_A,70); //λ�ñջ�����
				 if(LIMITB==0)flag=1,position_A=POSITION_RIGHT;//�г̿��رպ� �͸���λ��
			 }
			else if(flag==1)//������һ��
			{
				 Position_Zero=POSITION_MIDDLE;//����Ŀ���м�λ��
				 Balance_Pwm=balance(Angle_Balance-ANGLE_ORIGIN)/8;   //���PD����						
				 Position_Pwm=Pre_Position(position_A);
				 if(position_A>230000) Moto=Incremental_PI(Encoder_A,-70); //λ�ñջ�����;//�뿪��Ե�ط��ٿ�ʼPID����
				 else	Moto=-Balance_Pwm+Position_Pwm;     //��ȡPD���������PWM
				 if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)&&(position_A>(POSITION_MIDDLE-2000)&&position_A<(POSITION_MIDDLE+2000)))  count++;
				 if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)) count+=0.1;
				 if(count>200)	Run_Way=1,flag=0,Moto=0,Target_Position=POSITION_MIDDLE-18000;//�ڸ��˶����м�λ�ã�ֹͣ
			}
			if(Moto>2500)	Moto=2500;   //����λ�ñջ����ƹ��̵��ٶ�
			if(Moto<-2500)Moto=-2500;
			Set_Pwm(Moto);//���Ƶ��
		}
 }		
/**************************************************************************
�������ܣ��Զ����
��ڲ�������
����  ֵ����
**************************************************************************/
void Auto_run(void)
{
	 static float Ratio_Count;
		if(Inverted_Flag==0)  //�Զ���ڵ�һ��
				{	
					  Ratio_Count++;
					  if(Ratio_Count>1000)Ratio=1+(Ratio_Count-1000)/5000;
					   if(Ratio>2)Ratio=2;
						if(D_Angle_Balance>-15&&D_Angle_Balance<15&&Run_Way_Flag2==1)//�ڸ�����ߴ�ʱ�򣬷������˶�
						{
						 if(Angle_Balance>ANGLE_ORIGIN&&Angle_Balance<(ANGLE_MIDDLE))Target_Position=POSITION_MIDDLE+12000;//�����˶�����
						 else Target_Position=POSITION_MIDDLE-4000;//��һ������
							Run_Way_Flag2=0;//�ȴ��´λ���ָ��
						} 
						if(Run_Way_Flag2==0)
						{
						if((Angle_Balance>(ANGLE_ORIGIN-300)&&Angle_Balance<(ANGLE_ORIGIN+300)))Run_Way_Flag2=1;//�ڸ˻��� �����ڰڸ˽��ٶȽӽ�0��ʱ��ִ����һ�λ���
						}	
						Moto=Position_PID(position_A,Target_Position); //λ�ñջ�����
						if(Angle_Balance<(ANGLE_MIDDLE+150)&&Angle_Balance>(ANGLE_MIDDLE-150)&&(position_A>210000&&position_A<230000)) Inverted_Count2++; 
						else Inverted_Count2=0; //Ϊ��ֹ���� ���⼸��
						if(Inverted_Count2>8)Inverted_Flag=1;	//�ڶ��Ĺ�������ڸ˽ӽ�ƽ��λ�� ���鲻�ڱ�Եλ�� ���ɿ���ƽ��	
            if(Moto>3000)	Moto=3000;   //����λ�ñջ����ƹ��̵��ٶ�
					  if(Moto<-4500)Moto=-4500;						
				}
					else	if(Inverted_Flag==1)//����ߵģ����
				{	
						Balance_Pwm=balance(Angle_Balance-ANGLE_MIDDLE);   //���PD����
						if(Inverted_Count++<30)Position_Zero=position_A;	//��ʱ100ms�ٿ���λ�ÿ���							
					  else		if(++Position_Target>4)  Position_Pwm=Position(position_A),Position_Target=0;
						if(Inverted_Count>500&&Inverted_Count<1200)  //�ȶ�֮�� �ðڸ���������
						{
							 if(Position_Zero>220000)Position_Zero-=20;
							 if(Position_Zero<220000)Position_Zero+=20;
						}
						if(Inverted_Count>30)Basics_Position_KD=200,Basics_Position_KP=20;
							Moto=Balance_Pwm-Position_Pwm;     //��ȡPD���������PWM
				}
}
/**************************************************************************
�������ܣ���ȡ�ڸ˽Ƕȱ仯��
��ڲ�������
����  ֵ����
**************************************************************************/
void Get_D_Angle_Balance(void)
{
		if(++D_Count>5) //��ȡ�Ƕȱ仯�ʣ���� ʱ�䳣��25ms
		{
			D_Angle_Balance=Mean_Filter(Angle_Balance-Last_Angle_Balance);	//ƽ���˲��õ�������С�İڸ˽��ٶ���Ϣ		
			Last_Angle_Balance=Angle_Balance; //������ʷ����
			D_Count=0;	//����������
		}
}
