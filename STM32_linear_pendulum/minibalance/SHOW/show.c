#include "show.h"
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
unsigned char i,temp;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数
float Vol;
//float ControlInput;


 
/**************************************************************************
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{
	OLED_ShowString(0,0,"MOTOR:");                //第二行
	if(Flag_Stop==1)OLED_ShowString(65,0,"OFF");  //第二行
	else OLED_ShowString(65,0,"O N");             //第二行
	
	OLED_ShowString(0,10,"LIMITB:");              //第二行
	if(LIMITB==1)OLED_ShowString(65,10,"OFF");    //第二行
	else OLED_ShowString(65,10,"O N");            //第二行
	
	OLED_ShowString(0,20,"Moto:");              //第三行
	if(Moto>=0) {OLED_ShowNumber(55,20,Moto,4,12);         //第二行
	             OLED_ShowString(40,20,"+");   }           //第二行
	if(Moto<0) { OLED_ShowNumber(55,20,-Moto,4,12);        //第二行
                 OLED_ShowString(40,20,"-");   }         //第二行    
	
	OLED_ShowString(00,30,"VOL  :");            //第四行
	OLED_ShowNumber(55,30,Voltage/100,2,12);    //第四行
	OLED_ShowNumber(73,30,Voltage%100,2,12);    //第四行
	OLED_ShowString(67,30,".");                 //第四行
	OLED_ShowString(89,30,"V");                 //第四行
	
	//OLED_ShowString(0,40,"ADC:");               //第五行
	//OLED_ShowNumber(55,40,Angle_Balance,4,12);  //第五行
								 
	//OLED_ShowString(0,40,"CtN:");               //第五行
	//OLED_ShowNumber(30,40,ControlNum,6,12);  //第五行
	 
	OLED_ShowString(0,50,"Ctr:");                 //第六行
	OLED_ShowNumber(20,50,(int)Control,8,12);  //第六行
	//OLED_ShowString(55,50,"P:");                //第六行
	//OLED_ShowNumber(75,50,position_A,6,12);     //第六行
	delay_ms(10);
  OLED_Refresh_Gram();	
}
/**************************************************************************
函数功能：虚拟示波器往上位机发送数据 关闭显示屏
入口参数：无
返回  值：无
作    者：平衡小车之家
**************************************************************************/
void DataScope(void)
{   
		DataScope_Get_Channel_Data( Angle_Balance, 1 );      
		DataScope_Get_Channel_Data( D_Angle_Balance, 2 );         
		DataScope_Get_Channel_Data( position_A, 3 );              
//		DataScope_Get_Channel_Data( 0 , 4 );   
//		DataScope_Get_Channel_Data(0, 5 ); //用您要显示的数据替换0就行了
//		DataScope_Get_Channel_Data(0 , 6 );//用您要显示的数据替换0就行了
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
