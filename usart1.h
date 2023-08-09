#ifndef __USART1_H
#define __USART1_H
#include "stm32f10x.h" 

void usart1_init(int dbs);	//PA15
//发送一个字节数据
void usart1_sendbyte(char data);
//发送一串字节数据
void usart1_sendstring(char* data);
//读取一个字节数据
char usart1_Receivebyte(void);

#endif
