#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#define FILTERING_TIMES  4
#define POSITION_MIDDLE 220000
#define POSITION_LEFT 200000
#define POSITION_RIGHT 240000
void Key(void);
int  balance(float angle);
int  Origin_balance(float angle);
void Set_Pwm(int moto);
void Xianfu_Pwm(void);
void key(void);
void Auto_Run(void);
int  Position(int Encoder);
int  Pre_Position(int Encoder);
int  abs(int a);
void Move(long encoder,long target);
int  Position_PID (int Encoder,int Target);
int  Incremental_PI (int Encoder,int Target);
int  Mean_Filter(int sensor);
void Find_Zero(void);
void Auto_run(void);
void Get_D_Angle_Balance(void);
u8 Turn_Off(void);
#define PI 3.14159265


#endif
