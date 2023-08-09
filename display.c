#include "main.h"

u16 dis_id = 1;

void zlg7289_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_ZLG_GPIOx, ENABLE);  
	GPIO_InitStructure.GPIO_Pin = CS_Pin | CLK_Pin | DIO_Pin;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //10M
	GPIO_Init(ZLG_GPIOx, &GPIO_InitStructure);  
}


void send_byte(u8 date_byte, u8 send_byte_num)
{
	CS(0);
	for (u8 i=0;i<8;i++)
	{
		DIO(date_byte&0x80);
		CLK(1);
		Delay_Us(10);
		CLK(0);
		Delay_Us(10);
		date_byte <<= 1;  
	}
	if(send_byte_num == 1)
	{
		CS(1);
	}
}

void dis_one(u8 cmd, u8 dta)
{
	send_byte (cmd,2);
	send_byte (dta,2);
	CS(1);
}

void dis_int(int int_num) //-9999 ~ 99999
{
	//����int_num������ʾ��Χ
	if(int_num > 99999 || int_num < -9999)
	{
		dis_one(Decode1_wei + 0, 9);
		dis_one(Decode1_wei + 1, 9);
		dis_one(Decode1_wei + 2, 9);
		dis_one(Decode1_wei + 3, 9);
		dis_one(Decode1_wei + 4, int_num < 0 ? 10 : 9); //����Ǹ���������λ��-
		dis_one(Blink, 0);
		return;
	}
	
	//����int_num�Ƕ���λ��
	u8 order_num = 1;
	int int_num_temp = int_num;
	for(;int_num_temp /= 10;)
		++order_num;
	
	int_num = int_num >= 0 ? int_num : (dis_one(Decode1_wei + order_num, 10),-int_num);//����Ǹ������з�ת����������ǰ����ʾ����
	for(u8 i = 0; i < order_num; ++i)
	{
		dis_one(Decode1_wei + i, int_num%10);
		int_num /= 10;
	}
}

void dis_p(u16 p_id, int p_num)//parameter����
{
	send_byte(Clear,1);
	dis_one(Decode1_wei + 4, 14);
	dis_int(p_id);
	Delay_Ms(1000);
	send_byte(Clear,1);
	dis_int(p_num);
	Delay_Ms(1000);

}




//////////////// LED
void LED_GPIO_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE); 						 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	/*��ʼ����󣬹ر�3��LED*/ 
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
}


