#include "sys.h"
#include "adc.h"



		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				//LED0-->PB.0.1 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		                //ģ������
    GPIO_Init(GPIOB, &GPIO_InitStructure);					            //�����趨������ʼ��GPIOB.0

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                   //����Ƶ 72/6=12M/���ܳ���14M
	ADC_DeInit(ADC1);                                                   //��λADC1
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;                   //��������   ����
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;                //���뷽ʽ   �Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;   //������ʽ   �������
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;                    //ADCģʽ    ����ģʽ
	ADC_InitStructure.ADC_NbrOfChannel=1;                               //˳����й���ת��ͨ����Ŀ    1��ͨ��
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;                         //ɨ��ģʽ    ������
	ADC_Init(ADC1,&ADC_InitStructure);                                  //�����ó�ʼ��
	
	ADC_Cmd(ADC1,ENABLE);                                               //ʹ��ADC1
	
	ADC_ResetCalibration(ADC1);                                         //У׼
	while(ADC_GetResetCalibrationStatus(ADC1));                         //У׼
	ADC_StartCalibration(ADC1);                                         //У׼
	while(ADC_GetCalibrationStatus(ADC1));                              //У׼
}				  

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		                        //ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                     //�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	                            //�������һ��ADC1�������ת�����

}

/**************************************************************************
�������ܣ���ȡ��ص�ѹ 
��ڲ�������
����  ֵ����ص�ѹ ��λMV
��    �ߣ�ƽ��С��֮��
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


