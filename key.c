#include "main.h"

u8 status = RUN;
u16 i = 0;
void KEY_NVIC_Configuration(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	
}

/************按键IO配置*******外部中断配置****************************************************/
void EXTI_Configuration(void)
{
	
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;                 
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;                 
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;                 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);
	EXTI_ClearITPendingBit(EXTI_Line7); //清楚外部中断线挂起位
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);  //中断线冲突
//	EXTI_ClearITPendingBit(EXTI_Line8); //清楚外部中断线挂起位
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
	EXTI_ClearITPendingBit(EXTI_Line9); //清楚外部中断线挂起位
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	EXTI_ClearITPendingBit(EXTI_Line11); //清楚外部中断线挂起位
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	EXTI_ClearITPendingBit(EXTI_Line8); //清楚外部中断线挂起位
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//设置为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//设置为上升沿或者下降沿中断请求
	EXTI_InitStructure.EXTI_Line = EXTI_Line11 | EXTI_Line8 | EXTI_Line7 | EXTI_Line9;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //中断使能
	EXTI_Init(&EXTI_InitStructure);
}


/************外部中断处理函数****************************************************/
//gnd	mode  up   down  ok	  cancel
//		pb11  pa8  pc9   pc7  pc8
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
		{
			Delay_Ms(10);
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
			{
				if(arr[dis_id].opt == WRITE)
				{
					status = status == RUN ? (LED3_ON,EDIT) : (LED3_OFF,RUN);
				}
				else
				{
					send_byte(Clear,1);
					dis_one(Decode1_wei + 4, 14);
					dis_int(dis_id);
					dis_one(Blink, 0);
					Delay_Ms(3000);
				}
				
			}
		}
	EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

void EXTI9_5_IRQHandler(void)
{
	/////////up
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
		{
			Delay_Ms(10);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
			{
				switch(status)
				{
					case RUN:
							dis_id > (sizeof(arr)/sizeof(arr[0]) -2 ) ? dis_id : ++dis_id;
							send_byte(Clear,1);
							dis_one(Decode1_wei + 4, 14);//p
							dis_int(dis_id);
							Delay_Ms(350);
							for(i = 0;GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0;Delay_Ms(50 - i/10),i < 490 ? ++i : i)
							{
								dis_id > (sizeof(arr)/sizeof(arr[0]) -2 ) ? dis_id : ++dis_id;
								send_byte(Clear,1);
								dis_one(Decode1_wei + 4, 14);//p
								dis_int(dis_id);
							}	
						break;
					case EDIT:
							if(arr[dis_id].cur_valu >= arr[dis_id].max)
							{
								send_byte(Clear,1);
								dis_int(arr[dis_id].max);
								dis_one(Blink, 0);
								Delay_Ms(3000);
							}
							else
							{
								arr[dis_id].cur_valu >= arr[dis_id].max ? arr[dis_id].cur_valu : ++arr[dis_id].cur_valu;
								send_byte(Clear,1);
								dis_int(arr[dis_id].cur_valu);
								Delay_Ms(350);
								for(i = 0;GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0;Delay_Ms(50 - i/10),i < 490 ? ++i : i)
								{
									arr[dis_id].cur_valu >= arr[dis_id].max ? arr[dis_id].cur_valu : ++arr[dis_id].cur_valu;
									send_byte(Clear,1);
									dis_int(arr[dis_id].cur_valu);
								}
							}
						break;
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
	/////////save
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7) == 0)
		{
			Delay_Ms(10);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7) == 0)
			{
				LED1_ON;
				/**********  写 flash  *************/
				FLASH_Unlock(); //解锁flash
				FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_BSY | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清楚标志位
				FLASH_ErasePage(0x08032000);  //擦除扇区
				for(u8 i = 0; i < 31; i++)    FLASH_ProgramHalfWord(0x08032000+ 2*i , arr[i].cur_valu); //将ARM_DATA写入flash 半字写入
				FLASH_Lock();
				/************************************/
				LED1_OFF;
			}
		}
		
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
	
	///////down
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) == 0)
		{
			Delay_Ms(10);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) == 0)
			{
				switch(status)
				{
					case RUN:
							dis_id > 1 ? --dis_id : dis_id;
							send_byte(Clear,1);
							dis_one(Decode1_wei + 4, 14);//p
							dis_int(dis_id);
							Delay_Ms(350);
							for(i = 0;GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) == 0;Delay_Ms(50 - i/10),i < 490 ? ++i : i)
							{
								dis_id > 1 ? --dis_id : dis_id;
								send_byte(Clear,1);
								dis_one(Decode1_wei + 4, 14);//p
								dis_int(dis_id);
							}					
						break;
					case EDIT:
							if(arr[dis_id].cur_valu <= arr[dis_id].min)
							{
								send_byte(Clear,1);
								dis_int(arr[dis_id].min);
								dis_one(Blink, 0);
								Delay_Ms(3000);
							}
							else
							{
								arr[dis_id].cur_valu <= arr[dis_id].min ? arr[dis_id].cur_valu : --arr[dis_id].cur_valu;
								send_byte(Clear,1);
								dis_int(arr[dis_id].cur_valu);
								Delay_Ms(350);
								for(i = 0;GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) == 0;Delay_Ms(50 - i/10),i < 490 ? ++i : i)
								{
									arr[dis_id].cur_valu <= arr[dis_id].min ? arr[dis_id].cur_valu : --arr[dis_id].cur_valu;
									send_byte(Clear,1);
									dis_int(arr[dis_id].cur_valu);
								}					
							}

						break;
				}
			}


		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

