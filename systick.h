#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32f10x.h" 

//void Systick_Inti(unsigned char clk);   //��ʱ��ʼ��
void delay_init(void);   //��ʱ��ʼ��

void Delay_Us(unsigned int us);         //΢����ʱ
void Delay_Ms(unsigned int Ms);         //������ʱ

#endif
