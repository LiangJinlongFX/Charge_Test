#ifndef __TLC5615_H
#define __TLC5615_H
////////////////////////////////////////////////////////////////////////////////////	
#include "sys.h"
//¶Ë¿Ú¶¨Òå
#define SPI3_CS PBout(6)	 	  

void TLC5615_Init(void);
void TLC5615_SetVoltage(u16 val);

#endif

