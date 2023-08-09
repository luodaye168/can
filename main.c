#include "main.h"

enum OPT opt;
struct Arm arr[31] =
{
  // id   min	max    opt   def_val  cur_val
	{0 ,	 0,	   0,	READ,	  0,	  0},
	{1 ,	 0,	 360,	READ,	  0,	  0},
	{2 ,	 0,	  50,	READ,	  0,	  0},
	{3 ,	 0,	 360,	READ,	  0,	  0},
	{4 ,	 0,	   4,	READ,	  0,	  0},
	{5 ,	 0,  360,	READ,	  0,	  0},
	{6 ,	 0,27648,	READ,	  0,	  0},
	{7 ,	 0,20000,	READ,	  0,	  0},
	{8 ,	 0,	   0,	READ,	  0,	  0},
	{9 ,	 0,	   0,	READ,	  0,	  0},
	{10,	 0,	   0,	READ,	  0,	  0},
	{11,	11,	1100,	WRITE,	110,	110},
	{12,	 0,	   0,	READ,	  0,	  0},
	{13,	13,	1300,	WRITE,	130,	130},
	{14,	 0,	   0,	READ,	  0,	  0},
	{15,	15,	1500,	WRITE,	150,	150},
	{16,	 0,	   0,	READ,	  0,	  0},
	{17,	17,	1700,	WRITE,	170,	170},
	{18,	 0,	   0,	READ,	  0,	  0},
	{19,	19,	1900,	WRITE,	190,	190},
	{20,	 0,	   0,	READ,	  0,	  0},
	{21,	21,	2100,	WRITE,	210,	210},
	{22,	 0,	   0,	READ,	  0,	  0},
	{23,	23,	2300,	WRITE,	230,	230},
	{24,	24,	2400,	WRITE,	240,	240},
	{25,	25,	2500,	WRITE,	250,	250},
	{26,	26,	2600,	WRITE,	260,	260},
	{27,	27,	2700,	WRITE,	270,	270},
	{28,	28,	2800,	WRITE,	280,	280},
	{29,	29,	2900,	WRITE,	290,	290},
	{30,	30,	3000,	WRITE,	300,	300},
 };	

int main()
{
	delay_init();
	LED_GPIO_Config(); 		  //LED 端口初始化
	EXTI_Configuration();	  //按键外部中断配置
	KEY_NVIC_Configuration(); //中断优先级配置 
	CAN_Configuration();	  //can初始化
	CAN_NVIC_Configuration(); 
	usart1_init(115200);
	zlg7289_GPIO_Config();
	send_byte(Clear,1);
	
	printf("\r\nflash: ");
	for(int i = 0; i < 31; i++)
	{
		printf(" %X ", *((u16*)0x08032000 + i));
		arr[i].cur_valu = *((u16*)0x08032000 + i);
	}	
	printf("\r\n");
	
	while(1)
	{
		printf("\r\nflash: ");
		for(u8 i = 0; i < 30; i++,printf(" [%d]:%d ", i,*((u16*)0x08032000 + i)));
		printf("\r\n");
		status == RUN ? dis_p(dis_id,arr[dis_id].cur_valu): (send_byte(Clear,1),dis_int(arr[dis_id].cur_valu));		
		Delay_Ms(1000);
	};
}



