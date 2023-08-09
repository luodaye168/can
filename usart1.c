#include "usart1.h"

void usart1_init(int dbs)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//推挽复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef u1;
	u1.USART_BaudRate = 115200;	//波特率
	u1.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS;	//
	u1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//发送或接收
	u1.USART_Parity = USART_Parity_No;	//无校验
	u1.USART_StopBits = USART_StopBits_1;	//1个停止位
	u1.USART_WordLength = USART_WordLength_8b;	//8位数据位
	
	USART_Init(USART1, &u1);
	
	USART_Cmd(USART1, ENABLE);
}

void usart1_sendbyte(char data)
{
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET));	//判断等待发送完，退出循环
	USART_SendData(USART1, data);	//发送数据
}

void usart1_sendstring(char* data)
{
	while(*data)
	{
		usart1_sendbyte(*data++);
	}
}

char usart1_Receivebyte(void)
{
		while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));	//判断等待发送完，退出循环
		return USART_ReceiveData(USART1);
}


//printf重定向输出到串口

#include <stdio.h>
 
/* 告知连接器不从C库链接使用半主机的函数 */
#pragma import(__use_no_semihosting)
 
/* 定义 _sys_exit() 以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}
 
/* 标准库需要的支持类型 */
struct __FILE
{
    int handle;
};
 
FILE __stdout;
 
/*  */
int fputc(int ch, FILE *stream)
{
	usart1_sendbyte(ch);   //串口发送数据
    return ch;
}

