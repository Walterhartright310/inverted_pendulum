#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 

  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/

#define PWMA   TIM1->CCR1            //PORTA.8
#define AIN2   PBout(13)             //
#define PWMB   TIM1->CCR4            //PORTA.11
#define BIN2   PBout(12)             //
#define EN     PAout(3)              //PORTA.3ʹ��λ


void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);

#endif
