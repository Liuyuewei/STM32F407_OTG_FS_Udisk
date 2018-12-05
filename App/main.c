#include"main.h"
//#include"delay.h"
//#include"usart.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
//#include "LowLevel.h"
#include "udisk.h"
#include "flash_if.h"
#include "bsp_nand_flash.h"
#include <stdbool.h>
#include "fat.h"
#include "LCD_RA8875.h"
#include "stm32f4xx_tim.h"
#include "Uart_communication.h"    
#include "stm32f4xx_flash.h"
void MCU_UPDATE_IO_init(void);
typedef  void (*pFunction)(void);
__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;
 NAND_ADDRESS WriteReadAddr;
 pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t flashdestination=APPLICATION_ADDRESS, ramsource;
extern void fat12_init(void);
extern bool fpend_flag;
extern void UART5_Configuration(void);
extern void UART5_NVIC_Config(void);
FILE1* fpp=NULL;
int cout=0,j;
void Timer2_NVIC_Config(void);
void Timer2_Config(void);
bool flag=0,Master_flag=0,Slave_flag=0;
unsigned char test_data[256] = {0};
void hard_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//开机检测引脚：PE2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	             //设置输出类型为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//设置输出速度为100Mhz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					 //设置上/下拉电阻为无上拉下拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);
		//开机检测引脚：PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					 //设置上/下拉电阻为无上拉下拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		//电源控制脚：PE3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	

	//LED控制脚：PE4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	UART5_Configuration();
	UART5_NVIC_Config();		
}
//从机 选择系统存储区作为自举空间
void slave_boot()
{	
	GPIO_SetBits(GPIOF,GPIO_Pin_8);//MCU_BOOT0_H;
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//MCU_BOOT1_L;
	delay_nms(500);
	GPIO_ResetBits(GPIOG,GPIO_Pin_1);//MCU_NRST_L;
	delay_nms(1000);
	GPIO_SetBits(GPIOG,GPIO_Pin_1);;//MCU_NRST_H;		
	delay_nms(500);	
}
void master_slave_communicate()
{
	unsigned int i,result = 0;
	do
	{
		//从机进入自举程序
		send_test_command();
		result = uart5_read();
		delay_nms(100);
	
	}
	while(rcv_mcu_buf[0] != 0x79);
	//获取自举程序版本号和命令
	send_command(S_GETALL);
	result = uart5_read();
	delay_nms(500);
	//获取芯片ID
	send_command(S_GETID);
	result = uart5_read();
	delay_nms(500);

}
int main(void)
{
	unsigned char buf[512]={0};
	unsigned int i = 0 ,result = 0;
	static char counter=0;
	unsigned char msg_string[2]={0};
	hard_init();
	MCU_UPDATE_IO_init();
	
	//检测开机按键按下	LYW 20181203
	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
	{
		for(i=0x4567;i--;);
		for(i=0x4567;i--;);
		while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_3);	//开机状态保持	LYW 20181203
			GPIO_SetBits(GPIOE,GPIO_Pin_4); //开启LED指示灯	LYW 20181203
			if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				break;
			}
		}
	}
	
//	master_slave_communicate();
//	while(1);
	
	
	
//如果等1，则进行简单的闪灯测试程序，否则为正常的boot程序
#if 0	
	//液晶显示驱动初始化
	RA8875_InitHard();
	bmp_dispflash(ADRESS_LOGIN,0,0,LENGTH_LOGIN_X,LENGTH_LOGIN_Y);	
	bmp_dispflash(ADRESS_UNCOMPUTER,704,10,30,18);	
	SERIAL_ROM_Font_mode();	
	Graphic_Mode();//
	while(1)
	{
		delay_nms(500);
		GPIO_SetBits(GPIOE,GPIO_Pin_4); //开启LED指示灯	LYW 20181203
		delay_nms(500);
		GPIO_ResetBits(GPIOE,GPIO_Pin_4); //开启LED指示灯	LYW 20181203
		
	}
#else 
	//开启定时器		1s	LYW 20181203
	Timer2_Config();
	Timer2_NVIC_Config();
//	//从机MCU BOOT0 BOOT1 NRST GPIO初始化
//	MCU_UPDATE_IO_init();	

	//USB_VBUS	USB电源引脚，即插入USB线 LYW 20181203
	if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) != 0))
	{
		for(i = 0;i < 256;i++)
		{
			test_data[i] = i;
		}  
		
		for(i=0x4567;i--;);	//延时
		//USB_VBUS依然为高电平	
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) != 0)
		{
			//液晶显示驱动初始化
			RA8875_InitHard();
			bmp_dispflash(ADRESS_LOGIN,0,0,LENGTH_LOGIN_X,LENGTH_LOGIN_Y);	
			bmp_dispflash(ADRESS_UNCOMPUTER,704,10,30,18);	
			SERIAL_ROM_Font_mode();	
			Graphic_Mode();//

			enable_nandflash_cs();
			close_lcd_cs();
			
			FSMC_NAND_Test();
			WriteReadAddr.Zone = 2000/1024;
			WriteReadAddr.Block = 2000%1024;
			WriteReadAddr.Page = 0;
			
			for(i=0;i<2095;i++)
			{
				 FSMC_NAND_EraseBlock(WriteReadAddr);
				 FSMC_NAND_AddressIncrement(&WriteReadAddr);
			}
		    //USB进行初始化
			USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);

			for(i=0x4567;i--;);
			for(i=0x4567;i--;);
			/* Initialize flashdestination variable */
			flashdestination = APPLICATION_ADDRESS;

			//USB 初始化完成后，一直处于该死循环，此时可以对映射出的磁盘进行操作
			//当程序复制到磁盘后，点击开机键，跳出死循环执行复制操作
			while(1)
			{
				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)
				{
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					//如果PE2依然为低电平，则退出。
					if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)
					{
						break;
					}
				}			
			}
			//把主机程序复制到内部Flash中
			//把从机程序通过串口发给从机
			while(1)
			{
//				close_nandflash_cs();
//				enable_lcd_cs();
//				RA8875_DispStr(550,200,"开始升级......",RA_FONT_16);
//				enable_nandflash_cs();
//				close_lcd_cs();
				
				//初始化文件系统
				
				fat12_init();	
				
				//主机升级程序
				fpp=fat12_openFILE1("LPKJ-000");
				if(fpp!=NULL && !Master_flag)
				{	
					cout=fpp->f_size; 	//读取文件大小								
					flashdestination = APPLICATION_ADDRESS;
					//访问flash前需要进行解锁	LYW　20181203
					FLASH_If_Init();
					//擦除用户区代码	LYW　20181203
					while(FLASH_If_Erase(APPLICATION_ADDRESS));
					//读取文件内容并写入到应用程序区
					//fpp:文件句柄	buf:缓冲区  cout:文件大小
					fat12_read(fpp, buf, cout,0);
					
					FLASH_Lock();
					Master_flag=1;
				}
				//从机升级程序
				fpp=fat12_openFILE1("LPKJ-001");
				if(fpp!=NULL && !Slave_flag)
				{
					slave_boot();
					do
					{
						//从机进入自举程序
						send_test_command();
						result = uart5_read();
						delay_nms(100);
					
					}
					while(rcv_mcu_buf[0] != 0x79);
					//获取自举程序版本号和命令
					send_command(S_GETALL);
					result = uart5_read();
					delay_nms(500);
					//获取芯片ID
					send_command(S_GETID);
					result = uart5_read();
					delay_nms(500);
					cout=fpp->f_size;
					//读取文件内容并通过串口传给从机
					fat12_read(fpp, buf, cout,1);  
					Slave_flag=1;											
				}
				//如果主机和从机任一个升级完成则关机。之后需要人为开机
				if(Master_flag||Slave_flag)
				{
					GPIO_ResetBits(GPIOE,GPIO_Pin_3);
					while(1);
				}
			}
		}
	}
	//拔掉USB升级线
	else
	{
		//复位从机  
		GPIO_ResetBits(GPIOF,GPIO_Pin_8);//MCU_BOOT0_L;
		GPIO_ResetBits(GPIOF,GPIO_Pin_7);//MCU_BOOT1_L;
		GPIO_ResetBits(GPIOG,GPIO_Pin_1);//MCU_NRST_L;
		delay_nms(1000);
		GPIO_SetBits(GPIOG,GPIO_Pin_1);;//MCU_NRST_H;	
		delay_nms(1000);
	}			
	while(1)
	{
		//跳转新地址
		if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{ 
			/* Jump to user application */
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			
			Jump_To_Application = (pFunction) JumpAddress;
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			Jump_To_Application();
		}
	}
#endif
}
 void Timer2_NVIC_Config(void)
 {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	//设置优先级分组
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	//中断线	28
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		  //抢占式
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //副优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 }

 void Timer2_Config(void)
 {
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);		//打开TIM2的时钟为42M

	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period= 4199*2;					   //4199为1s
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

	TIM_Cmd(TIM2,ENABLE);

 }
/***************************************************************************
*函数名：MCU_UPDATE_IO_init()
*输入参数：void
*返回值:void
*功能:初始化从MCU BOOT0  BOOT1引脚电平 初始化为0 需要进行从MCU升级时再进行相应配置
      初始化从MCU NRST引脚电平 初始化为1 BOOT0 BOOT1设置OK后再给低电平一段时间后给高电平
*****************************************************************************/
void MCU_UPDATE_IO_init(void)
{
	int i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//从机BOOT1:PF7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//从机BOOT0:PF8 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//从MCU NRST:PG1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOG,GPIO_Pin_1);//MCU_NRST_H;	
	GPIO_SetBits(GPIOF,GPIO_Pin_8);//MCU_BOOT0_H;
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//MCU_BOOT1_L;
	GPIO_ResetBits(GPIOG,GPIO_Pin_1);//MCU_NRST_L;
	delay_nms(1000);
	GPIO_SetBits(GPIOG,GPIO_Pin_1);;//MCU_NRST_H;	
	delay_nms(1000);
}
//UART5 用于与另一MCU通信 PC12 TXD PD2 RXD
void UART5_Configuration(void)
{ 				
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	//打开串口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

	GPIO_PinAFConfig(GPIOC,  GPIO_PinSource12, GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource2,  GPIO_AF_UART5);

	/*
	*  Open_UART_TX -> PC12 , Open_UART_RX -PD2
	*/
	/*配置UART5的发送管脚TXD(PC12)为复用推挽输出*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;                                       //设置初始化GPIOC为PIN12
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                               //设置GPIO的速度为50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //设置GPIO模式为复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								                //设置GPIO输出类型为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						                    //设置GPIO为连接上拉电阻
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	/*配置UART5的接收管脚RXD(PD2)为复用浮空输入*/                                                                            
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;                                      //设置初始化GPIOD为PIN2
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*配置USART1的接收管脚RXD(PC11)为浮空输入*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //设置GPIO的模式为复用模式
	GPIO_Init(GPIOD,&GPIO_InitStructure);                                         //初始化GPIOD的PIN2

/*
   		 USARTx configured as follow:
         - BaudRate = 115200 baud  
		     - Word Length = 8 Bits
         - One Stop Bit
         - No parity
         - Hardware flow control disabled (RTS and CTS signals)
         - Receive and transmit    
 */

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//校验方式：
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	//开启串口接收中断
	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	//开启发送中断位
	//USART_ITConfig(Open_USART,USART_IT_TXE,ENABLE);

	USART_Cmd(UART5, ENABLE);
}

void UART5_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

