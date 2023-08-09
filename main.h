#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "stm32f10x.h"                  // Device header
#include "display.h"
#include "usart1.h"
#include "math.h"
#include "key.h"
#include "can.h"
#include "crc.h"
#include "stdio.h"
#include "systick.h"

extern u16 dis_id;
extern u8 status;

extern enum OPT
{
	READ,
	WRITE,
}opt;






extern struct Arm{
	u16 	 id;
	u16 	 min;
	u16 	 max;
	enum OPT opt;
	u16 	 def_value;
	u16 	 cur_valu;
	
}arr[31];


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
