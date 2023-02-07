#include "sys.h"
#include "adc.h"



		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				//LED0-->PB.0.1 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		                //模拟输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);					            //根据设定参数初始化GPIOB.0

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                   //六分频 72/6=12M/不能超过14M
	ADC_DeInit(ADC1);                                                   //复位ADC1
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;                   //连续触发   开启
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;                //对齐方式   右对齐
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;   //触发方式   软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;                    //ADC模式    独立模式
	ADC_InitStructure.ADC_NbrOfChannel=1;                               //顺序进行规则转换通道数目    1个通道
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;                         //扫描模式    不开启
	ADC_Init(ADC1,&ADC_InitStructure);                                  //安设置初始化
	
	ADC_Cmd(ADC1,ENABLE);                                               //使能ADC1
	
	ADC_ResetCalibration(ADC1);                                         //校准
	while(ADC_GetResetCalibrationStatus(ADC1));                         //校准
	ADC_StartCalibration(ADC1);                                         //校准
	while(ADC_GetCalibrationStatus(ADC1));                              //校准
}				  

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		                        //使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                     //等待转换结束

	return ADC_GetConversionValue(ADC1);	                            //返回最近一次ADC1规则组的转换结果

}

/**************************************************************************
函数功能：读取电池电压 
入口参数：无
返回  值：电池电压 单位MV
作    者：平衡小车之家
**************************************************************************/
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_us(200);
	}
	return temp_val/times;
} 	 


