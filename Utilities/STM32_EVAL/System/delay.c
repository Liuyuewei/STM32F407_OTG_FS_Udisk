#include "delay.h"

static uint16_t  fac_us=0;//us��ʱ������
static uint32_t fac_ms=0;//ms��ʱ������
//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void DELAY_Init()	 
{ 
  SysTick->CTRL  = 0<<2;                    
  SystemCoreClockUpdate();                     //��ȡ��ǰϵͳ��ʱ��Ƶ��       
 	fac_us =SystemCoreClock/8000000;						 //Ϊϵͳʱ�ӵ�1/8         
  fac_ms = fac_us * 1000;                      //1ms�������������

}								    //   1/100

void delay_us(uint32_t nus)
{        
     uint32_t temp;
     SysTick -> LOAD = nus*fac_us;                      //ʱ�����(SysTick->LOADΪ24bit)
     SysTick -> VAL = 0x00;                   //��ռ�����
     SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����      
			 do
			{
					temp = SysTick -> CTRL;              //��ȡsystick��ʱ������״̬�Ĵ���
			 }
			 while ((temp & 0x01) && (!(temp & (1<<16))));//�ȴ�ʱ�䵽��        
     SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����        
     SysTick->VAL   =  (0x00);                //��ռ�����
}


void delay_ms(uint16_t nms)
{	 		  	  
		uint32_t temp;		   
		SysTick->LOAD=fac_ms;//ʱ�����1ms
		SysTick->VAL =0x00;           //��ռ�����
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
		for(;nms>0;nms--)
		{		
				do
				{
					 temp = SysTick -> CTRL;              //��ȡsystick��ʱ������״̬�Ĵ���
				}
				while ((temp & 0x01) && (!(temp & (1<<16))));//�ȴ�ʱ�䵽��  
		}

		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
		SysTick->VAL =0X00;       //��ռ�����	  	    
} 


