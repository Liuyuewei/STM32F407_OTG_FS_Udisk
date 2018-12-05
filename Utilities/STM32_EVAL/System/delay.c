#include "delay.h"

static uint16_t  fac_us=0;//us延时倍乘数
static uint32_t fac_ms=0;//ms延时倍乘数
//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void DELAY_Init()	 
{ 
  SysTick->CTRL  = 0<<2;                    
  SystemCoreClockUpdate();                     //获取当前系统的时钟频率       
 	fac_us =SystemCoreClock/8000000;						 //为系统时钟的1/8         
  fac_ms = fac_us * 1000;                      //1ms需计算的脉冲个数

}								    //   1/100

void delay_us(uint32_t nus)
{        
     uint32_t temp;
     SysTick -> LOAD = nus*fac_us;                      //时间加载(SysTick->LOAD为24bit)
     SysTick -> VAL = 0x00;                   //清空计数器
     SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数      
			 do
			{
					temp = SysTick -> CTRL;              //读取systick定时器控制状态寄存器
			 }
			 while ((temp & 0x01) && (!(temp & (1<<16))));//等待时间到达        
     SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器        
     SysTick->VAL   =  (0x00);                //清空计数器
}


void delay_ms(uint16_t nms)
{	 		  	  
		uint32_t temp;		   
		SysTick->LOAD=fac_ms;//时间加载1ms
		SysTick->VAL =0x00;           //清空计数器
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数 
		for(;nms>0;nms--)
		{		
				do
				{
					 temp = SysTick -> CTRL;              //读取systick定时器控制状态寄存器
				}
				while ((temp & 0x01) && (!(temp & (1<<16))));//等待时间到达  
		}

		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
		SysTick->VAL =0X00;       //清空计数器	  	    
} 


