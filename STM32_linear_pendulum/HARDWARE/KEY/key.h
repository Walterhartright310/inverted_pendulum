#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

void KEY_Init(void);

u8 click_N_Double (u8 time);    //��������ɨ���˫������ɨ��
//u8 click(void);               //��������ɨ��
u8 Long_Press(void);            //��������ɨ��

void KEY_scan(void);
	
#define KEY2    PAin(5)
#define KEY_S1  PAin(2)
#define KEY_S2  PBin(14)
#define LIMITB PBin(9)
#endif
