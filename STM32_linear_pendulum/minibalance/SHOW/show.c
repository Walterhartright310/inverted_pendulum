#include "show.h"
#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
unsigned char i,temp;          //��������
unsigned char Send_Count; //������Ҫ���͵����ݸ���
float Vol;
//float ControlInput;


 
/**************************************************************************
�������ܣ�OLED��ʾ
��ڲ�������
����  ֵ����
**************************************************************************/
void oled_show(void)
{
	OLED_ShowString(0,0,"MOTOR:");                //�ڶ���
	if(Flag_Stop==1)OLED_ShowString(65,0,"OFF");  //�ڶ���
	else OLED_ShowString(65,0,"O N");             //�ڶ���
	
	OLED_ShowString(0,10,"LIMITB:");              //�ڶ���
	if(LIMITB==1)OLED_ShowString(65,10,"OFF");    //�ڶ���
	else OLED_ShowString(65,10,"O N");            //�ڶ���
	
	OLED_ShowString(0,20,"Moto:");              //������
	if(Moto>=0) {OLED_ShowNumber(55,20,Moto,4,12);         //�ڶ���
	             OLED_ShowString(40,20,"+");   }           //�ڶ���
	if(Moto<0) { OLED_ShowNumber(55,20,-Moto,4,12);        //�ڶ���
                 OLED_ShowString(40,20,"-");   }         //�ڶ���    
	
	OLED_ShowString(00,30,"VOL  :");            //������
	OLED_ShowNumber(55,30,Voltage/100,2,12);    //������
	OLED_ShowNumber(73,30,Voltage%100,2,12);    //������
	OLED_ShowString(67,30,".");                 //������
	OLED_ShowString(89,30,"V");                 //������
	
	//OLED_ShowString(0,40,"ADC:");               //������
	//OLED_ShowNumber(55,40,Angle_Balance,4,12);  //������
								 
	//OLED_ShowString(0,40,"CtN:");               //������
	//OLED_ShowNumber(30,40,ControlNum,6,12);  //������
	 
	OLED_ShowString(0,50,"Ctr:");                 //������
	OLED_ShowNumber(20,50,(int)Control,8,12);  //������
	//OLED_ShowString(55,50,"P:");                //������
	//OLED_ShowNumber(75,50,position_A,6,12);     //������
	delay_ms(10);
  OLED_Refresh_Gram();	
}
/**************************************************************************
�������ܣ�����ʾ��������λ���������� �ر���ʾ��
��ڲ�������
����  ֵ����
��    �ߣ�ƽ��С��֮��
**************************************************************************/
void DataScope(void)
{   
		DataScope_Get_Channel_Data( Angle_Balance, 1 );      
		DataScope_Get_Channel_Data( D_Angle_Balance, 2 );         
		DataScope_Get_Channel_Data( position_A, 3 );              
//		DataScope_Get_Channel_Data( 0 , 4 );   
//		DataScope_Get_Channel_Data(0, 5 ); //����Ҫ��ʾ�������滻0������
//		DataScope_Get_Channel_Data(0 , 6 );//����Ҫ��ʾ�������滻0������
//		DataScope_Get_Channel_Data(0, 7 );
//		DataScope_Get_Channel_Data( 0, 8 ); 
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
		Send_Count = DataScope_Data_Generate(3);
	//	for( i = 0 ; i < Send_Count; i++) 
		//{
	//	while((USART1->SR&0X40)==0);  
	//	USART1->DR = DataScope_OutPut_Buffer[i]; 
	//	}
}
