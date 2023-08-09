#include "main.h"

#define SELF_ID 0x01    //��λ����ַĬ��0x01
u8 CAN1_INIT_OK = 1; //can1��ʼ��״̬

unsigned char CAN1_RX_DATA[7];  
unsigned char CAN1_TX_DATA[7];
u16 Function_ID = 0;    //�ڶ������ֽ� 0~15λ ָ��
u16 Arr_ID = 0;         //�ڶ������ֽ� 0~14λ �����±�
u16 Return_ID = SELF_ID;   //
u16 Two_Byte_Data = 0;  //���ĵ����ֽ�
u16 crc = 0;



//��ʽ�� ������  CAN_SJW 	CAN_BS1  CAN_BS2 CAN_Prescaler
const unsigned int CAN_baud_table[CAN_BAUD_NUM][5] = 
{
//�����ʣ� CAN_SJW��   CAN_BS1��    CAN_BS2��CAN_Prescaler 
	{5,   CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq,450},		//δͨ			
	{10,  CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_2tq, 400},		//δͨ			
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
	{666, CAN_SJW_1tq,CAN_BS1_5tq, CAN_BS2_2tq,8},			//δͨ
	{800, CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,5},			//800K
	{1000,CAN_SJW_1tq,CAN_BS1_6tq, CAN_BS2_2tq,4},			//1000K
};


/***********************************************************************

ע    �⣺
CAN_SJW : CAN_SJW_1tq - CAN_SJW_4tq	  ���ܱ��κ�һ��λ����γ�
CAN_BS1 : CAN_BS1_1tq - CAN_BS1_16tq
CAN_BS2 : CAN_BS2_1tq - CAN_BS2_8tq
CAN_Prescaler : 1 - 1024
	����˵����
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
	CAN_InitStructure.CAN_TTCM = DISABLE; /* ʱ�䴥����ֹ, ʱ�䴥����CANӲ�����ڲ���ʱ����������ұ����ڲ���ʱ��� */
	CAN_InitStructure.CAN_ABOM = DISABLE; /* �Զ����߽�ֹ���Զ����ߣ�һ��Ӳ����ص�128��11������λ�����Զ��˳�����״̬��������Ҫ����趨������˳� */
	CAN_InitStructure.CAN_AWUM = DISABLE; /* �Զ����ѽ�ֹ���б�������ʱ���Զ��˳�����	*/
	CAN_InitStructure.CAN_NART = DISABLE; /* �����ش�, �������һֱ�����ɹ�ֹ������ֻ��һ�� */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* ����FIFO����, 1--��������յ��µı���ժ��Ҫ��0--���յ��µı����򸲸�ǰһ����	*/
	CAN_InitStructure.CAN_TXFP = ENABLE;  /* �������ȼ�  0---�ɱ�ʶ������  1---�ɷ�������˳�����	*/
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* ģʽ	CAN_Mode_Normal CAN_Mode_LoopBack*/ 

	CAN_Baud_Process(250,&CAN_InitStructure);//������250K

	if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
	{
		//printf("can1 init error");
		CAN1_INIT_OK = 2;
		/* ��ʼ��ʱ������CAN_MCR�ĳ�ʼ��λ */  														
		/* Ȼ��鿴Ӳ���Ƿ����������CAN_MSR�ĳ�ʼ��λ��ȷ���Ƿ�����˳�ʼ��ģʽ  */		 													                  
	}
	else CAN1_INIT_OK = 0;

	//CAN1�˲�������
	CAN_FilterInitStructure.CAN_FilterNumber = 0;     ///* �������� */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* ����ģʽ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; ///* 32λ */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;  
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;  ///* �ܹ�ͨ���ù������ı��Ĵ浽fifo0�� */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* �Һ��ж�, �����жϺ��fifo�ı��ĺ����ͷű������жϱ�־ */
}


void  can1_tx(unsigned int ID,unsigned char* can_send_data)
{
	unsigned char i;
	CanTxMsg TxMessage;
	TxMessage.StdId=ID;	//��׼��ʶ��Ϊ0x00
    TxMessage.ExtId=0x0000; //��չ��ʶ��0x0000
	TxMessage.IDE = CAN_ID_STD;//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA; /* ����Ϊ����֡ */
	TxMessage.DLC = 7;            /* ���ݳ���, can���Ĺ涨�������ݳ���Ϊ8�ֽ� */
	for(i = 0;i < 7; i ++)
	{
		TxMessage.Data[i] = can_send_data[i];//can_send_data[i];
	}

	u8 res = CAN_Transmit(CAN1,&TxMessage);
	//printf("can1_tx return: %d\r\n",res);  /* ���������Ϣ�����͵������0,1,2��û�п���4 */	
}

//CAN1�жϳ���
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage; 
	RxMessage.StdId=0x01;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0x07;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;     
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);  /* �˺��������ͷ���������˵�,�ڷǱ�Ҫʱ,����Ҫ�Լ��ͷ� */
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
			case 0xC005:  //�������ָ��
				if(Two_Byte_Data == 0xA151)
				{
					printf("�������");

				    /**********  д flash  *************/
					FLASH_Unlock(); //����flash
					FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_BSY | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //�����־λ
					FLASH_ErasePage(0x08032000);  //��������
					for(u16 i = 0; i < 31; i++)    FLASH_ProgramHalfWord(0x08032000+ 2*i , arr[i].cur_valu); //��ARM_DATAд��flash ����д��
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
					printf("�ָ���������");
					for(u16 i = 0; i < 31; i++)    arr[i].cur_valu = arr[i].def_value;	
					CAN1_TX_DATA[3] = 0x00;
					CAN1_TX_DATA[4] = 0x00;
					crc = CRC16(CAN1_TX_DATA,5);
					CAN1_TX_DATA[5] = crc >> 8;
					CAN1_TX_DATA[6] = crc & 0x00ff;
					can1_tx(SELF_ID,CAN1_TX_DATA);
				}
				else printf("\r\n�������ָ�ȫУ��λ����\r\n");
			break;
			default: 

				if(Arr_ID > 30)
				{
					printf("Arr_IDԽ�� 0 ~ 30");	
					break;
				}
				switch(CAN1_RX_DATA[1]>>7)//��д����λ 0�� 1д
				{
					case 0: 
							printf("\r\n**** �� Arm_Data[%d] = %X **** \r\n", Arr_ID, arr[Arr_ID].cur_valu);
//							CAN1_TX_DATA[0] = Return_ID;
							CAN1_TX_DATA[3] = arr[Arr_ID].cur_valu >> 8;
							CAN1_TX_DATA[4] =arr[Arr_ID].cur_valu & 0x00ff;
							crc = CRC16(CAN1_TX_DATA,5);
							CAN1_TX_DATA[5] = crc >> 8;
							CAN1_TX_DATA[6] = crc & 0x00ff;
							can1_tx(SELF_ID,CAN1_TX_DATA);
						break;
					case 1:
							printf("\r\n**** д Arm_Data[%d] %X **** \r\n", Arr_ID, Two_Byte_Data);
							arr[Arr_ID].cur_valu = Two_Byte_Data;
//							CAN1_TX_DATA[0] = Return_ID;
							crc = CRC16(CAN1_TX_DATA,5);
							CAN1_TX_DATA[5] = crc >> 8;
							CAN1_TX_DATA[6] = crc & 0x00ff;
							can1_tx(SELF_ID,CAN1_TX_DATA);
						break;
					default:
							printf("��������");
						break;
				}
			break;
		}
		
	}
/**************************************************************************************/	
/***************************handle RxMessage.Data******************************************/	
/**************************************************************************************/	
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* ��������ж� */
	
	
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

