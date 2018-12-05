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
	//����������ţ�PE2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	             //�����������Ϊ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//��������ٶ�Ϊ100Mhz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					 //������/��������Ϊ��������������
	GPIO_Init(GPIOE, &GPIO_InitStructure);
		//����������ţ�PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					 //������/��������Ϊ��������������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		//��Դ���ƽţ�PE3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	

	//LED���ƽţ�PE4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	UART5_Configuration();
	UART5_NVIC_Config();		
}
//�ӻ� ѡ��ϵͳ�洢����Ϊ�Ծٿռ�
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
		//�ӻ������Ծٳ���
		send_test_command();
		result = uart5_read();
		delay_nms(100);
	
	}
	while(rcv_mcu_buf[0] != 0x79);
	//��ȡ�Ծٳ���汾�ź�����
	send_command(S_GETALL);
	result = uart5_read();
	delay_nms(500);
	//��ȡоƬID
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
	
	//��⿪����������	LYW 20181203
	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
	{
		for(i=0x4567;i--;);
		for(i=0x4567;i--;);
		while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_3);	//����״̬����	LYW 20181203
			GPIO_SetBits(GPIOE,GPIO_Pin_4); //����LEDָʾ��	LYW 20181203
			if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				break;
			}
		}
	}
	
//	master_slave_communicate();
//	while(1);
	
	
	
//�����1������м򵥵����Ʋ��Գ��򣬷���Ϊ������boot����
#if 0	
	//Һ����ʾ������ʼ��
	RA8875_InitHard();
	bmp_dispflash(ADRESS_LOGIN,0,0,LENGTH_LOGIN_X,LENGTH_LOGIN_Y);	
	bmp_dispflash(ADRESS_UNCOMPUTER,704,10,30,18);	
	SERIAL_ROM_Font_mode();	
	Graphic_Mode();//
	while(1)
	{
		delay_nms(500);
		GPIO_SetBits(GPIOE,GPIO_Pin_4); //����LEDָʾ��	LYW 20181203
		delay_nms(500);
		GPIO_ResetBits(GPIOE,GPIO_Pin_4); //����LEDָʾ��	LYW 20181203
		
	}
#else 
	//������ʱ��		1s	LYW 20181203
	Timer2_Config();
	Timer2_NVIC_Config();
//	//�ӻ�MCU BOOT0 BOOT1 NRST GPIO��ʼ��
//	MCU_UPDATE_IO_init();	

	//USB_VBUS	USB��Դ���ţ�������USB�� LYW 20181203
	if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) != 0))
	{
		for(i = 0;i < 256;i++)
		{
			test_data[i] = i;
		}  
		
		for(i=0x4567;i--;);	//��ʱ
		//USB_VBUS��ȻΪ�ߵ�ƽ	
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) != 0)
		{
			//Һ����ʾ������ʼ��
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
		    //USB���г�ʼ��
			USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);

			for(i=0x4567;i--;);
			for(i=0x4567;i--;);
			/* Initialize flashdestination variable */
			flashdestination = APPLICATION_ADDRESS;

			//USB ��ʼ����ɺ�һֱ���ڸ���ѭ������ʱ���Զ�ӳ����Ĵ��̽��в���
			//�������Ƶ����̺󣬵����������������ѭ��ִ�и��Ʋ���
			while(1)
			{
				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)
				{
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					for(i=0x4567;i--;);
					//���PE2��ȻΪ�͵�ƽ�����˳���
					if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)
					{
						break;
					}
				}			
			}
			//�����������Ƶ��ڲ�Flash��
			//�Ѵӻ�����ͨ�����ڷ����ӻ�
			while(1)
			{
//				close_nandflash_cs();
//				enable_lcd_cs();
//				RA8875_DispStr(550,200,"��ʼ����......",RA_FONT_16);
//				enable_nandflash_cs();
//				close_lcd_cs();
				
				//��ʼ���ļ�ϵͳ
				
				fat12_init();	
				
				//������������
				fpp=fat12_openFILE1("LPKJ-000");
				if(fpp!=NULL && !Master_flag)
				{	
					cout=fpp->f_size; 	//��ȡ�ļ���С								
					flashdestination = APPLICATION_ADDRESS;
					//����flashǰ��Ҫ���н���	LYW��20181203
					FLASH_If_Init();
					//�����û�������	LYW��20181203
					while(FLASH_If_Erase(APPLICATION_ADDRESS));
					//��ȡ�ļ����ݲ�д�뵽Ӧ�ó�����
					//fpp:�ļ����	buf:������  cout:�ļ���С
					fat12_read(fpp, buf, cout,0);
					
					FLASH_Lock();
					Master_flag=1;
				}
				//�ӻ���������
				fpp=fat12_openFILE1("LPKJ-001");
				if(fpp!=NULL && !Slave_flag)
				{
					slave_boot();
					do
					{
						//�ӻ������Ծٳ���
						send_test_command();
						result = uart5_read();
						delay_nms(100);
					
					}
					while(rcv_mcu_buf[0] != 0x79);
					//��ȡ�Ծٳ���汾�ź�����
					send_command(S_GETALL);
					result = uart5_read();
					delay_nms(500);
					//��ȡоƬID
					send_command(S_GETID);
					result = uart5_read();
					delay_nms(500);
					cout=fpp->f_size;
					//��ȡ�ļ����ݲ�ͨ�����ڴ����ӻ�
					fat12_read(fpp, buf, cout,1);  
					Slave_flag=1;											
				}
				//��������ʹӻ���һ�����������ػ���֮����Ҫ��Ϊ����
				if(Master_flag||Slave_flag)
				{
					GPIO_ResetBits(GPIOE,GPIO_Pin_3);
					while(1);
				}
			}
		}
	}
	//�ε�USB������
	else
	{
		//��λ�ӻ�  
		GPIO_ResetBits(GPIOF,GPIO_Pin_8);//MCU_BOOT0_L;
		GPIO_ResetBits(GPIOF,GPIO_Pin_7);//MCU_BOOT1_L;
		GPIO_ResetBits(GPIOG,GPIO_Pin_1);//MCU_NRST_L;
		delay_nms(1000);
		GPIO_SetBits(GPIOG,GPIO_Pin_1);;//MCU_NRST_H;	
		delay_nms(1000);
	}			
	while(1)
	{
		//��ת�µ�ַ
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	//�������ȼ�����
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	//�ж���	28
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		  //��ռʽ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 }

 void Timer2_Config(void)
 {
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);		//��TIM2��ʱ��Ϊ42M

	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period= 4199*2;					   //4199Ϊ1s
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

	TIM_Cmd(TIM2,ENABLE);

 }
/***************************************************************************
*��������MCU_UPDATE_IO_init()
*���������void
*����ֵ:void
*����:��ʼ����MCU BOOT0  BOOT1���ŵ�ƽ ��ʼ��Ϊ0 ��Ҫ���д�MCU����ʱ�ٽ�����Ӧ����
      ��ʼ����MCU NRST���ŵ�ƽ ��ʼ��Ϊ1 BOOT0 BOOT1����OK���ٸ��͵�ƽһ��ʱ�����ߵ�ƽ
*****************************************************************************/
void MCU_UPDATE_IO_init(void)
{
	int i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//�ӻ�BOOT1:PF7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//�ӻ�BOOT0:PF8 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//��MCU NRST:PG1
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
//UART5 ��������һMCUͨ�� PC12 TXD PD2 RXD
void UART5_Configuration(void)
{ 				
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	//�򿪴���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

	GPIO_PinAFConfig(GPIOC,  GPIO_PinSource12, GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource2,  GPIO_AF_UART5);

	/*
	*  Open_UART_TX -> PC12 , Open_UART_RX -PD2
	*/
	/*����UART5�ķ��͹ܽ�TXD(PC12)Ϊ�����������*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;                                       //���ó�ʼ��GPIOCΪPIN12
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                               //����GPIO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //����GPIOģʽΪ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								                //����GPIO�������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						                    //����GPIOΪ������������
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	/*����UART5�Ľ��չܽ�RXD(PD2)Ϊ���ø�������*/                                                                            
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;                                      //���ó�ʼ��GPIODΪPIN2
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*����USART1�Ľ��չܽ�RXD(PC11)Ϊ��������*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //����GPIO��ģʽΪ����ģʽ
	GPIO_Init(GPIOD,&GPIO_InitStructure);                                         //��ʼ��GPIOD��PIN2

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
	//У�鷽ʽ��
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	//�������ڽ����ж�
	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	//���������ж�λ
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

