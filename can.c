#include "main.h"

#define SELF_ID 0x01    //下位机地址默认0x01
u8 CAN1_INIT_OK = 1; //can1初始化状态

unsigned char CAN1_RX_DATA[7];  
unsigned char CAN1_TX_DATA[7];
u16 Function_ID = 0;    //第二第三字节 0~15位 指令
u16 Arr_ID = 0;         //第二第三字节 0~14位 数组下标
u16 Return_ID = SELF_ID;   //
u16 Two_Byte_Data = 0;  //第四第五字节
u16 crc = 0;



//格式： 波特率  CAN_SJW 	CAN_BS1  CAN_BS2 CAN_Prescaler
const unsigned int CAN_baud_table[CAN_BAUD_NUM][5] = 
{
//波特率， CAN_SJW，   CAN_BS1，    CAN_BS2，CAN_Prescaler 
	{5,   CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,450},		//未通			
	{10,  CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_2tq, 400},		//未通			
	{15,  CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,150},		//15K
	{20,  CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,200},		//20k
	{25,  CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,90},			//25k
	{40,  CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,100},		//40k
	{50,  CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,45},			//50k
	{62,  CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,36},			//62.5k
	{80,  CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,50},			//80k
	{100, CAN_SJW_1tq,CAN_BS1_5tq, CAN_BS2_2tq,45},			//100K
	{125, CAN_SJW_1tq,CAN_BS1_13tq, CAN_BS2_2tq,18},		//125K
	{200, CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,20},			//200K
	{250, CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,9},		    //250k
	{400, CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,10},			//400K
	{500, CAN_SJW_1tq,CAN_BS1_3tq, CAN_BS2_5tq,8},			//500K
	{666, CAN_SJW_1tq,CAN_BS1_5tq, CAN_BS2_2tq,8},			//未通
	{800, CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,5},			//800K
	{1000,CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,4},			//1000K
};


/***********************************************************************

注    意：
CAN_SJW : CAN_SJW_1tq - CAN_SJW_4tq	  不能比任何一相位缓冲段长
CAN_BS1 : CAN_BS1_1tq - CAN_BS1_16tq
CAN_BS2 : CAN_BS2_1tq - CAN_BS2_8tq
CAN_Prescaler : 1 - 1024
	配置说明：
CAN_SJW + CAN_BS1 / (CAN_SJW + CAN_BS1 + CAN_BS2)
	0.75     baud > 800k
	0.80     baud > 500k
	0.875    baud <= 500k
	baud = 36 / (CAN_SJW + CAN_BS1 + CAN_BS2) / CAN_Prescaler
***********************************************************************/
void CAN_Baud_Process(unsigned int Baud,CAN_InitTypeDef *CAN_InitStructure)
{
	unsigned int i = 0;
	for(i = 0;i < CAN_BAUD_NUM;i ++)
	{
		if(Baud == CAN_baud_table[i][0])
		{
			CAN_InitStructure->CAN_SJW = CAN_baud_table[i][1];
			CAN_InitStructure->CAN_BS1 = CAN_baud_table[i][2];
			CAN_InitStructure->CAN_BS2 = CAN_baud_table[i][3];
			CAN_InitStructure->CAN_Prescaler = CAN_baud_table[i][4];
			break;	
		}
	}	
}

void CAN_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	/* Configure CAN1 pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	/* Configure CAN1 pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE; /* 时间触发禁止, 时间触发：CAN硬件的内部定时器被激活，并且被用于产生时间戳 */
	CAN_InitStructure.CAN_ABOM = DISABLE; /* 自动离线禁止，自动离线：一旦硬件监控到128次11个隐性位，就自动退出离线状态。在这里要软件设定后才能退出 */
	CAN_InitStructure.CAN_AWUM = DISABLE; /* 自动唤醒禁止，有报文来的时候自动退出休眠	*/
	CAN_InitStructure.CAN_NART = DISABLE; /* 报文重传, 如果错误一直传到成功止，否则只传一次 */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* 接收FIFO锁定, 1--锁定后接收到新的报文摘不要，0--接收到新的报文则覆盖前一报文	*/
	CAN_InitStructure.CAN_TXFP = ENABLE;  /* 发送优先级  0---由标识符决定  1---由发送请求顺序决定	*/
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* 模式	CAN_Mode_Normal CAN_Mode_LoopBack*/ 

	CAN_Baud_Process(250,&CAN_InitStructure);//波特率250K

	if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
	{
		//printf("can1 init error");
		CAN1_INIT_OK = 2;
		/* 初始化时先设置CAN_MCR的初始化位 */  														
		/* 然后查看硬件是否真的设置了CAN_MSR的初始化位来确认是否进入了初始化模式  */		 													                  
	}
	else CAN1_INIT_OK = 0;

	//CAN1滤波器设置
	CAN_FilterInitStructure.CAN_FilterNumber = 0;     ///* 过滤器组 */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; ///* 32位 */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;  
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* 挂号中断, 进入中断后读fifo的报文函数释放报文清中断标志 */
}


void  can1_tx(unsigned int ID,unsigned char* can_send_data)
{
	unsigned char i;
	CanTxMsg TxMessage;
	TxMessage.StdId=ID;	//标准标识符为0x00
    TxMessage.ExtId=0x0000; //扩展标识符0x0000
	TxMessage.IDE = CAN_ID_STD;//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA; /* 设置为数据帧 */
	TxMessage.DLC = 7;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	for(i = 0;i < 7; i ++)
	{
		TxMessage.Data[i] = can_send_data[i];//can_send_data[i];
	}

	u8 res = CAN_Transmit(CAN1,&TxMessage);
	//printf("can1_tx return: %d\r\n",res);  /* 返回这个信息请求发送的邮箱号0,1,2或没有空闲4 */	
}

//CAN1中断程序
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage; 
	RxMessage.StdId=0x01;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0x07;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;     
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);  /* 此函数包含释放提出报文了的,在非必要时,不需要自己释放 */
/******************************************************************************************/	
/***************************handle RxMessage.Data******************************************/
/******************************************************************************************/	
	
	printf("RxMessage_data: ");
	for(int i = 0; i < 7; i++)
	{
		//printf(" %X \t",RxMessage.Data[i]);
		CAN1_RX_DATA[i] = RxMessage.Data[i];
		CAN1_TX_DATA[i] = RxMessage.Data[i];
	}
	Arr_ID = ((CAN1_RX_DATA[1] << 8) + CAN1_RX_DATA[2]) & 0x7fff;
	Function_ID = (CAN1_RX_DATA[1] << 8) + CAN1_RX_DATA[2];
	Two_Byte_Data = (CAN1_RX_DATA[3]<<8) + CAN1_RX_DATA[4];
	crc = (CAN1_RX_DATA[5]<<8) + CAN1_RX_DATA[6];
	
	if(CAN1_RX_DATA[0] == SELF_ID && crc == CRC16(CAN1_RX_DATA,5))
	{	
		switch(Function_ID)
		{
			case 0xC005:  //保存参数指令
				if(Two_Byte_Data == 0xA151)
				{
					printf("保存参数");

				    /**********  写 flash  *************/
					FLASH_Unlock(); //解锁flash
					FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_BSY | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清楚标志位
					FLASH_ErasePage(0x08032000);  //擦除扇区
					for(u16 i = 0; i < 31; i++)    FLASH_ProgramHalfWord(0x08032000+ 2*i , arr[i].cur_valu); //将ARM_DATA写入flash 半字写入
					FLASH_Lock();
					/************************************/
					
//					CAN1_TX_DATA[0] = Return_ID;
					CAN1_TX_DATA[3] = 0x00;
					CAN1_TX_DATA[4] = 0x00;
					crc = CRC16(CAN1_TX_DATA,5);
					CAN1_TX_DATA[5] = crc >> 8;
					CAN1_TX_DATA[6] = crc & 0x00ff;
					can1_tx(SELF_ID,CAN1_TX_DATA);
				}
				else if(Two_Byte_Data == 0xA153)
				{
					printf("恢复出厂参数");
					for(u16 i = 0; i < 31; i++)    arr[i].cur_valu = arr[i].def_value;	
					CAN1_TX_DATA[3] = 0x00;
					CAN1_TX_DATA[4] = 0x00;
					crc = CRC16(CAN1_TX_DATA,5);
					CAN1_TX_DATA[5] = crc >> 8;
					CAN1_TX_DATA[6] = crc & 0x00ff;
					can1_tx(SELF_ID,CAN1_TX_DATA);
				}
				else printf("\r\n保存参数指令安全校验位错误\r\n");
			break;
			default: 

				if(Arr_ID > 30)
				{
					printf("Arr_ID越界 0 ~ 30");	
					break;
				}
				switch(CAN1_RX_DATA[1]>>7)//读写控制位 0读 1写
				{
					case 0: 
							printf("\r\n**** 读 Arm_Data[%d] = %X **** \r\n", Arr_ID, arr[Arr_ID].cur_valu);
//							CAN1_TX_DATA[0] = Return_ID;
							CAN1_TX_DATA[3] = arr[Arr_ID].cur_valu >> 8;
							CAN1_TX_DATA[4] =arr[Arr_ID].cur_valu & 0x00ff;
							crc = CRC16(CAN1_TX_DATA,5);
							CAN1_TX_DATA[5] = crc >> 8;
							CAN1_TX_DATA[6] = crc & 0x00ff;
							can1_tx(SELF_ID,CAN1_TX_DATA);
						break;
					case 1:
							printf("\r\n**** 写 Arm_Data[%d] %X **** \r\n", Arr_ID, Two_Byte_Data);
							arr[Arr_ID].cur_valu = Two_Byte_Data;
//							CAN1_TX_DATA[0] = Return_ID;
							crc = CRC16(CAN1_TX_DATA,5);
							CAN1_TX_DATA[5] = crc >> 8;
							CAN1_TX_DATA[6] = crc & 0x00ff;
							can1_tx(SELF_ID,CAN1_TX_DATA);
						break;
					default:
							printf("参数错误");
						break;
				}
			break;
		}
		
	}
/**************************************************************************************/	
/***************************handle RxMessage.Data******************************************/	
/**************************************************************************************/	
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* 清除挂起中断 */
	
	
}




void CAN_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

