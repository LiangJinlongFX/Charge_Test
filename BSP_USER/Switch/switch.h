#ifndef _SWITCH_H
#define _SWITCH_H
#include "sys.h"

#define SW PCout(0)
#define RELAY PCout(13)		//1[����]
#define FAN PAout(15)			//1[����]

void Switch_Init(void);


#endif

