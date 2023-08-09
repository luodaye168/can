#ifndef __key_h
#define __key_h

#include "stm32f10x.h"

enum {
	RUN,
	EDIT
};


//gnd	mode  up   down  ok	  cancel
//		pb11  pa8  pc9   pc8  pc7

void EXTI_Configuration(void);
void KEY_NVIC_Configuration(void);



#endif
