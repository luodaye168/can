#include "systick.h"

static unsigned char fac_us  = 0;	//us延时倍乘数
static unsigned short fac_ms = 0;	//ms延时倍乘数
/*********************************************************************************************************
* 函 数 名 : Systick_Inti
* 功能说明 : 初始化系统滴答
* 形    参 : clk：系统运行频率
* 返 回 值 : 无
* 备    注 : 外部时钟源 = 系统运行频率 / 8；
*********************************************************************************************************/ 
//void Systick_Inti(unsigned char clk)
//{
//	SysTick->CTRL &= ~(1<<2);	//选择外部时钟作为滴答的时钟源
//	fac_us = clk / 8.0;			//得到计1us需要计的数
//	fac_ms = clk / 8.0 * 1000;	//得到计1ms需要计的数
//}
void delay_init()
{
#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
	reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为K	   
	reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/delay_ostickspersec;			//代表OS可以延时的最少单位	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

#else
	fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数   
#endif
}				
/*********************************************************************************************************
* 函 数 名 : Delay_Us
* 功能说明 : us级延迟
* 形    参 : us：需要延迟的us数
* 返 回 值 : 无
* 备    注 : 最大延迟时间 = （2^24-1）/ 10.5 = 798915(us)，注意：尽量不要在中断中调用
*********************************************************************************************************/ 
void Delay_Us(unsigned int us)
{
	unsigned int temp = 0;
	SysTick->LOAD = us*fac_us;				//计10.5个数是1us
	SysTick->VAL  = 0;						//清除当前计数值
	SysTick->CTRL |= (1<<0);				//使能计数器
	do{										//必须要先读出寄存器的值再判断，否则会产生误差									
		temp = SysTick->CTRL;				//等待第16位为1
	}while(!(temp&(1<<16))&&(temp&(1<<0)));	//并且确定定时器还在运行
	SysTick->CTRL &= ~(1<<0);				//关闭计数器
}
/*********************************************************************************************************
* 函 数 名 : Delay_Ms
* 功能说明 : ms级延迟
* 形    参 : ms：要延迟的ms数
* 返 回 值 : 无
* 备    注 : 最大延迟 = （2^24-1）/ 21 = 798(ms)
*********************************************************************************************************/ 
void Delay_N_Ms(unsigned int Ms)
{
	unsigned int temp = 0;
	SysTick->LOAD = Ms*fac_ms;					//计10.5个数是1us，1ms有1000us
	SysTick->VAL  = 0;							//清除当前计数值
	SysTick->CTRL |= (1<<0);					//使能计数器
	do{											//必须要先读出寄存器的值再判断，否则会产生误差
		temp = SysTick->CTRL;					//等待第16位为1
	}while(!(temp&(1<<16))&&(temp&(1<<0)));		//并且确定定时器还在运行
	SysTick->CTRL &= ~(1<<0);					//关闭计数器
}


void Delay_Ms(unsigned int Ms)  //延时时间无限制
{
		int i;
		int bs=Ms/500;   //求倍数
		int ys=Ms%500;   //求余数
	
		for(i=0;i<bs;i++)  Delay_N_Ms(500);
		
		if(ys!=0) Delay_N_Ms(ys);
	
}

