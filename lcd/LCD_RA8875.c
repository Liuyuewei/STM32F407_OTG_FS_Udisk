/*
*********************************************************************************************************
*
*	ģ������ : RA8875оƬ����ģ��
*	�ļ����� : LCD_RA8875.c
*	��    �� : V1.1
*	˵    �� : RA8875�ײ�������������
*	�޸ļ�¼ :
*		�汾��  ����   
*		V1.0    2012-08-07 
*
*
*********************************************************************************************************
*/
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "fonts.h"
#include "LCD_RA8875.h"
#include "bsp_tft_lcd.h"
#include "stm32f4xx_fsmc.h"







static __IO uint16_t s_usTouchX, s_usTouchY;	/* ������ADCֵ */
__IO uint8_t s_ucRA8875Busy = 0;

/* ���浱ǰ��ʾ���ڵ�λ�úʹ�С���⼸�������� RA8875_SetDispWin() �������� */
static __IO uint16_t s_WinX = 0;
static __IO uint16_t s_WinY = 0;
static __IO uint16_t s_WinHeight = 272;
static __IO uint16_t s_WinWidth = 480;

 



static void BTE_SetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer);
static void BTE_SetOperateCode(uint8_t _ucOperate);

static void RA8875_Delaly1us(void)
{
	uint16_t i;

	for (i = 0; i < 1670; i++);	/* �ӳ�, ��׼ */
}

static void RA8875_Delaly1ms(void)
{
	uint16_t i;

	for (i = 0; i < 16700; i++);	/* �ӳ�1us, ��׼ */
}


/*******************Һ����Ӳ������**********************

PD14 -----FSMC_D0  ----D0
PD15 -----FSMC_D1  ----D1
PD0   -----FSMC_D2  ----D2
PD1   -----FSMC_D3  ----D3
PE7    -----FSMC_D4  ---D4
PE8    -----FSMC_D5  ---D5
PE9    -----FSMC_D6  ---D6
PE10  -----FSMC_D7   ----D7
PE11  -----FSMC_D8   ----D8
PE12  -----FSMC_D9   ----D9
PE13  -----FSMC_D10   ----D10
PE14  -----FSMC_D11   ----D11
PE15  -----FSMC_D12   ----D12
PD8   -----FSMC_D13   ----D13
PD9   -----FSMC_D14   ----D14
PD10 -----FSMC_D15   ----D15
PD4   -----FSMC_NOE -----RD
PD5   -----FSMC_NWE ----WR
PG10    -----FSMC_NE3  ----CS
PF0 -----FSMC_A0   ----RS
***************************************************************/



// void LCD_Reset(void)
// {
// 	GPIO_InitTypeDef  GPIO_InitStructure;
// 	uint32_t i;
// 	
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//     GPIO_Init(GPIOH, &GPIO_InitStructure);	 
// 	GPIOH->BSRRH = GPIO_Pin_2;	
// 	for(i = 0; i < 1000000; i++)
// 	GPIOH->BSRRL = GPIO_Pin_2;	
// }
// void LCD_Mode8080(void)
// {
//   GPIO_InitTypeDef  GPIO_InitStructure;

//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//   GPIO_Init(GPIOI, &GPIO_InitStructure);
// 	GPIOI->BSRRH = GPIO_Pin_8;
// 	
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//   GPIO_Init(GPIOE, &GPIO_InitStructure);	
//   GPIOE->BSRRH = GPIO_Pin_6;			 // 8080
// }


/**************************************************************************************************************
 * ��������LCD_GPIO_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��GPIO�ܽ���FSMC����
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
	
//     GPIO_InitTypeDef GPIO_InitStructure;		                                              //����GPIO��ʼ���ṹ��
//      
//     /* ʹ��FSMC��AHB3ʱ�ӣ�����ע�⣬��ͬоƬ���߻᲻ͬ */
//     RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
//     
//     /* ʹ��GPIOB��GPIOD��GPIOE��AHB1ʱ��*/
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
//                          RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH , ENABLE);
//    	 /*����Һ�����ı�����ƹܽ�*/
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz��M4���ǿ찡
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //�����������Ϊ�������
// 	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //������/��������Ϊ��������������		

//     /* ���û���FSMC��Һ������������
// 	 * Һ���������ߣ�FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
// 	 */	
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //���ùܽ�ģʽΪ����ģʽ
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | 
//                                   GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
//     GPIO_Init(GPIOD, &GPIO_InitStructure);													  //��ʼ��GPIOB��Pin_0��Pin_��Pin_2��
// 	                                                                    
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
//                                   GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
//                                   GPIO_Pin_15;
//     GPIO_Init(GPIOE, &GPIO_InitStructure); 
//     
//     /* ���û���FSMC��Һ����������
// 	 * PD4-FSMC_NOE  :LCD-RD //PD4
//    * PD5-FSMC_NWE  :LCD-WR //PD5
// 	 * PG10-FSMC_NE3  :LCD-CS //PD7
//    * PF0-FSMC_A0 :LCD-DC    //PD11
// 	 */
// 	 //csf1.11
// //     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
// //     GPIO_Init(GPIOD, &GPIO_InitStructure);
// //     
// //     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
// //     GPIO_Init(GPIOD, &GPIO_InitStructure);
// //     
// //     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
// //     GPIO_Init(GPIOF, &GPIO_InitStructure);  
// // 		
// //     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; 
// //     GPIO_Init(GPIOG, &GPIO_InitStructure); 
//    
//     GPIO_AF_FSMC_Config();				             //��F407�б��������������������ã����������ӵ�FSMC
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
	
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 

	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                   +

	+-------------------+--------------------+
	*/
	/* GPIOD configuration */
		/* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.11(A16), PD.14(D0), PD.15(D1) as alternate function push pull */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC); //PD7���ó�FSMC�� csf
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */	
	/* GPIOE configuration */
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
	                             GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
				
}

/**************************************************************************************************************
 * ��������LCD_FSMC_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��FSMC
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	                                              //����GPIO��ʼ���ṹ��
	  
  	FSMC_NORSRAMTimingInitTypeDef  p;                                                 
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 2;                                           //��ַ����ʱ��
  	p.FSMC_AddressHoldTime = 0;                                            //��ַ����ʱ��
  	p.FSMC_DataSetupTime =1;                                              //���ݽ���ʱ��
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
		
		
}
/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteCmd
*	����˵��: дRA8875ָ��Ĵ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_WriteCmd(uint8_t _ucRegAddr)
{
	RA8875_REG = _ucRegAddr;	/* ���üĴ�����ַ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteData
*	����˵��: дRA8875ָ��Ĵ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_WriteData(uint8_t _ucRegValue)
{
	RA8875_RAM = _ucRegValue;	/* ���üĴ�����ַ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_ReadData
*	����˵��: ��RA8875�Ĵ���ֵ
*	��    ��: ��
*	�� �� ֵ: �Ĵ���ֵ
*********************************************************************************************************
*/
uint8_t RA8875_ReadData(uint8_t _ucRegAddr)
{
	uint8_t value;
	value = RA8875_RAM;		/* ��ȡ�Ĵ���ֵ */
	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteData16
*	����˵��: дRA8875�������ߣ�16bit������RGB�Դ�д��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_WriteData16(uint16_t _usRGB)
{
	RA8875_RAM = _usRGB;	/* ���üĴ�����ַ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_ReadData16
*	����˵��: ��RA8875�Դ棬16bit RGB
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t RA8875_ReadData16(void)
{
	uint16_t value;
	value = RA8875_RAM;		/* ��ȡ�Ĵ���ֵ */
	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteReg
*	����˵��: дRA8875�Ĵ���. RA8875�ļĴ�����ַ�����ݶ���8bit��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_WriteReg(uint8_t _ucRegAddr, uint8_t _ucRegValue)
{
	s_ucRA8875Busy = 1;
	RA8875_REG = _ucRegAddr;	/* ���üĴ�����ַ */
	RA8875_RAM = _ucRegValue;	/* д��Ĵ���ֵ */
	s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteReg
*	����˵��: дRA8875�Ĵ���. RA8875�ļĴ�����ַ�����ݶ���8bit��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 uint8_t RA8875_ReadReg(uint8_t _ucRegAddr)
{
	uint8_t value;
	RA8875_REG = _ucRegAddr;/* ���üĴ�����ַ */
	value = RA8875_RAM;		/* ��ȡ�Ĵ���ֵ */

	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_ReadStatus
*	����˵��: ��RA8875״̬�Ĵ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// uint8_t RA8875_ReadStatus(void)
// {
// 	uint8_t value;
// 	value = RA8875_REG;
// 	return value;

// }


u8 LCD_StatusRead(void)
{
	u16 Data;
	Data = ReadCOMM();
	return Data;
}
/*
*********************************************************************************************************
*	�� �� ��: RA8875_ReadID
*	����˵��: ��ȡLCD����оƬID
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

// void PWM1_enable(void)
// {
// 	u8 temp;
// //	WriteCommand(0x8a);//MCLR
// //	temp = ReadData();
// 	temp=RA8875_ReadReg(0x8a);
// 	temp |= 0x80 ;
// 	WriteData(temp);
// //	RA8875_WAITSTATUS();
//  }

//RA8875�ȴ�����
void RA8875_WAITSTATUS(void)
{
	u8 temp;
	do
	{
		temp=LCD_StatusRead();
	}while((temp&0x80)==0x80);

}


uint16_t RA8875_ReadID(void)
{
// 	uint8_t value;
// 	RA8875_REG = _ucRegAddr;/* ���üĴ�����ַ */
// 	value = RA8875_RAM;		/* ��ȡ�Ĵ���ֵ */
		return RA8875_ReadReg(0x00);
	
}



// void clear_TP_interrupt(void)		//WRITE 1 TO 0XF1 BIT 2
// {
// // 	u8 temp;
// // 	RA8875_WAITSTATUS();
// // 	RA8875_WriteCmd(0xF1);//INTC
// // 	temp=RA8875_ReadStatus(); 
// // 	temp |= 0x04;
// // 	RA8875_WriteCmd(temp);
// // 	RA8875_WAITSTATUS();
// 	u8 temp;
// 	RA8875_WAITSTATUS();
// // 	WriteCommand(0xF1);//INTC
// // 	temp=ReadData(); 
// 	temp=RA8875_ReadReg(0xF1);
// 	temp |= 0x04;
// 	WriteData(temp);
// 	RA8875_WAITSTATUS();
//  }
// //ʹ�ܴ������ж�
// void Touch_Panel_Interrupt_Enable(void)
// {
// // 	u8 temp;
// // 	RA8875_WriteCmd(0xF0);//INTC
// // 	temp = RA8875_ReadStatus();
// // 	temp |= 0x04;		 //0000 0100 �����������ж�
// // 	RA8875_WriteCmd(temp);
// // 	RA8875_WAITSTATUS();
// 		u8 temp;
// 	WriteCommand(0xF0);//INTC
// 	temp = ReadData();
// 	temp |= 0x04;		 //0000 0100 �����������ж�
// 	WriteData(temp);
// 	RA8875_WAITSTATUS();
// }



// void Memory_Clear(void)
// {
// 	u8 temp;
// 	WriteCommand(0x8e);//MCLR
// 	temp = ReadData();
// 	temp |= 0x80 ;
// 	WriteData(temp); 
// 	RA8875_WAITSTATUS();
// }
// //��������ʼ��

// void TOUCH_InitHard()
// {

// // 	RA8875_WriteReg(0x70, 0xB4);//ADCƵ��Ϊ  100b : ϵͳƵ�� / 16
// // 	RA8875_WAITSTATUS();
// // 	RA8875_WriteReg(0x71, 0x00);//0 : �Զ�ģʽ��0 : �ڲ������ο���ѹ��
// // 	RA8875_WAITSTATUS();
// // 	Touch_Panel_Interrupt_Enable();
// 		WriteCommand(0x71);
// 		WriteData(0x00);						     
// 		RA8875_WAITSTATUS();
// 		//TP_auto_mode();			//enable auto mode
// 		/*
// 			u8 temp;
//   			WriteCommand(0x71);//TPCR1	
// 			temp = ReadData();
// 			temp &= cClrD6;//temp |= 0x3F;	AUTO MODE
// 			
// 			//temp |= 0x3F;               //�ⲿ��׼��0x1f�ڲ���׼
// 			WriteData(temp);
// 			RA8875_WAITSTATUS();

// 		TPCR1 71H
// 		7   N/A     
// 		6   0�Զ�   1�ֶ�   
// 		5   0�ڲο� 1��ο�
// 		43  N/A
// 		2   0������ 1������ 
// 		10  �ֶ�ģʽѡ��λ 00���� 01���		         
// 		*/


// 		WriteCommand(0x70);		//set TP sample time  & ADC clock 	TPCR0
// 		WriteData(0xb3);//0xb3 0xa2		����1 AD����4096������011 �رմ����¼�����ģʽ0 ������������ADCƵ���趨ϵͳCLK/8 011
// 		//1                    1 0 1                0                            011
// 		//bit7 ����1 
// 		//             AD����4096������011 
// 		//                                   �رմ����¼�����ģʽ0 
// 		//                                                              ������������ADCƵ���趨ϵͳCLK/8 011
// 		RA8875_WAITSTATUS();
// 		//Enable_TP(); // I add it 		not need	

// 		Touch_Panel_Interrupt_Enable();	// demo code using Auto mode + polling 8Fh to confirm touch
// 		clear_TP_interrupt();

// 		//Active_Window(100,400,100,400);
// //		Active_Window(0,479,0,271);	//�趨���ֱ���Ϊ480*272 pixel
// 		//HDIR_SEGn_SEG0();		//����Ļ��ʾ�ߵ�90��
// 	//	Clear_Active_Window();
// //		Text_Foreground_Color(GRAY0);
// //		Text_Background_Color(RED);
// 		
// 		//Font_with_BackgroundTransparency();
// 		Memory_Clear();
// 	//	Display_ON();	

// }

	unsigned char temp_ID;
/*
*********************************************************************************************************
*	�� �� ��: RA8875_InitHard
*	����˵��: ��ʼ��RA8875����оƬ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_InitHard(void)
{

  LCD_GPIO_Config();		
	LCD_FSMC_Config();
	RA8875_Delaly1ms();
	RA8875_Delaly1ms();
	temp_ID=RA8875_ReadID();

	
	/* ��ʼ��PLL.  ����Ƶ��Ϊ25M */
	RA8875_WriteCmd(0x88);
	RA8875_Delaly1us();		/* �ӳ�1us */
	RA8875_WriteData(11);	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 12 */
	RA8875_Delaly1ms();
	RA8875_WriteCmd(0x89);
	RA8875_Delaly1us();		/* �ӳ�1us */
	RA8875_WriteData(2);//RA8875_WriteData(3);//RA8875_WriteData(3);	/* PLLDIVK[2:0] = 2, ����4 */
		/* RA8875 ���ڲ�ϵͳƵ�� (SYS_CLK) �ǽ���񵴵�·��PLL ��·��������Ƶ�ʼ��㹫ʽ���� :
		  SYS_CLK = FIN * ( PLLDIVN [4:0] +1 ) / ((PLLDIVM+1 ) * ( 2^PLLDIVK [2:0] ))
		          = 25M * (11 + 1) / ((0 + 1) * (2 ^ 2))
				  = 75MHz
		*/
		/* REG[88h]��REG[89h]���趨��Ϊ��֤PLL ����ȶ�����ȴ�һ�Ρ���Ƶʱ�䡹(< 100us)��*/
	RA8875_Delaly1ms();
	
//	PWM1_enable();
	
//	RA8875_SetBackLight(1);//�������ó�������� csf
//RA8875_SetBackLight(1);//����
RA8875_SetBackLight(128);//����

	
		/*
			����ϵͳ���ƼĴ����� ����pdf ��18ҳ:

			bit3:2 ɫ������趨 (Color Depth Setting)
				00b : 8-bpp ��ͨ��TFT �ӿڣ� i.e. 256 ɫ��
				1xb : 16-bpp ��ͨ��TFT �ӿڣ� i.e. 65K ɫ��	 ��ѡ�����

			bit1:0 MCUIF ѡ��
				00b : 8-λMCU �ӿڡ�
				1xb : 16-λMCU �ӿڡ� ��ѡ�����
		*/
	RA8875_WriteReg(0x10, (1 <<3 ) | (1 << 1));	/* ����16λMCU���ڣ�65Kɫ */
		/* REG[04h] Pixel Clock Setting Register (PCSR)
			bit7  PCLK Inversion
				0 : PDAT ����PCLK ��Ե���� (Rising Edge) ʱ��ȡ����
				1 : PDAT ����PCLK ��Ե�½� (Falling Edge) ʱ��ȡ����
			bit1:0 PCLK Ƶ�������趨
				Pixel Clock ,PCLK Ƶ�������趨��
				00b: PCLK Ƶ������= ϵͳƵ�����ڡ�
				01b: PCLK Ƶ������= 2 ����ϵͳƵ�����ڡ�
				10b: PCLK Ƶ������= 4 ����ϵͳƵ�����ڡ�
				11b: PCLK Ƶ������= 8 ����ϵͳƵ�����ڡ�
		*/
	RA8875_WriteReg(0x04, 0x80); /* ����PCLK���� */
	RA8875_Delaly1ms();
	
// 	RA8875_WriteReg(0xC0, 0x80);
// 	RA8875_WriteReg(0x13, 0x00);
	
	
	RA8875_WriteReg(0x14, 0x63);
	RA8875_WriteReg(0x15, 0x00);
	RA8875_WriteReg(0x16, 0x03);
	RA8875_WriteReg(0x17, 0x03);
	RA8875_WriteReg(0x18, 0x0B);
	RA8875_WriteReg(0x19, 0xDF);
	RA8875_WriteReg(0x1A, 0x01);
	RA8875_WriteReg(0x1B, 0x1F);
	RA8875_WriteReg(0x1C, 0x00);
	RA8875_WriteReg(0x1D, 0x16);
	RA8875_WriteReg(0x1E, 0x00);
	RA8875_WriteReg(0x1F, 0x01);

	RA8875_WriteReg(0x20, 0x08);  //add by zl 2013.1.17	0x04


	  /* LCD ��ʾ/�ر�Ѷ�� (LCD Display on) */
	RA8875_WriteReg(0x01, 0x80);

	/* 	REG[40h] Memory Write Control Register 0 (MWCR0)

		Bit 7	��ʾģʽ�趨
			0 : ��ͼģʽ��
			1 : ����ģʽ��

		Bit 6	����д����/�ڴ�д�����趨
			0 : �趨����/�ڴ�д����Ϊ����ʾ��
			1 : �趨����/�ڴ�д����Ϊ��ʾ��

		Bit 5	����д����/�ڴ�д������˸�趨
			0 : �α겻��˸��
			1 : �α���˸��

		Bit 4   NA

		Bit 3-2  ��ͼģʽʱ���ڴ�д�뷽��
			00b : �� -> �ң�Ȼ���� -> �¡�
			01b : �� -> ��Ȼ���� -> �¡�
			10b : �� -> �£�Ȼ���� -> �ҡ�
			11b : �� -> �ϣ�Ȼ���� -> �ҡ�

		Bit 1 	�ڴ�д�����Զ����ӹ����趨
			0 : ���ڴ�д��ʱ���λ���Զ���һ��
			1 : ���ڴ�д��ʱ���λ�ò����Զ���һ��

		Bit 0 �ڴ��ȡ����Զ����ӹ����趨
			0 : ���ڴ��ȡʱ���λ���Զ���һ��
			1 : ���ڴ��ȡʱ���λ�ò����Զ���һ��
	*/
	RA8875_WriteReg(0x40, 0x00);	/* ѡ���ͼģʽ */
	/* 	REG[41h] Memory Write Control Register1 (MWCR1)
		д��Ŀ��λ�ã�ѡ��ͼ��1
	*/
	RA8875_WriteReg(0x41, 0x00);	/* ѡ���ͼģʽ, Ŀ��ΪCGRAM */

	RA8875_SetDispWin(0, 0, 480, 800);
	
	RA8875_DispOn();
	
	//��������ʼ��
	//TOUCH_InitHard();

}


// //��ȡ�������ж�
// u8 RA8875_ReadTP(void)
// {

// 	return RA8875_ReadReg(0xF1);

// }
/*
*********************************************************************************************************
*	�� �� ��: BTE_SetTarBlock
*	����˵��: ����RA8875 BTEĿ�������Լ�Ŀ��ͼ��
*	��    ��:
*			uint16_t _usX : ˮƽ�������
*			uint16_t _usY : ��ֱ�������
*			uint16_t _usHeight : ����߶�
*			uint16_t _usWidth : ������
*			uint8_t _ucLayer �� 0 ͼ��1�� 1 ͼ��2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// static void BTE_SetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer)
// {
// 	/* ����������� */
// 	RA8875_WriteReg(0x58, _usX);
// 	RA8875_WriteReg(0x59, _usX >> 8);

// 	RA8875_WriteReg(0x5A, _usY);
// 	if (_ucLayer == 0)	/* ͼ��2 */
// 	{
// 		RA8875_WriteReg(0x5B, _usY >> 8);
// 	}
// 	else
// 	{
// 		RA8875_WriteReg(0x5B, (1 << 7) | (_usY >> 8));	/* Bit7 ��ʾͼ�㣬 0 ͼ��1�� 1 ͼ��2*/
// 	}

// 	/* ���������� */
// 	RA8875_WriteReg(0x5C, _usWidth);
// 	RA8875_WriteReg(0x5D, _usWidth >> 8);

// 	/* ��������߶� */
// 	RA8875_WriteReg(0x5E, _usHeight);
// 	RA8875_WriteReg(0x5F, _usHeight >> 8);
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: BTE_SetOperateCode
// *	����˵��: �趨BTE ������͹�դ������
// *	��    ��: _ucOperate : ������
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// static void BTE_SetOperateCode(uint8_t _ucOperate)
// {
// 	/*  �趨BTE ������͹�դ������  */
// 	RA8875_WriteReg(0x51, _ucOperate);
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_SetFrontColor
// *	����˵��: �趨ǰ��ɫ
// *	��    ��: ��ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_SetFrontColor(uint16_t _usColor)
// {
// s_ucRA8875Busy = 1;
// RA8875_WriteReg(0x63, (_usColor & 0xF800) >> 11);	/* R5  */
// RA8875_WriteReg(0x64, (_usColor & 0x07E0) >> 5);	/* G6 */
// RA8875_WriteReg(0x65, (_usColor & 0x001F));			/* B5 */
// s_ucRA8875Busy = 0;
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetBackColor
*	����˵��: �趨����ɫ
*	��    ��: ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_SetBackColor(uint16_t _usColor)
// {
// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x60, (_usColor & 0xF800) >> 11);	/* R5  */
// 	RA8875_WriteReg(0x61, (_usColor & 0x07E0) >> 5);	/* G6 */
// 	RA8875_WriteReg(0x62, (_usColor & 0x001F));			/* B5 */
// 	s_ucRA8875Busy = 0;
// }


/*
*********************************************************************************************************
*	�� �� ��: BTE_Start
*	����˵��: ����BTE����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
// */
// void BTE_Start(void)
// {
// 	s_ucRA8875Busy = 1;
// 	/* RA8875_WriteReg(0x50, 0x80);  ����ʹ�������������Ϊ�ڲ��Ѿ������� s_ucRA8875Busy ��־ */
// 	RA8875_WriteCmd(0x50);	/* ���üĴ�����ַ */
// 	RA8875_WriteData(0x80);	/* д��Ĵ���ֵ */
// }

/*
*********************************************************************************************************
*	�� �� ��: BTE_Wait
*	����˵��: �ȴ�BTE��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void BTE_Wait(void)
// {
// 	while ((RA8875_ReadStatus() & 0x40) == 0x40);
// 	s_ucRA8875Busy = 0;
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_IsBusy
*	����˵��: RA8875�Ƿ�æ
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾæ�� 0 ��ʾ����
*********************************************************************************************************
// */
// uint8_t RA8875_IsBusy(void)
// {
// 	if (s_ucRA8875Busy == 0)
// 	{
// 		return 0;
// 	}
// 	return 1;
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WaitBusy
*	����˵��: �ȴ�RA8875����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_WaitBusy(void)
// {
// 	while ((RA8875_ReadStatus() & 0x80) == 0x80);
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_Layer1Visable
*	����˵��: RA8875 ͼ��1��ʾ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_Layer1Visable(void)
// {
// 	/* 0x52 �Ĵ����� Bit2:0
// 		ͼ����ʾģʽ
// 		000b : ֻ��ͼ��1 ��ʾ��
// 		001b : ֻ��ͼ��2 ��ʾ��
// 		010b : ��ʾͼ��1 ��ͼ��2 �Ľ���/����ģʽ��
// 		011b : ��ʾͼ��1 ��ͼ��2 ��ͨ͸ģʽ��
// 		100b : Boolean OR��
// 		101b : Boolean AND��
// 		110b : ��������ģʽ (Floating window mode)��
// 		111b :������
// 	*/
// 	RA8875_WriteReg(0x52, RA8875_ReadReg(0x52) & 0xF8);	/* ֻ��ͼ��1 ��ʾ */
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_Layer2Visable
*	����˵��: ֻ��ʾͼ��2
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_Layer2Visable(void)
{
	/* 0x52 �Ĵ����� Bit2:0
		ͼ����ʾģʽ
		000b : ֻ��ͼ��1 ��ʾ��
		001b : ֻ��ͼ��2 ��ʾ��
		010b : ��ʾͼ��1 ��ͼ��2 �Ľ���/����ģʽ��
		011b : ��ʾͼ��1 ��ͼ��2 ��ͨ͸ģʽ��
		100b : Boolean OR��
		101b : Boolean AND��
		110b : ��������ģʽ (Floating window mode)��
		111b :������
	*/
	RA8875_WriteReg(0x52, (RA8875_ReadReg(0x52) & 0xF8) | 0x01);	/* ֻ��ͼ��2 ��ʾ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_TouchInit
*	����˵��: ��ʼ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_TouchInit(void)
// {
// 	RA8875_WriteReg(0xF0, (1 << 2));	/* ������������ж�λ */

// 	/*
// 		REG[71h] Touch Panel Control Register 1 (TPCR1)

// 		Bit7 N/A
// 		Bit6 �������ģʽ�趨
// 			0 : �Զ�ģʽ��
// 			1 : �ֶ�ģʽ��
// 		Bit5 ������������ADC �ο���ѹ(Vref)��Դ�趨
// 			0 : �ڲ������ο���ѹ��
// 			1 : �ⲿ����ο���ѹ��ADC �ο���ѹ׼λ= 1/2 VDD��
// 		Bit4-3 N/A
// 		Bit2 �����ж�Ѷ�ŵ�����������·ѡ��
// 			0: �ر�����������·��
// 			1: ��������������·��
// 		Bit1-0 ��������ֶ�ģʽ֮ѡ��λ
// 			00b : ����ģʽ�����ؿ��Ƶ�Ԫ��������ģʽ��
// 			01b : ��ⴥ���¼��������ڴ�ģʽ����������ⴥ���¼��ķ�
// 				�����¼��������������жϻ����ɻ�������֪(REG[F1h]
// 				Bit2)��
// 			10b : X ������ߢȡģʽ���ڴ�ģʽ����λ�õ�X �����ݻᱻ��
// 				���� REG[72h] ��REG[74h]��
// 			11b : Y ������ߢȡģʽ���ڴ�ģʽ����λ�õ�Y �����ݻᱻ��
// 				����REG[73h] and REG[74h]��
// 	*/
// 	RA8875_WriteReg(0x71, (0 << 6) | (0 << 5) | (1 << 2));	/* ѡ���Զ�ģʽ */

// 	/*
// 		REG[70h] Touch Panel Control Register 0 (TPCR0)

// 		Bit7 ������幦���趨
// 			0 : �رա�
// 			1 : ������
// 		Bit6-4 ������������ȡ��ʱ���趨
// 			000b : ADC ȡ��ʱ��Ϊ512 ��ϵͳƵ�����ڡ�
// 			001b : ADC ȡ��ʱ��Ϊ 1024 ��ϵͳƵ�����ڡ�
// 			010b : ADC ȡ��ʱ��Ϊ 2048 ��ϵͳƵ�����ڡ�
// 			011b : ADC ȡ��ʱ��Ϊ 4096 ��ϵͳƵ�����ڡ�
// 			100b : ADC ȡ��ʱ��Ϊ 8192 ��ϵͳƵ�����ڡ�
// 			101b : ADC ȡ��ʱ��Ϊ 16384 ��ϵͳƵ�����ڡ�
// 			110b : ADC ȡ��ʱ��Ϊ 32768 ��ϵͳƵ�����ڡ�
// 			111b : ADC ȡ��ʱ��Ϊ65536 ��ϵͳƵ�����ڡ�
// 		Bit3 ������廽��ģʽ
// 			0 : �رմ����¼�����ģʽ��
// 			1 : �����¼��ɻ���˯��ģʽ��
// 		Bit2-0 ������������ADC Ƶ���趨
// 			000b : ϵͳƵ�ʡ�
// 			001b : ϵͳƵ�� / 2��
// 			010b : ϵͳƵ�� / 4��
// 			011b : ϵͳƵ�� / 8��
// 			100b : ϵͳƵ�� / 16��
// 			101b : ϵͳƵ�� / 32��
// 			110b : ϵͳƵ�� / 64��
// 			111b : ϵͳƵ�� / 128��

// 		ע : ADC ������Ƶ���趨���ܳ���10MHz��
// 	*/
// 	RA8875_WriteReg(0x70, (1 << 7) | (3 << 4) | (0 << 3) | (2 << 0));	/* �����������ܣ� */
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_TouchReadX
// *	����˵��: ��ȡXλ��ADC   �������������ʱ�жϷ��������ã���Ҫ����Ĵ������ʳ�ͻ )
// *	��    ��: ��
// *	�� �� ֵ: ADCֵ��10Bit
// *********************************************************************************************************
// */
// uint16_t RA8875_TouchReadX(void)
// {
// 	uint16_t usAdc;
// 	uint8_t ucRegValue;
// 	uint8_t ucReg74;

// 	/* �����ȡ�ж��¼���־ */
// 	ucRegValue = RA8875_ReadReg(0xF1);
// 	if (ucRegValue & (1 << 2))
// 	{
// 		ucReg74 = RA8875_ReadReg(0x74);
// 		usAdc = RA8875_ReadReg(0x72);	/* Bit9-2 */
// 		usAdc <<= 2;
// 		usAdc += (ucReg74 & 0x03);

// 		s_usTouchX = usAdc;

// 		usAdc = RA8875_ReadReg(0x73);	/* Bit9-2 */
// 		usAdc <<= 2;
// 		usAdc += ((ucReg74 & 0x0C) >> 2);

// 		s_usTouchY = usAdc;
// 	}
// 	else
// 	{
// 		s_usTouchX = 0;
// 		s_usTouchY = 0;
// 	}

// 	/*
// 		bit2 д�빦��?? ��������ж����λ
// 		0 : δ������
// 		1 : �����������жϡ�
// 		��ȡ����?? ��������ж�״̬
// 		0 : δ������������жϡ�
// 		1 : ������������жϡ�
// 	*/

// 	/*
// 		��Ҫ�����������д�Ĵ�������Ϊ�ú�����д��busy��־
// 		RA8875_WriteReg(0xF1, (1 << 2));	 ��������� �Ż��´β���.
// 	*/
// 	RA8875_WriteCmd(0xF1);
// 	RA8875_WriteData(1 << 2);

// 	return s_usTouchX;
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_TouchReadY
*	����˵��: ��ȡYλ��ADC �� �����ȵ��� RA8875_TouchReadX()
*	��    ��: ��
*	�� �� ֵ: ADCֵ��10Bit
*********************************************************************************************************
*/
uint16_t RA8875_TouchReadY(void)
{
	return s_usTouchY;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DispOn
*	����˵��: ����ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_DispOn(void)
{
	RA8875_WriteReg(0x01, 0x80);
	//RA8875_WriteReg(0x8A, 1<<6);	//�������ó����� ����csf
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DispOff
*	����˵��: �ر���ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_DispOff(void)
// {
// 	RA8875_WriteReg(0x01, 0x00);
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_PutPixel
*	����˵��: ��1������
*	��    �Σ�
*			_usX,_usY : ��������
*			_usColor  ��������ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	RA8875_SetCursor(_usX, _usY);	/* ���ù��λ�� */

	s_ucRA8875Busy = 1;

	RA8875_WriteCmd(0x02); 		/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */
	RA8875_WriteData16(_usColor);
	s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_GetPixel
*	����˵��: ��ȡ1������
*	��    �Σ�
*			_usX,_usY : ��������
*			_usColor  ��������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
// */
uint16_t RA8875_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	RA8875_SetReadCursor(_usX, _usY);	/* ���ö�ȡ���λ�� */

	s_ucRA8875Busy = 1;

	RA8875_WriteCmd(0x02);
	usRGB = RA8875_ReadData16();

	s_ucRA8875Busy = 0;

	return usRGB;
}

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_ClrScr
// *	����˵��: �����������ɫֵ����.RA8875֧��Ӳ����ɫ��䡣�ú������Ե�ǰ�������ʾ���ڽ�������. ��ʾ
// *			 ���ڵ�λ�úʹ�С�� RA8875_SetDispWin() ������������
// *	��    �Σ�_usColor : ����ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_ClrScr(uint16_t _usColor)
// {
// 	/* Ҳ����ͨ��0x30-0x37�Ĵ�����û�õ�ǰ�������ʾ���� */

// 	/* ��ɫ��������, ����pdf ��162ҳ
// 	�˹���ʹ���ڽ�ѡ���ض�������������ǉ\�����ĳ��ǰ��ɫ��R8875 ����ĵ�ɫ�趨ΪBTE ǰ��ɫ��

// 	�������裺
// 		1. �趨Ŀ��ͼ���λ�� REG[58h], [59h], [5Ah], [5Bh]
// 		2. �趨BTE ��Ⱥ͸߶� REG[5Ch], [5Dh], [5Eh], [5Fh]
// 		3. �趨BTE ������͹�դ������  REG[51h] Bit[3:0] = 0Ch
// 		4. �趨ǰ��ɫ  REG[63h], [64h], [65h]
// 		5. ����BTE ����  REG[50h] Bit7 = 1
// 		6. ���״̬������ STSR Bit6��ȷ��BTE �Ƿ����
// 	*/
// 	BTE_SetTarBlock(s_WinX, s_WinY, s_WinHeight, s_WinWidth, 0);	/* ����BTEλ�úͿ�ȸ߶��Լ�Ŀ��ͼ�㣨0��1�� */
// 	BTE_SetOperateCode(0x0C);		/* �趨BTE ������͹�դ������  REG[51h] Bit[3:0] = 0Ch */
// 	RA8875_SetFrontColor(_usColor);	/* ����BTEǰ��ɫ */
// 	BTE_Start();					/* ����BTE ���� */
// 	BTE_Wait();						/* �ȴ��������� */
// 	
// // 	RA8875_Delaly1ms();
// // 	BTE_SetTarBlock(s_WinX, s_WinY, s_WinHeight, s_WinWidth, 1);	/* ����BTEλ�úͿ�ȸ߶��Լ�Ŀ��ͼ�㣨0��1�� */
// // 	BTE_SetOperateCode(0x0C);		/* �趨BTE ������͹�դ������  REG[51h] Bit[3:0] = 0Ch */
// // 	RA8875_SetFrontColor(_usColor);	/* ����BTEǰ��ɫ */
// // 	BTE_Start();					/* ����BTE ���� */
// // 	BTE_Wait();						/* �ȴ��������� */
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawBMP
*	����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ����򣺴����ң����ϵ���
*	��    �Σ�
*			_usX, _usY : ͼƬ������
*			_usHeight  ��ͼƬ�߶�
*			_usWidth   ��ͼƬ���
*			_ptr       ��ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
// */
// void RA8875_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
// {
// 	uint32_t index = 0;
// 	const uint16_t *p;

// 	/* ����ͼƬ��λ�úʹ�С�� ��������ʾ���� */
// 	RA8875_SetDispWin(_usX, _usY, _usHeight, _usWidth);

// 	s_ucRA8875Busy = 1;

// 	RA8875_WriteCmd(0x02); 		/* ׼����д�Դ� */

// 	p = _ptr;
// 	for (index = 0; index < _usHeight * _usWidth; index++)
// 	{
// 		/*
// 			armfly : �����Ż�, �����͵�չ��
// 			RA8875_WriteRAM(_ptr[index]);

// 			�˴��ɿ�����DMA����
// 		*/
// 		RA8875_WriteData16(*p++);
// 	}
// 	s_ucRA8875Busy = 0;

// 	/* �˳����ڻ�ͼģʽ */
// 	RA8875_QuitWinMode();
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_DrawLine
// *	����˵��: ����RA8875��Ӳ�����ܣ���2��仭һ��ֱ�ߡ�
// *	��    �Σ�
// *			_usX1, _usY1 ����ʼ������
// *			_usX2, _usY2 ����ֹ��Y����
// *			_usColor     ����ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
// {
// 	/* pdf ��131ҳ
// 		RA8875 ֧��ֱ�߻�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ�ֱ�ߡ�����
// 		��ֱ�ߵ���ʼ��REG[91h~94h] �������REG[95h~98h]��ֱ�ߵ���ɫREG[63h~65h]��Ȼ����
// 		����ͼ�趨REG[90h] Bit4 = 0, Bit0=0 ��REG[90h] Bit7 = 1��RA8875 �ͻὫֱ�ߵ�ͼ��д��
// 		DDRAM����Ե���TFT ģ���ϾͿ�����ʾ������ֱ�ߡ�
// 	*/

// 	/* ����������� */
// 	RA8875_WriteReg(0x91, _usX1);
// 	RA8875_WriteReg(0x92, _usX1 >> 8);
// 	RA8875_WriteReg(0x93, _usY1);
// 	RA8875_WriteReg(0x94, _usY1 >> 8);

// 	/* �����յ����� */
// 	RA8875_WriteReg(0x95, _usX2);
// 	RA8875_WriteReg(0x96, _usX2 >> 8);
// 	RA8875_WriteReg(0x97, _usY2);
// 	RA8875_WriteReg(0x98, _usY2 >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* ����ֱ�ߵ���ɫ */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (0 << 4) | (0 << 0));	/* ��ʼ��ֱ�� */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));				/* �ȴ����� */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_DrawRect
// *	����˵��: ����RA8875Ӳ�����ܻ��ƾ���
// *	��    �Σ�
// *			_usX,_usY���������Ͻǵ�����
// *			_usHeight �����εĸ߶�
// *			_usWidth  �����εĿ��
// *			_usColor  ����ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
// {
// 	/*
// 		RA8875 ֧�ַ��λ�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ����Ρ�����
// 	�����ε���ʼ��REG[91h~94h]�������REG[95h~98h]�����ε���ɫREG[63h~65h]��Ȼ����
// 	����ͼ�趨REG[90h] Bit4=1, Bit0=0 ��REG[90h] Bit7 = 1��RA8875 �ͻὫ���ε�ͼ��д��
// 	DDRAM����Ե���TFT ģ���ϾͿ�����ʾ�����ķ��Ρ����趨REG[90h] Bit5 = 1����ɻ���һ
// 	ʵ�ķ��� (Fill)

// 	 ---------------->---
// 	|(_usX��_usY)        |
// 	V                    V  _usHeight
// 	|                    |
// 	 ---------------->---
// 		  _usWidth
// 	*/

// 	/* ����������� */
// 	RA8875_WriteReg(0x91, _usX);
// 	RA8875_WriteReg(0x92, _usX >> 8);
// 	RA8875_WriteReg(0x93, _usY);
// 	RA8875_WriteReg(0x94, _usY >> 8);

// 	/* �����յ����� */
// 	RA8875_WriteReg(0x95, _usX + _usWidth - 1);
// 	RA8875_WriteReg(0x96, (_usX + _usWidth - 1) >> 8);
// 	RA8875_WriteReg(0x97, _usY + _usHeight - 1);
// 	RA8875_WriteReg(0x98, (_usY + _usHeight - 1) >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* ������ɫ */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (0 << 5) | (1 << 4) | (0 << 0));	/* ��ʼ������  */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));							/* �ȴ����� */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_FillRect
// *	����˵��: ����RA8875Ӳ���������һ������Ϊ��ɫ
// *	��    �Σ�
// *			_usX,_usY���������Ͻǵ�����
// *			_usHeight �����εĸ߶�
// *			_usWidth  �����εĿ��
// *			_usColor  ����ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
// {
// 	/*
// 		RA8875 ֧�ַ��λ�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ����Ρ�����
// 	�����ε���ʼ��REG[91h~94h]�������REG[95h~98h]�����ε���ɫREG[63h~65h]��Ȼ����
// 	����ͼ�趨REG[90h] Bit4=1, Bit0=0 ��REG[90h] Bit7 = 1��RA8875 �ͻὫ���ε�ͼ��д��
// 	DDRAM����Ե���TFT ģ���ϾͿ�����ʾ�����ķ��Ρ����趨REG[90h] Bit5 = 1����ɻ���һ
// 	ʵ�ķ��� (Fill)

// 	 ---------------->---
// 	|(_usX��_usY)        |
// 	V                    V  _usHeight
// 	|                    |
// 	 ---------------->---
// 		  _usWidth
// 	*/

// 	/* ����������� */
// 	RA8875_WriteReg(0x91, _usX);
// 	RA8875_WriteReg(0x92, _usX >> 8);
// 	RA8875_WriteReg(0x93, _usY);
// 	RA8875_WriteReg(0x94, _usY >> 8);

// 	/* �����յ����� */
// 	RA8875_WriteReg(0x95, _usX + _usWidth - 1);
// 	RA8875_WriteReg(0x96, (_usX + _usWidth - 1) >> 8);
// 	RA8875_WriteReg(0x97, _usY + _usHeight - 1);
// 	RA8875_WriteReg(0x98, (_usY + _usHeight - 1) >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* ������ɫ */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (1 << 5) | (1 << 4) | (0 << 0));	/* ��ʼ������  */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));							/* �ȴ����� */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

/*
// *********************************************************************************************************
// *	�� �� ��: RA8875_DrawCircle
// *	����˵��: ����һ��Բ���ʿ�Ϊ1������
// *	��    �Σ�
// *			_usX,_usY  ��Բ�ĵ�����
// *			_usRadius  ��Բ�İ뾶
// *			_usColor  ����ɫ
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
// {
// 	/*
// 		RA8875 ֧��Բ�λ�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ�Բ�����趨
// 		Բ�����ĵ�REG[99h~9Ch]��Բ�İ뾶REG[9Dh]��Բ����ɫREG[63h~65h]��Ȼ��������ͼ
// 		REG[90h] Bit6 = 1��RA8875 �ͻὫԲ��ͼ��д��DDRAM����Ե���TFT ģ���ϾͿ�����ʾ��
// 		����Բ�����趨REG[90h] Bit5 = 1����ɻ���һʵ��Բ (Fill)�����趨REG[90h] Bit5 = 0�����
// 		��������Բ (Not Fill
// 	*/
// 	/* ����Բ�İ뾶 */
// 	if (_usRadius > 255)
// 	{
// 		return;
// 	}

// 	/* ����Բ������ */
// 	RA8875_WriteReg(0x99, _usX);
// 	RA8875_WriteReg(0x9A, _usX >> 8);
// 	RA8875_WriteReg(0x9B, _usY);
// 	RA8875_WriteReg(0x9C, _usY >> 8);

// 	RA8875_WriteReg(0x9D, _usRadius);	/* ����Բ�İ뾶 */

// 	RA8875_SetFrontColor(_usColor);	/* ������ɫ */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 6) | (0 << 5));				/* ��ʼ��Բ, �����  */
// //	while (RA8875_ReadReg(0x90) & (1 << 6));				/* �ȴ����� */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_FillCircle
*	����˵��: ���һ��Բ
*	��    �Σ�
*			_usX,_usY  ��Բ�ĵ�����
*			_usRadius  ��Բ�İ뾶
*			_usColor   ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
// {
// 	/*
// 		RA8875 ֧��Բ�λ�ͼ���ܣ���ʹ�����Լ��׻���ٵ�MCU �Ϳ�����TFT ģ���ϻ�Բ�����趨
// 		Բ�����ĵ�REG[99h~9Ch]��Բ�İ뾶REG[9Dh]��Բ����ɫREG[63h~65h]��Ȼ��������ͼ
// 		REG[90h] Bit6 = 1��RA8875 �ͻὫԲ��ͼ��д��DDRAM����Ե���TFT ģ���ϾͿ�����ʾ��
// 		����Բ�����趨REG[90h] Bit5 = 1����ɻ���һʵ��Բ (Fill)�����趨REG[90h] Bit5 = 0�����
// 		��������Բ (Not Fill
// 	*/
// 	/* ����Բ�İ뾶 */
// 	if (_usRadius > 255)
// 	{
// 		return;
// 	}

// 	/* ����Բ������ */
// 	RA8875_WriteReg(0x99, _usX);
// 	RA8875_WriteReg(0x9A, _usX >> 8);
// 	RA8875_WriteReg(0x9B, _usY);
// 	RA8875_WriteReg(0x9C, _usY >> 8);

// 	RA8875_WriteReg(0x9D, _usRadius);	/* ����Բ�İ뾶 */

// 	RA8875_SetFrontColor(_usColor);	/* ������ɫ */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 6) | (1 << 5));				/* ��ʼ��Բ, ���  */
// //	while (RA8875_ReadReg(0x90) & (1 << 6));				/* �ȴ����� */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_SetFont
// *	����˵��: �ı�ģʽ�������������塢�о���־�
// *	��    �Σ�
// *			_ucFontType : ��������: RA_FONT_16, RA_FONT_24, RA_FONT_32
// *			_ucLineSpace : �о࣬���ص�λ (0-31)
// *			_ucCharSpace : �־࣬���ص�λ (0-63)
// *
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_SetFont(uint8_t _ucFontType, uint8_t _ucLineSpace, uint8_t _ucCharSpace)
// {
// 	/*
// 		[29H]������ģʽ�£������趨���ּ���о� (��λ: ����) ��
// 		ֻ�е�5��bit��Ч��0-31
// 	*/
// 	if (_ucLineSpace >31)
// 	{
// 		_ucLineSpace = 31;
// 	}
// 	RA8875_WriteReg(0x29, _ucLineSpace);

// 	/*
// 		[2EH] �����ַ���ࣨ���ص�λ��0-63���������壨16*16��24*24��32*32��
// 	*/
// 	if (_ucCharSpace > 63)
// 	{
// 		_ucCharSpace = 63;
// 	}
// 	if (_ucFontType > RA_FONT_32)
// 	{
// 		_ucFontType = RA_FONT_16;
// 	}
// 	RA8875_WriteReg(0x2E, (_ucCharSpace & 0x3F) | (_ucFontType << 6));
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetFontZoom
*	����˵��: �ı�ģʽ���������ֵķŴ�ģʽ��1X,2X,3X, 4X
*	��    �Σ�
*			_ucHSize : ����ˮƽ�Ŵ�����0-3 �ֱ��Ӧ X1��X2��X3��X4
*			_ucVSize : ���ִ��÷Ŵ�����0-3 �ֱ��Ӧ X1��X2��X3��X4
*		�������������ö�٣�RA_SIZE_X1��RA_SIZE_X2��RA_SIZE_X3��RA_SIZE_X4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_SetTextZoom(uint8_t _ucHSize, uint8_t _ucVSize)
// {
// 	/*
// 		pdf ��22ҳ		[22H]
// 		bit7 ���ڶ��룬һ�㲻�ã�ȱʡ��0
// 		bit6 ����ͨ͸��һ�㲻�ã�ȱʡ��0
// 		bit4 ������ת90����һ�㲻�ã�ȱʡ��0
// 		bit3-2 ˮƽ�Ŵ���
// 		bit1-0 ��ֱ�Ŵ���
// 	*/
// 	RA8875_WriteReg(0x22, ((_ucHSize & 0x03) << 2) | ( _ucVSize & 0x03));
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DispAscii
*	����˵��: ��ʾASCII�ַ������ַ�����������RA8875�ڲ�ROM�е�ASCII�ַ�����8*16������֧�ֺ���.
*			������ɫ��������ɫ���Ƿ�ͨ͸�������ĺ�����������
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	��    �Σ�_usX, _usY : ��ʾλ�ã����꣩
*			 _ptr : AascII�ַ�������0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_DispAscii(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 �ڽ�8x16 ���ASCII ����ROM���ṩʹ���߸�����ķ�ʽ���ض����� (Code) ������
// 		�֡��ڽ����ּ�֧��ISO/IEC 8859-1~4 �����׼�����⣬ʹ���߿���͸��REG[60h~62h] ѡ��
// 		����ǰ����ɫ���Լ�͸��REG[63h~65h] ѡ�񱳾���ɫ.

// 		ISO/IEC 8859-1���ֳ�Latin-1����ŷ���ԡ����ǹ��ʱ�׼����֯��ISO/IEC 8859�ĵ�һ��8λ�ַ�����
// 		����ASCIIΪ�������ڿ��õ�0xA0-0xFF�ķ�Χ�ڣ�����192����ĸ�����ţ����Թ�ʹ�ñ������ŵ�������ĸ����ʹ�á�

// 		ISO/IEC 8859-2 Latin-2����ŷ���ԡ����ǹ��ʱ�׼����֯��ISO/IEC 8859������һ��8λ�ַ��� .
// 		ISO/IEC 8859-3 ��ŷ�����ַ���
// 		ISO/IEC 8859-4 ��ŷ�����ַ���
// 	*/

// 	/*
// 	(1) Text mode  REG[40h] bit7=1
// 	(2) Internal Font ROM Select   REG[21h] bit7=0, bit5=0
// 	(3) Font foreground and background color Select  REG[63h~65h], REG[60h~62h]
// 	(4) Write the font Code  CMD_WR[02h]    DATA_WR[font_code]
// 	*/

// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* ����Ϊ�ı�ģʽ */

// 	/* ѡ��CGROM font; ѡ���ڲ�CGROM; �ڲ�CGROM ����ѡ��ISO/IEC 8859-1. */
// 	RA8875_WriteReg(0x2F, 0x00);
// 	RA8875_WriteReg(0x21, (0 << 7) | (0 << 5) | (0 << 1) | (0 << 0));

// 	s_ucRA8875Busy = 1;


// 	RA8875_WriteCmd(0x02); 			/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */

// 	/* ��ʼѭ�������ַ� */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_WriteData(*_ptr);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);	/* ����ȴ��ڲ�д��������� */
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* ��ԭΪͼ��ģʽ */
// }


// /*
// *********************************************************************************************************
// *	�� �� ��: LCD_DispStr
// *	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ���
// *	��    �Σ�
// *		_usX : X���꣬����3.0���������ΧΪ��0 - 399��
// *		_usY : Y���꣬����3.0���������ΧΪ ��0 - 239��
// *		_ptr  : �ַ���ָ��
// *		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
// {
// 	uint32_t i;
// 	uint8_t code1;
// 	uint8_t code2;
// 	uint32_t address;
// 	uint8_t buf[24 * 24 / 8];	/* ���֧��24������ */
// 	uint8_t m, width;
// 	uint8_t font_width,font_height, font_bytes;
// 	uint16_t x, y;
// 	const uint8_t *pAscDot;
// 	const uint8_t *pHzDot;

// 	/* �������ṹΪ��ָ�룬��ȱʡ��16���� */
// 	if (_tFont->usFontCode == FC_ST_12)
// 	{
// 		font_height = 12;
// 		font_width = 12;
// 		font_bytes = 24;
// 		pAscDot = g_Ascii12;
// 		pHzDot = g_Hz12;
// 	}
// 	else
// 	{
// 		/* ȱʡ��16���� */
// 		font_height = 16;
// 		font_width = 16;
// 		font_bytes = 32;
// 		pAscDot = g_Ascii16;
// 		pHzDot = g_Hz16;
// 	}
// 	
//   //RA8875_WriteReg(0x40, (1 << 7));	/* ����Ϊ�ı�ģʽ */
// 	/* ��ʼѭ�������ַ� */
// 	while (*_ptr != 0)
// 	{
// 		code1 = *_ptr;	/* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
// 		if (code1 < 0x80)
// 		{
// 			/* ��ascii�ַ������Ƶ�buf */
// 			memcpy(buf, &pAscDot[code1 * (font_bytes / 2)], (font_bytes / 2));
// 			width = font_width / 2;
// 		}
// 		else
// 		{
// 			code2 = *++_ptr;
// 			if (code2 == 0)
// 			{
// 				break;
// 			}

// 			/* ����16�����ֵ����ַ
// 				ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32
// 				;
// 			*/
// 			#ifdef USE_SMALL_FONT
// 				m = 0;
// 				while(1)
// 				{
// 					address = m * (font_bytes + 2);
// 					m++;
// 					if ((code1 == pHzDot[address + 0]) && (code2 == pHzDot[address + 1]))
// 					{
// 						address += 2;
// 						memcpy(buf, &pHzDot[address], font_bytes);
// 						break;
// 					}
// 					else if ((pHzDot[address + 0] == 0xFF) && (pHzDot[address + 1] == 0xFF))
// 					{
// 						/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
// 						memset(buf, 0xFF, font_bytes);
// 						break;
// 					}
// 				}
// 			#else	/* ��ȫ�ֿ� */
// 				/* �˴���Ҫ�����ֿ��ļ����λ�ý����޸� */
// 				address = ((code1-0xa1) * 94 + (code2-0xa1)) * font_bytes + HZK16_ADDR;
// 				memcpy(buf, (const uint8_t *)address, font_bytes);
// 			#endif

// 				width = font_width;
// 		}

// 		y = _usY;
// 		/* ��ʼˢLCD */
// 		for (m = 0; m < font_height; m++)	/* �ַ��߶� */
// 		{
// 			x = _usX;
// 			for (i = 0; i < width; i++)	/* �ַ���� */
// 			{
// 				if ((buf[m * ((2 * width) / font_width) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
// 				{
// 					RA8875_PutPixel(x, y, _tFont->usTextColor);	/* ����������ɫΪ����ɫ */
// 				}
// 				else
// 				{
// 					RA8875_PutPixel(x, y, _tFont->usBackColor);	/* ����������ɫΪ���ֱ���ɫ */
// 				}

// 				x++;
// 			}
// 			y++;
// 		}

// 		if (_tFont->usSpace > 0)
// 		{
// 			/* ������ֵ�ɫ��_tFont->usBackColor�������ּ����ڵ���Ŀ�ȣ���ô��Ҫ������֮�����(��ʱδʵ��) */
// 		}
// 		_usX += width + _tFont->usSpace;	/* �е�ַ���� */
// 		_ptr++;			/* ָ����һ���ַ� */
// 	}
// 	//RA8875_WriteReg(0x40, (0 << 7));	/* ����Ϊ�ı�ģʽ */
// }
/*
*********************************************************************************************************
*	�� �� ��: RA8875_DispStr1
*	����˵��: ��ʾ�ַ������ַ�����������RA8875��ӵ��ֿ�оƬ��֧�ֺ���.
*			������ɫ��������ɫ���Ƿ�ͨ͸�������ĺ�����������
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	��    �Σ�_usX, _usY : ��ʾλ�ã����꣩
*			 _ptr : AascII�ַ�������0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// void RA8875_DispStr1(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 ͸��ʹ�� �� �Ϻ���ͨ��˾ (Genitop Inc) �ⲿ����ʽ�����ڴ� (Font ROM)����֧�ָ���
// 		������д�뵽DDRAM �С�RA8875 ���Ϻ���ͨ��˾ (Genitop Inc) ���ݵĲ�Ʒ���� :
// 		GT21L16TW/GT21H16T1W ��GT23L16U2W ��GT23L24T3Y/GT23H24T3Y ��GT23L24M1Z ��
// 		�� GT23L32S4W/GT23H32S4W����Щ�������16x16, 24x24, 32x32 �� (Dot) �벻ͬ����

// 		������RA8875�����弯�ɵ��ֿ�оƬΪ ��ͨ�ֿ�оƬ_GT23l32S4W

// 		GT23L32S4W��һ���ں�11X12����15X16�㡢24X24����32X32����ĺ��ֿ�оƬ��֧��GB2312
// 		���꺺�֣����й����ű�ί�Ϸ���Ȩ����SCII�ַ������и�ʽΪ���ú��š��û�ͨ���ַ����룬���ñ���
// 		���ṩ�ķ�����������ַ�������оƬ�еĵ�ַ���ɴӸõ�ַ���������ַ�������Ϣ��
// 	*/

// 	/* �����ı���ʾλ�ã�ע���ı�ģʽ��д�����ͼ��ģʽ��д�����ǲ�ͬ�ļĴ��� */
// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* ����Ϊ�ı�ģʽ */

// 	/*
// 		Serial Flash/ROM Ƶ��Ƶ���趨
// 			0xb: SFCL Ƶ�� = ϵͳƵ��Ƶ��(��DMA Ϊ����״̬������ɫ�����Ϊ256 ɫ����SFCL Ƶ��
// 				�̶�Ϊ=ϵͳƵ��Ƶ��/ 2)
// 			10b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 2
// 			11b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 4

// 		������������ϵͳƵ��Ϊ 68MHz

// 		GT23L32S4W�ķ����ٶȣ�SPI ʱ��Ƶ�ʣ�20MHz(max.)

// 		�����Ҫ���� 4 ��Ƶ, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* ����Ϊ�ı�ģʽ */

// 	/* ѡ���ⲿ����ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # ѡ��
// 				0:ѡ��Serial Flash/ROM 0 �ӿڡ�[������RA8875�������ֿ�оƬ���� 0 �ӿ�]
// 				1:ѡ��Serial Flash/ROM 1 �ӿڡ�
// 		6	Serial Flash/ROM Ѱַģʽ
// 				0: 24 λѰַģʽ��
// 				��λ������Ϊ0��
// 		5	Serial Flash/ROM ����ģʽ
// 				0: ����ģʽ 0��
// 				1: ����ģʽ 3��
// 		4-3	Serial Flash /ROM ��ȡ���� (Read Cycle)
// 			00b: 4 bus ?? �޿����� (No Dummy Cycle)��
// 			01b: 5 bus ??1 byte �����ڡ�
// 			1Xb: 6 bus ??2 byte �����ڡ�
// 		2	Serial Flash /ROM ��ȡģʽ (Access Mode)
// 			0: ����ģʽ ��
// 			1: DMA ģʽ��
// 		1-0	Serial Flash /ROM I/F Data Latch ѡ��ģʽ
// 			0Xb: ��һģʽ��
// 			10b: ˫��ģʽ0��
// 			11b: ˫��ģʽ1��
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		�����ⲿ����оƬ�ͺ�Ϊ GT23L32S4W, ����ΪGB2312,

// 		Bit1:0 ����ASCII�ַ��ĸ�ʽ��
// 			0 = NORMAL		 [�ʻ�ϸ, �ͺ��ֶ�������]
// 			1 = Arial		 [�ʻ��֣��ͺ��ֵײ�����]
// 			2 = Roman		 [�ʻ�ϸ, �ͺ��ֵײ�����]
// 			3 = Bold		 [����,������]
// 	 */
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteCmd(0x02); 			/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */

// 	/* ��ʼѭ�������ַ� */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_WriteData(*_ptr);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* ��ԭΪͼ��ģʽ */
// }

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetDispWin
*	����˵��: ������ʾ���ڣ����봰����ʾģʽ���ڴ�����ʾģʽ������д�Դ棬�����Զ����趨�����ڽ��е���
*	��    �Σ�
*		_usX : ˮƽ����
*		_usY : ��ֱ����
*		_usHeight: ���ڸ߶�
*		_usWidth : ���ڿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{

	uint16_t usTemp;

	/* ����ϵͳʾ��ͼ�� ��������
			 -----------------------------
			|(0,0)                        |
			|     --------->              |
			|         |                   |
			|         |                   |
			|         |                   |
			|         V                   |
			|     --------->              |
			|                    (479,271)|
			 -----------------------------

		���Ͻ����������, ɨ�跽���ȴ����ң��ٴ��ϵ��¡�

		�����Ҫ��������ʽ������Ҫ��������������߼������ת��
	*/

	RA8875_WriteReg(0x30, _usX);
    RA8875_WriteReg(0x31, _usX >> 8);

	RA8875_WriteReg(0x32, _usY);
    RA8875_WriteReg(0x33, _usY >> 8);

	usTemp = _usWidth + _usX - 1;
	RA8875_WriteReg(0x34, usTemp);
    RA8875_WriteReg(0x35, usTemp >> 8);

	usTemp = _usHeight + _usY - 1;
	RA8875_WriteReg(0x36, usTemp);
    RA8875_WriteReg(0x37, usTemp >> 8);

	RA8875_SetCursor(_usX, _usY);

	/* ���浱ǰ������Ϣ������Ժ�ɫ��������Ч��.
	����һ��������ͨ����ȡ0x30-0x37�Ĵ�����õ�ǰ���ڣ�����Ч�ʽϵ� */
	s_WinX = _usX;
	s_WinY = _usY;
	s_WinHeight = _usHeight;
	s_WinWidth = _usWidth;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetCursor
*	����˵��: ����д�Դ�Ĺ��λ�ã�ͼ��ģʽ��
*	��    �Σ�_usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetCursor(uint16_t _usX, uint16_t _usY)
{
	/* �����ڴ�д�������� ��ע��0x80-83 �ǹ��ͼ�ε����꡿ */
	RA8875_WriteReg(0x46, _usX);
	RA8875_WriteReg(0x47, _usX >> 8);
	RA8875_WriteReg(0x48, _usY);
	RA8875_WriteReg(0x49, _usY >> 8);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetCursor
*	����˵��: ���ö��Դ�Ĺ��λ�á� �ܶ������Ŀ�����д���Ͷ��������ͬ�ļĴ���������RA8875�ǲ�ͬ�ġ�
*	��    �Σ�_usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetReadCursor(uint16_t _usX, uint16_t _usY)
{
	/* �����ڴ���������� */
	RA8875_WriteReg(0x4A, _usX);
	RA8875_WriteReg(0x4B, _usX >> 8);
	RA8875_WriteReg(0x4C, _usY);
	RA8875_WriteReg(0x4D, _usY >> 8);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetTextCursor
*	����˵��: �����ı�д����λ��
*	��    �Σ�_usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetTextCursor(uint16_t _usX, uint16_t _usY)
{
	/* �����ڴ���������� */
	RA8875_WriteReg(0x2A, _usX);
	RA8875_WriteReg(0x2B, _usX >> 8);
	RA8875_WriteReg(0x2C, _usY);
	RA8875_WriteReg(0x2D, _usY >> 8);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_QuitWinMode
*	����˵��: �˳�������ʾģʽ����Ϊȫ����ʾģʽ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_QuitWinMode(void)
{
	RA8875_SetDispWin(0, 0, 480, 800);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetBackLight
*	����˵��: ����RA8875оƬ��PWM1��ؼĴ���������LCD����
*	��    �Σ�_bright ���ȣ�0����255������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetBackLight(uint8_t _bright)
{
	if (_bright == 0)
	{
		/* �ر�PWM, PWM1����ȱʡ���LOW  */
		RA8875_WriteReg(0x8A, 0 << 6);
	}
	else if (_bright == 1)	/* ������� */
	{
		/* �ر�PWM, PWM1����ȱʡ���HIGH */
		RA8875_WriteReg(0x8A, 1 << 6);
	}
	else
	{
		/* ʹ��PWM1, ����ռ�ձȵ��� */

		/* 	REG[8Ah] PWM1 Control Register (P1CR)

			Bit7 ������ȵ��� (PWM1) �趨
				0 : �رգ���״̬�£�PWM1 ���׼λ���մ˻�����Bit6 ������
				1 : ������

			Bit6 PWM1 �ر�ʱ��׼λ
				0 : ��PWM �رջ���˯��ģʽʱ��PWM1 ���Ϊ��Low�� ״̬��
				1 : ��PWM �رջ���˯��ģʽʱ��PWM1 ���Ϊ��High�� ״̬��

			Bit5 ����

			Bit4 PWM1 ����ѡ��
				0 : PWM1 ���ܡ�
				1 : PWM1 �̶����һƵ��Ϊ�ⲿ��������Clock (Fin) Ƶ�� 1 /16 ��Clock

			Bit3-0  PWM1 ��·��Ƶ����Դѡ��PWM_CLK	������PWM���Ƶ�ʡ�
				0000b : SYS_CLK / 1 1000b : SYS_CLK / 256
				0001b : SYS_CLK / 2 1001b : SYS_CLK / 512
				0010b : SYS_CLK / 4 1010b : SYS_CLK / 1024
				0011b : SYS_CLK / 8 1011b : SYS_CLK / 2048
				0100b : SYS_CLK / 16 1100b : SYS_CLK / 4096
				0101b : SYS_CLK / 32 1101b : SYS_CLK / 8192
				0110b : SYS_CLK / 64 1110b : SYS_CLK / 16384
				0111b : SYS_CLK / 128 1111b : SYS_CLK / 32768

				��SYS_CLK�� ����ϵͳƵ�ʣ� ����SYS_CLK Ϊ20MHz�� ��Bit[3:0] =0001b ʱ��PWM1 Ƶ����ԴΪ10MHz��

				���ڰ�������4.3��ģ�飬SYS_CLK =  68.75MHz
				Bit3-0 ѡ��8ʱ��PWM_CLK = 68.75MHz / 256 = 0.2685546875 MHz = 268.55KHz��
				ʵ�������PWM����Ƶ�� = PWM_CLK / 256 = 1.049kHz������ʾ��������֤��PWMƵ�ʺ�׼��

				ѡ��10ʱ��Ƶ��256Hz���ң�����1KHz��Х����
		*/

//		RA8875_WriteReg(0x8A, (1 << 7) | 10);
		RA8875_WriteReg(0x8A, (1 << 7) | 7);

		/* REG[8Bh] PWM1 Duty Cycle Register (P1DCR) */
		RA8875_WriteReg(0x8B, _bright);
	}
}

// /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_DispStr
// *	����˵��: ��ʾ�ַ������ַ�����������RA8875��ӵ��ֿ�оƬ��֧�ֺ���.
// *			������ɫ��������ɫ���Ƿ�ͨ͸�������ĺ�����������
// *			void RA8875_SetFrontColor(uint16_t _usColor);
// *			void RA8875_SetBackColor(uint16_t _usColor);
// *	��    �Σ�_usX, _usY : ��ʾλ�ã����꣩
// *			 _ptr : AascII�ַ�������0����
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DispStrChip(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 ͸��ʹ�� �� �Ϻ���ͨ��˾ (Genitop Inc) �ⲿ����ʽ�����ڴ� (Font ROM)����֧�ָ���
// 		������д�뵽DDRAM �С�RA8875 ���Ϻ���ͨ��˾ (Genitop Inc) ���ݵĲ�Ʒ���� :
// 		GT21L16TW/GT21H16T1W ��GT23L16U2W ��GT23L24T3Y/GT23H24T3Y ��GT23L24M1Z ��
// 		�� GT23L32S4W/GT23H32S4W����Щ�������16x16, 24x24, 32x32 �� (Dot) �벻ͬ����

// 		������RA8875�����弯�ɵ��ֿ�оƬΪ ��ͨ�ֿ�оƬ_GT23l32S4W

// 		GT23L32S4W��һ���ں�11X12����15X16�㡢24X24����32X32����ĺ��ֿ�оƬ��֧��GB2312
// 		���꺺�֣����й����ű�ί�Ϸ���Ȩ����SCII�ַ������и�ʽΪ���ú��š��û�ͨ���ַ����룬���ñ���
// 		���ṩ�ķ�����������ַ�������оƬ�еĵ�ַ���ɴӸõ�ַ���������ַ�������Ϣ��
// 	*/

// 	/* �����ı���ʾλ�ã�ע���ı�ģʽ��д�����ͼ��ģʽ��д�����ǲ�ͬ�ļĴ��� */
// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* ����Ϊ�ı�ģʽ */

// 	/*
// 		Serial Flash/ROM Ƶ��Ƶ���趨
// 			0xb: SFCL Ƶ�� = ϵͳƵ��Ƶ��(��DMA Ϊ����״̬������ɫ�����Ϊ256 ɫ����SFCL Ƶ��
// 				�̶�Ϊ=ϵͳƵ��Ƶ��/ 2)
// 			10b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 2
// 			11b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 4

// 		������������ϵͳƵ��Ϊ 68MHz

// 		GT23L32S4W�ķ����ٶȣ�SPI ʱ��Ƶ�ʣ�20MHz(max.)

// 		�����Ҫ���� 4 ��Ƶ, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* ����SCLƵ�� */

// 	/* ѡ���ⲿ����ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # ѡ��
// 				0:ѡ��Serial Flash/ROM 0 �ӿڡ�[������RA8875�������ֿ�оƬ���� 0 �ӿ�]
// 				1:ѡ��Serial Flash/ROM 1 �ӿڡ�
// 		6	Serial Flash/ROM Ѱַģʽ
// 				0: 24 λѰַģʽ��
// 				��λ������Ϊ0��
// 		5	Serial Flash/ROM ����ģʽ
// 				0: ����ģʽ 0��
// 				1: ����ģʽ 3��
// 		4-3	Serial Flash /ROM ��ȡ���� (Read Cycle)
// 			00b: 4 bus ?? �޿����� (No Dummy Cycle)��
// 			01b: 5 bus ??1 byte �����ڡ�
// 			1Xb: 6 bus ??2 byte �����ڡ�
// 		2	Serial Flash /ROM ��ȡģʽ (Access Mode)
// 			0: ����ģʽ ��
// 			1: DMA ģʽ��
// 		1-0	Serial Flash /ROM I/F Data Latch ѡ��ģʽ
// 			0Xb: ��һģʽ��
// 			10b: ˫��ģʽ0��
// 			11b: ˫��ģʽ1��
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		�����ⲿ����оƬ�ͺ�Ϊ GT23L32S4W, ����ΪGB2312,

// 		Bit1:0 ����ASCII�ַ��ĸ�ʽ��
// 			0 = NORMAL		 [�ʻ�ϸ, �ͺ��ֶ�������]
// 			1 = Arial		 [�ʻ��֣��ͺ��ֵײ�����]
// 			2 = Roman		 [�ʻ�ϸ, �ͺ��ֵײ�����]
// 			3 = Bold		 [����,������]
// 	 */
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));

// 	s_ucRA8875Busy = 1;
// 	RA8875_REG = 0x02; 			/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */

// 	/* ��ʼѭ�������ַ� */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_RAM = *_ptr;
// 		while ((RA8875_REG & 0x80) == 0x80);
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* ��ԭΪͼ��ģʽ */
// }



/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawHLine
*	����˵��: ����һ���̶���ɫ��ˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*	��    �Σ�_usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usX2    ��������X����
*			  _usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);

	RA8875_WriteCmd(0x02); 		/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */
	for (i = 0; i < _usX2 - _usX1 + 1; i++)
	{
		RA8875_WriteData16(_usColor);
	}

}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawHColorLine
*	����˵��: ����һ����ɫˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*	��    �Σ�_usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usWidth ��ֱ�ߵĿ��
*			  _pColor : ��ɫ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);


	RA8875_WriteCmd(0x02); 		/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */

	for (i = 0; i < _usWidth; i++)
	{
		RA8875_WriteData16(*_pColor++);
	}

}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawVLine
*	����˵��: ����һ����ֱ���� ����Ҫ����UCGUI�Ľӿں�����
*	��    �Σ� _usX1    : ��ֱ�ߵ�X����
*			  _usY1    : ��ʼ��Y����
*			  _usY2    : ������Y����
*			  _usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);

	for (i = 1; i <= _usY2 - _usY1 + 1; i++)
	{
		s_ucRA8875Busy = 1;

		RA8875_WriteCmd(0x02); 		/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */
		RA8875_WriteData16(_usColor);

		RA8875_SetCursor(_usX1, _usY1 + i);
	}

}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawHColorLine
*	����˵��: ����һ����ɫˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*	��    �Σ�_usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usWidth ��ֱ�ߵĿ��
*			  _pColor : ��ɫ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{

		RA8875_DrawHColorLine(_usX1, _usY1, _usWidth, _pColor);
	

}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawLineV
*	����˵��: ����һ����ֱ���� ����Ҫ����UCGUI�Ľӿں�����
*	��    �Σ� _usX1    : ��ֱ�ߵ�X����
*			  _usY1    : ��ʼ��Y����
*			  _usY2    : ������Y����
*			  _usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLineV(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2, uint16_t _usColor)
{
	
		RA8875_DrawVLine(_usX1 , _usY1 , _usY2 , _usColor);
	

}

void LCD_DrawLineH(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2, uint16_t _usColor)
{
	
		RA8875_DrawHLine(_usX1, _usY1, _usX2, _usColor);
	
	
}

void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	
		RA8875_PutPixel(_usX, _usY, _usColor);
	

}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetPixel
*	����˵��: ��ȡ1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	
		usRGB = RA8875_GetPixel(_usX, _usY);


	return usRGB;
}

void Active_Window(u16 XL,u16 XR ,u16 YT ,u16 YB)
{
	//setting active window X
	
	WriteCommand(0x30);//HSAW0
	WriteData(XL); 
	WriteCommand(0x31);//HSAW1	   
	WriteData(XL>>8);  //��������ˮƽ��ʼ��
	RA8875_WAITSTATUS();//	
	WriteCommand(0x34);//HEAW0
	WriteData(XR);
	WriteCommand(0x35);//HEAW1	   
	WriteData(XR>>8);  //��������ˮƽ������
	RA8875_WAITSTATUS();//	
	//setting active window Y
	
	WriteCommand(0x32);//VSAW0
	WriteData(YT);  
	WriteCommand(0x33);//VSAW1	   
	WriteData(YT>>8);  //�������ڴ�ֱ��ʼ��
	RA8875_WAITSTATUS();//	
	WriteCommand(0x36);//VEAW0
	WriteData(YB); 
	WriteCommand(0x37);//VEAW1	   
	WriteData(YB>>8);  //�������ڴ�ֱ������
	RA8875_WAITSTATUS();//
}

//REG[BFh]
//input pic data quantity  for example  pic size = 800*480 = 384000
void DMA_Continuous_mode_size_setting(u32 set_size)
{ 
	WriteCommand(0xB4);
	WriteData(set_size);
	
	WriteCommand(0xB6);
	WriteData(set_size>>8);
	
	WriteCommand(0xB8);
	WriteData(set_size>>16);
}

void DMA_block_mode_size_setting(u16 BWR,u16 BHR,u16 SPWR)
{
	WriteCommand(0xB4);
	WriteData(BWR);
	WriteCommand(0xB5);
	WriteData(BWR>>8);
	
	WriteCommand(0xB6);
	WriteData(BHR);
	WriteCommand(0xB7);
	WriteData(BHR>>8);
	
	WriteCommand(0xB8);
	WriteData(SPWR);
	WriteCommand(0xB9);
	WriteData(SPWR>>8);  
}

//---------------------------------------------//
//REG[BFh]
void DMA_Continuous_mode(void)
{ 
	WriteCommand(0xBF);
	WriteData(0x00);
}

void DMA_Block_mode(void)
{ 
	WriteCommand(0xBF);
	WriteData(0x02);
}

void DMA_Start_enable(void)
{
	u8 temp;
	WriteCommand(0xBF);
	temp = ReadData();
	temp |= 0x01;
	WriteData(temp);
}

//REG[46h]~REG[49h]
void XY_Coordinate(u16 X,u16 Y)
{
	WriteCommand(0x46);
	WriteData(X);
	WriteCommand(0x47);
	WriteData(X>>8);
	
	WriteCommand(0x48);
	WriteData(Y);
	WriteCommand(0x49);
	WriteData(Y>>8);
}

void Serial_ROM_select1(void)
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp |=0x80;
	WriteData(temp);
}

void Text_Mode(void) //��������ģʽ
{
	u8 temp;	
	WriteCommand(0x40);//MWCR0
	temp = ReadData();
	temp |= 0x80 ;   
	WriteData(temp);	 
} 

 void Chk_DMA_Busy(void)
{
	u8 temp; 	
	do
	{
		WriteCommand(0xBF);
		temp = ReadData();
	}while((temp&0x01)==0x01);
}

void DMA_Start_address_setting(u32 set_address)
{ 
	WriteCommand(0xB0);
	WriteData(set_address);
	
	WriteCommand(0xB1);
	WriteData(set_address>>8);
	
	WriteCommand(0xB2);
	WriteData(set_address>>16);
	
	//WriteCommand(0xB3);
	//WriteData(set_address>>24);
}

// void SERIAL_ROM_Dual_mode0(void)	//ģʽ��˫��ģʽ0
// {
// 	u8 temp;
// 	WriteCommand(0x05);
// 	temp = ReadData();
// 	temp &=0xFC;
// 	temp |=0x02;
// 	WriteData(temp);
// }

 void SERIAL_ROM_DMA_mode(void)	 //DMAģʽ
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp |= 0x04;
	WriteData(temp);
}

//  void SERIAL_ROM_COMMON_mode(void)	 //��ͨģʽ
// {
// 	u8 temp;
// 	WriteCommand(0x05);
// 	temp = ReadData();
// 	temp &= 0xFB;
// 	WriteData(temp);
// }

void Graphic_Mode(void)		//����ͼ��ģʽ���ڴ�д�����1 �ڴ��ȡ����1
{
	WriteCommand(0x40);//MWCR0
	WriteData(0x00);

	//Cmd_Write(0x40,LCD_CmdRead(0x40)&0x7f); //GRAPHIC MODE
    RA8875_WriteReg(0x05,0xac); //ѡ��FLASH1
	RA8875_WriteReg(0x06,0x0);		//
}

void SERIAL_ROM_Font_mode(void)	  //����ģʽ
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xfb;
	WriteData(temp);
}

void bmp_dispflash(u32 bmpaddress,u16 x0,u16 y0,u16 bmp_x,u16 bmp_y)	
{

	u32 readcnt = 1024;		//һ�ζ�ȡ���ֽ���

	//u32 i;
	u32 pointcnt,bytecnt;//λͼ������
	u32 addr,addrend;
	u32 readtimes;
	u32 lastbytes; 
	u16 x1,y1;
	char mem[512];
	//FLASHBMPHEADER fbmpheader;

	addr=bmpaddress;

    //ͼ�ι���ģʽ
	RA8875_WriteReg(0x06, (2 << 0));	/* ����SCLƵ�� */
	s_ucRA8875Busy = 1;
	RA8875_REG = 0x02; 			/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */

	/* ��ʼѭ�������ַ� */


	pointcnt= bmp_x*bmp_y;
	bytecnt= pointcnt;	
	//readtimes=bytecnt/512;		 //�����ȡ�������
    //lastbytes=bytecnt%512;		 //����ʣ���ֽ���	
	x1=x0+bmp_x-1;
	y1=y0+bmp_y-1;
  DMA_Continuous_mode();	     // bfh=0
	Text_Mode();
	RA8875_REG = 0x02; 

	//Graphic_Mode();
	Active_Window(x0,x1,y0,y1);   //���ù�������   30h-37h
	XY_Coordinate(x0,y0);         //�����ڴ�д��λ��  46h-49h
	Serial_ROM_select1();         //ѡ��SPIFLASH  05h
	SERIAL_ROM_DMA_mode();
	//Select_Serial_Waveform_mode3();
	//SERIAL_ROM_Dual_mode1();
	//Serial_ROM_Address_set_32u8();
	DMA_Start_address_setting(addr);  //����DMA������Դ��ʼλ��	b0-b3h
	DMA_Continuous_mode_size_setting(pointcnt); //����DMA������Ŀ b4 b6 b8
	//Serial_ROM_direct_access_mode_enable();
	//WriteCommand(0xBF);
	//WriteData(0x01);

	//
	DMA_Start_enable();
 	//LcdPrintStr("�����DMA",10,100,RED,WHITE,1);

	Chk_DMA_Busy();
	//LcdPrintStr("������ʾ",10,130,RED,WHITE,1);
	//DMA_Continuous_mode();//
	//DMA_Start_enable();

		s_ucRA8875Busy = 0;
 }


// void RA8875_SetTextMode(uint8_t Mode)
// {
// 	/*
// 		pdf ��22ҳ		[22H]
// 		bit7 ���ڶ��룬һ�㲻�ã�ȱʡ��0
// 		bit6 ����ͨ͸��һ�㲻�ã�ȱʡ��0
// 		bit4 ������ת90����һ�㲻�ã�ȱʡ��0
// 		bit3-2 ˮƽ�Ŵ���
// 		bit1-0 ��ֱ�Ŵ���
// 	*/
// 	RA8875_WriteReg(0x22, (Mode<<6));
// }
//  u16 GUIColorToRGB(LCD_COLOR color)
// {
// 	u32 Color_R;
// 	u32 Color_G;
// 	u32 Color_B;
// 	u32 tempcolor;
// 	tempcolor=color;
// 	Color_B=(tempcolor&0x00f80000)>>19;//ȡ��5BIT
// 	Color_G=(tempcolor&0x0000fc00)>>5;
// 	Color_R=(tempcolor&0x000000f8)<<8;
// 	
// 	return((u16)((Color_R)|(Color_G)|Color_B));
// 	
// }
//  /*
// *********************************************************************************************************
// *	�� �� ��: RA8875_DispStr
// *	����˵��: ��ʾ�ַ������ַ�����������RA8875��ӵ��ֿ�оƬ��֧�ֺ���.
// *			������ɫ��������ɫ���Ƿ�ͨ͸�������ĺ�����������
// *			void RA8875_SetFrontColor(uint16_t _usColor);
// *			void RA8875_SetBackColor(uint16_t _usColor);
// *	��    �Σ�_usX, _usY : ��ʾλ�ã����꣩
// *			 _ptr : AascII�ַ�������0����
// *	�� �� ֵ: ��
// *********************************************************************************************************
// */
// void RA8875_DispChar(unsigned short c)
// {
// 	u8 i=0;
// 	unsigned char ch[2];
// 	ch[0]=(c>>8)&0x00ff;
// 	ch[1]=c&0xff;
// 	/*
// 		RA8875 ͸��ʹ�� �� �Ϻ���ͨ��˾ (Genitop Inc) �ⲿ����ʽ�����ڴ� (Font ROM)����֧�ָ���
// 		������д�뵽DDRAM �С�RA8875 ���Ϻ���ͨ��˾ (Genitop Inc) ���ݵĲ�Ʒ���� :
// 		GT21L16TW/GT21H16T1W ��GT23L16U2W ��GT23L24T3Y/GT23H24T3Y ��GT23L24M1Z ��
// 		�� GT23L32S4W/GT23H32S4W����Щ�������16x16, 24x24, 32x32 �� (Dot) �벻ͬ����

// 		������RA8875�����弯�ɵ��ֿ�оƬΪ ��ͨ�ֿ�оƬ_GT23l32S4W

// 		GT23L32S4W��һ���ں�11X12����15X16�㡢24X24����32X32����ĺ��ֿ�оƬ��֧��GB2312
// 		���꺺�֣����й����ű�ί�Ϸ���Ȩ����SCII�ַ������и�ʽΪ���ú��š��û�ͨ���ַ����룬���ñ���
// 		���ṩ�ķ�����������ַ�������оƬ�еĵ�ַ���ɴӸõ�ַ���������ַ�������Ϣ��
// 	*/

// 	/* �����ı���ʾλ�ã�ע���ı�ģʽ��д�����ͼ��ģʽ��д�����ǲ�ͬ�ļĴ��� */
// 	//RA8875_SetTextCursor(_usX, _usY);
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	RA8875_WriteReg(0x40, (1 << 7));	/* ����Ϊ�ı�ģʽ */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/*
// 		Serial Flash/ROM Ƶ��Ƶ���趨
// 			0xb: SFCL Ƶ�� = ϵͳƵ��Ƶ��(��DMA Ϊ����״̬������ɫ�����Ϊ256 ɫ����SFCL Ƶ��
// 				�̶�Ϊ=ϵͳƵ��Ƶ��/ 2)
// 			10b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 2
// 			11b: SFCL Ƶ�� =ϵͳƵ��Ƶ��/ 4

// 		

// 		GT23L32S4W�ķ����ٶȣ�SPI ʱ��Ƶ�ʣ�20MHz(max.)

// 		�����Ҫ���� 4 ��Ƶ, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* ����Ϊ�ı�ģʽ */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* ѡ���ⲿ����ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # ѡ��
// 				0:ѡ��Serial Flash/ROM 0 �ӿڡ�[��Ϊ�Ƽ�RA8875�������ֿ�оƬ���� 0 �ӿ�]
// 				1:ѡ��Serial Flash/ROM 1 �ӿڡ�
// 		6	Serial Flash/ROM Ѱַģʽ
// 				0: 24 λѰַģʽ��
// 				��λ������Ϊ0��
// 		5	Serial Flash/ROM ����ģʽ
// 				0: ����ģʽ 0��
// 				1: ����ģʽ 3��
// 		4-3	Serial Flash /ROM ��ȡ���� (Read Cycle)
// 			00b: 4 bus ?? �޿����� (No Dummy Cycle)��
// 			01b: 5 bus ??1 byte �����ڡ�
// 			1Xb: 6 bus ??2 byte �����ڡ�
// 		2	Serial Flash /ROM ��ȡģʽ (Access Mode)
// 			0: ����ģʽ ��
// 			1: DMA ģʽ��
// 		1-0	Serial Flash /ROM I/F Data Latch ѡ��ģʽ
// 			0Xb: ��һģʽ��
// 			10b: ˫��ģʽ0��
// 			11b: ˫��ģʽ1��
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		�����ⲿ����оƬ�ͺ�Ϊ GT23L32S4W, ����ΪGB2312,

// 		Bit1:0 ����ASCII�ַ��ĸ�ʽ��
// 			0 = NORMAL		 [�ʻ�ϸ, �ͺ��ֶ�������]
// 			1 = Arial		 [�ʻ��֣��ͺ��ֵײ�����]
// 			2 = Roman		 [�ʻ�ϸ, �ͺ��ֵײ�����]
// 			3 = Bold		 [����,������]
// 	 */
// 	 //while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (0 << 0));
//   RA8875_WriteReg(0x22, 0<<6);//����ͨ͸
// 	//s_ucRA8875Busy = 1;
// 	RA8875_WriteCmd(0x02); 			/* �����趨RA8875 �����ڴ�(DDRAM��CGRAM)��ȡ/д��ģʽ */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* ��ʼѭ�������ַ� */
// 	for(i=0;i<2;i++)
// 	{
// 		//RA8875_WriteData("��");
// 		//ch[0]="��";
// 		RA8875_WriteData(ch[i]);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);

// 		//i++;
// 	}
// 	//s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* ��ԭΪͼ��ģʽ */
// 	while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	
// 	while((RA8875_ReadReg(0xf0)&0x01)==0x01) ;
// }



