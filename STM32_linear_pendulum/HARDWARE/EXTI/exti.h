#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h"

void EXTIX_Init(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQnHandler(void);
	

#define LIM_A  PBin(8)
#define LIM_B  PAin(12)
#define LIM_C  PBin(13)
#define LIM_D  PBin(14)

#endif

