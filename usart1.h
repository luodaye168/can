#ifndef __USART1_H
#define __USART1_H
#include "stm32f10x.h" 

void usart1_init(int dbs);	//PA15
//����һ���ֽ�����
void usart1_sendbyte(char data);
//����һ���ֽ�����
void usart1_sendstring(char* data);
//��ȡһ���ֽ�����
char usart1_Receivebyte(void);

#endif
