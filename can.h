#ifndef  _CAN_H_
#define  _CAN_H_
#include "stm32f10x_can.h"

#define CAN_BAUD_NUM 20  //波特率个数


//extern unsigned char can1_rec_flag;
//extern unsigned char CAN1_DATA[8];

void can1_tx(unsigned int ID,unsigned char* can_send_data);
void CAN_Configuration(void);
void CAN_Baud_Process(unsigned int Baud,CAN_InitTypeDef *CAN_InitStructure);


void CAN_NVIC_Configuration(void);


#endif
