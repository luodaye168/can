#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32f10x.h" 

//void Systick_Inti(unsigned char clk);   //延时初始化
void delay_init(void);   //延时初始化

void Delay_Us(unsigned int us);         //微秒延时
void Delay_Ms(unsigned int Ms);         //毫秒延时

#endif
