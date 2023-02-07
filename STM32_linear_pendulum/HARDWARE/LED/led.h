#ifndef __LED_H
#define __LED_H
#include "sys.h"




void LED_Init(void);
void LED_Flash(void);

#define  LED_blue  PAout(4)

#endif
