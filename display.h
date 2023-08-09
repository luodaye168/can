#ifndef __display_h
#define __display_h

#include "stm32f10x.h"

#define LED1_ON 		GPIO_ResetBits(GPIOC , GPIO_Pin_1)
#define LED2_ON 		GPIO_ResetBits(GPIOC , GPIO_Pin_2)
#define LED3_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_0)
#define LED1_OFF 		GPIO_SetBits(GPIOC , GPIO_Pin_1)
#define LED2_OFF 		GPIO_SetBits(GPIOC , GPIO_Pin_2)
#define LED3_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_0)

#define	RCC_ZLG_GPIOx 	RCC_APB2Periph_GPIOB
#define ZLG_GPIOx  	GPIOB
#define CS_Pin  	GPIO_Pin_12
#define CLK_Pin 	GPIO_Pin_13
#define DIO_Pin 	GPIO_Pin_15

//��ָ��
#define Test  	0xBF	//��������ܺû�
#define Right 	0xA0	//����
#define Left  	0xA1	//����
#define R_load 	0xA2	//ѭ������
#define L_load 	0xA3	//ѭ������
#define Clear 	0xA4	//����
 
//������ָ��
#define Decode1_wei	  0x80  //�ڶ����ֽ�Ϊ����0-15�����ڵ���ʱ���ϵڼ�λ�������õ���λ��ʾ(Decode1_wei+4), ����ʾ 0123456789-EHLP��
#define Decode2_wei	  0xD0  //�ڶ����ֽ�Ϊ����0-15�����ڵ���ʱ���ϵڼ�λ�������õ���λ��ʾ(Decode2_wei+4), ����ʾ 0123456789ABCDEF
#define Nodecode_wei  0x90  //�����룬�ڶ����ֽڶ�Ӧ����ܵ�ÿһ��
#define	Blink		  0x88	//��˸���ڶ����ֽ�ÿһ�����ض�Ӧһ�������1-8
#define Blanking	  0x98 	//�������ڶ����ֽ�ÿһ�����ض�Ӧһ�������1-8

#define CS(level)	if(level)	\
					GPIO_SetBits(ZLG_GPIOx,CS_Pin);\
					else		\
					GPIO_ResetBits(ZLG_GPIOx,CS_Pin)

#define CLK(level)	if(level)	\
					GPIO_SetBits(ZLG_GPIOx,CLK_Pin);\
					else		\
					GPIO_ResetBits(ZLG_GPIOx,CLK_Pin)

#define DIO(level)	if(level)	\
					GPIO_SetBits(ZLG_GPIOx,DIO_Pin);\
					else		\
					GPIO_ResetBits(ZLG_GPIOx,DIO_Pin)

void zlg7289_GPIO_Config(void);
void send_byte(u8 date_byte, u8 send_byte_num);
void dis_one(u8 cmd, u8 dta);
void dis_int(int int_num);
void dis_p(u16 p_id, int p_num);//parameter����
void LED_GPIO_Config(void);
						
					
#endif
