#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 

  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/

#define PWMA   TIM1->CCR1            //PORTA.8
#define AIN2   PBout(13)             //
#define PWMB   TIM1->CCR4            //PORTA.11
#define BIN2   PBout(12)             //
#define EN     PAout(3)              //PORTA.3使能位


void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);

#endif
