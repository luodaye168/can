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

//纯指令
#define Test  	0xBF	//测试数码管好坏
#define Right 	0xA0	//右移
#define Left  	0xA1	//左移
#define R_load 	0xA2	//循环右移
#define L_load 	0xA3	//循环左移
#define Clear 	0xA4	//清屏
 
//带数据指令
#define Decode1_wei	  0x80  //第二个字节为编码0-15，需在调用时加上第几位，比如让第五位显示(Decode1_wei+4), 可显示 0123456789-EHLP空
#define Decode2_wei	  0xD0  //第二个字节为编码0-15，需在调用时加上第几位，比如让第五位显示(Decode2_wei+4), 可显示 0123456789ABCDEF
#define Nodecode_wei  0x90  //不译码，第二个字节对应数码管的每一段
#define	Blink		  0x88	//闪烁，第二个字节每一个比特对应一个数码管1-8
#define Blanking	  0x98 	//消隐，第二个字节每一个比特对应一个数码管1-8

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
void dis_p(u16 p_id, int p_num);//parameter参数
void LED_GPIO_Config(void);
						
					
#endif
