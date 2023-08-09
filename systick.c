#include "systick.h"

static unsigned char fac_us  = 0;	//us��ʱ������
static unsigned short fac_ms = 0;	//ms��ʱ������
/*********************************************************************************************************
* �� �� �� : Systick_Inti
* ����˵�� : ��ʼ��ϵͳ�δ�
* ��    �� : clk��ϵͳ����Ƶ��
* �� �� ֵ : ��
* ��    ע : �ⲿʱ��Դ = ϵͳ����Ƶ�� / 8��
*********************************************************************************************************/ 
//void Systick_Inti(unsigned char clk)
//{
//	SysTick->CTRL &= ~(1<<2);	//ѡ���ⲿʱ����Ϊ�δ��ʱ��Դ
//	fac_us = clk / 8.0;			//�õ���1us��Ҫ�Ƶ���
//	fac_ms = clk / 8.0 * 1000;	//�õ���1ms��Ҫ�Ƶ���
//}
void delay_init()
{
#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;				//Ϊϵͳʱ�ӵ�1/8  
#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
	reload=SystemCoreClock/8000000;				//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/delay_ostickspersec;		//����delay_ostickspersec�趨���ʱ��
												//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/delay_ostickspersec;			//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 						//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    

#else
	fac_ms=(u16)fac_us*1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����   
#endif
}				
/*********************************************************************************************************
* �� �� �� : Delay_Us
* ����˵�� : us���ӳ�
* ��    �� : us����Ҫ�ӳٵ�us��
* �� �� ֵ : ��
* ��    ע : ����ӳ�ʱ�� = ��2^24-1��/ 10.5 = 798915(us)��ע�⣺������Ҫ���ж��е���
*********************************************************************************************************/ 
void Delay_Us(unsigned int us)
{
	unsigned int temp = 0;
	SysTick->LOAD = us*fac_us;				//��10.5������1us
	SysTick->VAL  = 0;						//�����ǰ����ֵ
	SysTick->CTRL |= (1<<0);				//ʹ�ܼ�����
	do{										//����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������									
		temp = SysTick->CTRL;				//�ȴ���16λΪ1
	}while(!(temp&(1<<16))&&(temp&(1<<0)));	//����ȷ����ʱ����������
	SysTick->CTRL &= ~(1<<0);				//�رռ�����
}
/*********************************************************************************************************
* �� �� �� : Delay_Ms
* ����˵�� : ms���ӳ�
* ��    �� : ms��Ҫ�ӳٵ�ms��
* �� �� ֵ : ��
* ��    ע : ����ӳ� = ��2^24-1��/ 21 = 798(ms)
*********************************************************************************************************/ 
void Delay_N_Ms(unsigned int Ms)
{
	unsigned int temp = 0;
	SysTick->LOAD = Ms*fac_ms;					//��10.5������1us��1ms��1000us
	SysTick->VAL  = 0;							//�����ǰ����ֵ
	SysTick->CTRL |= (1<<0);					//ʹ�ܼ�����
	do{											//����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������
		temp = SysTick->CTRL;					//�ȴ���16λΪ1
	}while(!(temp&(1<<16))&&(temp&(1<<0)));		//����ȷ����ʱ����������
	SysTick->CTRL &= ~(1<<0);					//�رռ�����
}


void Delay_Ms(unsigned int Ms)  //��ʱʱ��������
{
		int i;
		int bs=Ms/500;   //����
		int ys=Ms%500;   //������
	
		for(i=0;i<bs;i++)  Delay_N_Ms(500);
		
		if(ys!=0) Delay_N_Ms(ys);
	
}

