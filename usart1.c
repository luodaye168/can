#include "usart1.h"

void usart1_init(int dbs)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//���츴�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef u1;
	u1.USART_BaudRate = 115200;	//������
	u1.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS;	//
	u1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//���ͻ����
	u1.USART_Parity = USART_Parity_No;	//��У��
	u1.USART_StopBits = USART_StopBits_1;	//1��ֹͣλ
	u1.USART_WordLength = USART_WordLength_8b;	//8λ����λ
	
	USART_Init(USART1, &u1);
	
	USART_Cmd(USART1, ENABLE);
}

void usart1_sendbyte(char data)
{
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET));	//�жϵȴ������꣬�˳�ѭ��
	USART_SendData(USART1, data);	//��������
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
		while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));	//�жϵȴ������꣬�˳�ѭ��
		return USART_ReceiveData(USART1);
}


//printf�ض������������

#include <stdio.h>
 
/* ��֪����������C������ʹ�ð������ĺ��� */
#pragma import(__use_no_semihosting)
 
/* ���� _sys_exit() �Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}
 
/* ��׼����Ҫ��֧������ */
struct __FILE
{
    int handle;
};
 
FILE __stdout;
 
/*  */
int fputc(int ch, FILE *stream)
{
	usart1_sendbyte(ch);   //���ڷ�������
    return ch;
}

