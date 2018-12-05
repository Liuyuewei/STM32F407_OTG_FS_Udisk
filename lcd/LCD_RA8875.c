/*
*********************************************************************************************************
*
*	模块名称 : RA8875芯片驱动模块
*	文件名称 : LCD_RA8875.c
*	版    本 : V1.1
*	说    明 : RA8875底层驱动函数集。
*	修改记录 :
*		版本号  日期   
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







static __IO uint16_t s_usTouchX, s_usTouchY;	/* 触摸屏ADC值 */
__IO uint8_t s_ucRA8875Busy = 0;

/* 保存当前显示窗口的位置和大小，这几个变量由 RA8875_SetDispWin() 进行设置 */
static __IO uint16_t s_WinX = 0;
static __IO uint16_t s_WinY = 0;
static __IO uint16_t s_WinHeight = 272;
static __IO uint16_t s_WinWidth = 480;

 



static void BTE_SetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer);
static void BTE_SetOperateCode(uint8_t _ucOperate);

static void RA8875_Delaly1us(void)
{
	uint16_t i;

	for (i = 0; i < 1670; i++);	/* 延迟, 不准 */
}

static void RA8875_Delaly1ms(void)
{
	uint16_t i;

	for (i = 0; i < 16700; i++);	/* 延迟1us, 不准 */
}


/*******************液晶屏硬件连接**********************

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
 * 函数名：LCD_GPIO_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的GPIO管脚与FSMC连接
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
	
//     GPIO_InitTypeDef GPIO_InitStructure;		                                              //定义GPIO初始化结构体
//      
//     /* 使能FSMC的AHB3时钟，这里注意，不同芯片总线会不同 */
//     RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
//     
//     /* 使能GPIOB，GPIOD，GPIOE的AHB1时钟*/
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
//                          RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH , ENABLE);
//    	 /*配置液晶屏的背光控制管脚*/
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz，M4就是快啊
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //设置输出类型为推挽输出
// 	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //设置上/下拉电阻为无上拉下拉电阻		

//     /* 配置基于FSMC的液晶屏的数据线
// 	 * 液晶屏数据线：FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
// 	 */	
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //设置管脚模式为复用模式
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | 
//                                   GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
//     GPIO_Init(GPIOD, &GPIO_InitStructure);													  //初始化GPIOB的Pin_0，Pin_，Pin_2，
// 	                                                                    
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
//                                   GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
//                                   GPIO_Pin_15;
//     GPIO_Init(GPIOE, &GPIO_InitStructure); 
//     
//     /* 配置基于FSMC的液晶屏控制线
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
//     GPIO_AF_FSMC_Config();				             //在F407中必须加上这个复用连接配置，否则不能连接到FSMC
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
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC); //PD7配置成FSMC脚 csf
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
 * 函数名：LCD_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	                                              //定义GPIO初始化结构体
	  
  	FSMC_NORSRAMTimingInitTypeDef  p;                                                 
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 2;                                           //地址建立时间
  	p.FSMC_AddressHoldTime = 0;                                            //地址保持时间
  	p.FSMC_DataSetupTime =1;                                              //数据建立时间
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
*	函 数 名: RA8875_WriteCmd
*	功能说明: 写RA8875指令寄存器
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_WriteCmd(uint8_t _ucRegAddr)
{
	RA8875_REG = _ucRegAddr;	/* 设置寄存器地址 */
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_WriteData
*	功能说明: 写RA8875指令寄存器
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_WriteData(uint8_t _ucRegValue)
{
	RA8875_RAM = _ucRegValue;	/* 设置寄存器地址 */
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_ReadData
*	功能说明: 读RA8875寄存器值
*	形    参: 无
*	返 回 值: 寄存器值
*********************************************************************************************************
*/
uint8_t RA8875_ReadData(uint8_t _ucRegAddr)
{
	uint8_t value;
	value = RA8875_RAM;		/* 读取寄存器值 */
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_WriteData16
*	功能说明: 写RA8875数据总线，16bit，用于RGB显存写入
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void RA8875_WriteData16(uint16_t _usRGB)
{
	RA8875_RAM = _usRGB;	/* 设置寄存器地址 */
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_ReadData16
*	功能说明: 读RA8875显存，16bit RGB
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t RA8875_ReadData16(void)
{
	uint16_t value;
	value = RA8875_RAM;		/* 读取寄存器值 */
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_WriteReg
*	功能说明: 写RA8875寄存器. RA8875的寄存器地址和数据都是8bit的
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_WriteReg(uint8_t _ucRegAddr, uint8_t _ucRegValue)
{
	s_ucRA8875Busy = 1;
	RA8875_REG = _ucRegAddr;	/* 设置寄存器地址 */
	RA8875_RAM = _ucRegValue;	/* 写入寄存器值 */
	s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_WriteReg
*	功能说明: 写RA8875寄存器. RA8875的寄存器地址和数据都是8bit的
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 uint8_t RA8875_ReadReg(uint8_t _ucRegAddr)
{
	uint8_t value;
	RA8875_REG = _ucRegAddr;/* 设置寄存器地址 */
	value = RA8875_RAM;		/* 读取寄存器值 */

	return value;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_ReadStatus
*	功能说明: 读RA8875状态寄存器
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: RA8875_ReadID
*	功能说明: 读取LCD驱动芯片ID
*	形    参：无
*	返 回 值: 无
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

//RA8875等待空闲
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
// 	RA8875_REG = _ucRegAddr;/* 设置寄存器地址 */
// 	value = RA8875_RAM;		/* 读取寄存器值 */
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
// //使能触摸屏中断
// void Touch_Panel_Interrupt_Enable(void)
// {
// // 	u8 temp;
// // 	RA8875_WriteCmd(0xF0);//INTC
// // 	temp = RA8875_ReadStatus();
// // 	temp |= 0x04;		 //0000 0100 开启触摸屏中断
// // 	RA8875_WriteCmd(temp);
// // 	RA8875_WAITSTATUS();
// 		u8 temp;
// 	WriteCommand(0xF0);//INTC
// 	temp = ReadData();
// 	temp |= 0x04;		 //0000 0100 开启触摸屏中断
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
// //触摸屏初始化

// void TOUCH_InitHard()
// {

// // 	RA8875_WriteReg(0x70, 0xB4);//ADC频率为  100b : 系统频率 / 16
// // 	RA8875_WAITSTATUS();
// // 	RA8875_WriteReg(0x71, 0x00);//0 : 自动模式。0 : 内部产生参考电压。
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
// 			//temp |= 0x3F;               //外部基准，0x1f内部基准
// 			WriteData(temp);
// 			RA8875_WAITSTATUS();

// 		TPCR1 71H
// 		7   N/A     
// 		6   0自动   1手动   
// 		5   0内参考 1外参考
// 		43  N/A
// 		2   0关消弹 1开消弹 
// 		10  手动模式选择位 00闲置 01侦察		         
// 		*/


// 		WriteCommand(0x70);		//set TP sample time  & ADC clock 	TPCR0
// 		WriteData(0xb3);//0xb3 0xa2		开启1 AD采样4096个周期011 关闭触发事件唤醒模式0 触控面板控制器ADC频率设定系统CLK/8 011
// 		//1                    1 0 1                0                            011
// 		//bit7 开启1 
// 		//             AD采样4096个周期011 
// 		//                                   关闭触发事件唤醒模式0 
// 		//                                                              触控面板控制器ADC频率设定系统CLK/8 011
// 		RA8875_WAITSTATUS();
// 		//Enable_TP(); // I add it 		not need	

// 		Touch_Panel_Interrupt_Enable();	// demo code using Auto mode + polling 8Fh to confirm touch
// 		clear_TP_interrupt();

// 		//Active_Window(100,400,100,400);
// //		Active_Window(0,479,0,271);	//设定面板分辨率为480*272 pixel
// 		//HDIR_SEGn_SEG0();		//让屏幕显示颠倒90度
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
*	函 数 名: RA8875_InitHard
*	功能说明: 初始化RA8875驱动芯片
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_InitHard(void)
{

  LCD_GPIO_Config();		
	LCD_FSMC_Config();
	RA8875_Delaly1ms();
	RA8875_Delaly1ms();
	temp_ID=RA8875_ReadID();

	
	/* 初始化PLL.  晶振频率为25M */
	RA8875_WriteCmd(0x88);
	RA8875_Delaly1us();		/* 延迟1us */
	RA8875_WriteData(11);	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 12 */
	RA8875_Delaly1ms();
	RA8875_WriteCmd(0x89);
	RA8875_Delaly1us();		/* 延迟1us */
	RA8875_WriteData(2);//RA8875_WriteData(3);//RA8875_WriteData(3);	/* PLLDIVK[2:0] = 2, 除以4 */
		/* RA8875 的内部系统频率 (SYS_CLK) 是结合振荡电路及PLL 电路所产生，频率计算公式如下 :
		  SYS_CLK = FIN * ( PLLDIVN [4:0] +1 ) / ((PLLDIVM+1 ) * ( 2^PLLDIVK [2:0] ))
		          = 25M * (11 + 1) / ((0 + 1) * (2 ^ 2))
				  = 75MHz
		*/
		/* REG[88h]或REG[89h]被设定后，为保证PLL 输出稳定，须等待一段「锁频时间」(< 100us)。*/
	RA8875_Delaly1ms();
	
//	PWM1_enable();
	
//	RA8875_SetBackLight(1);//背光设置成最大亮度 csf
//RA8875_SetBackLight(1);//背光
RA8875_SetBackLight(128);//背光

	
		/*
			配置系统控制寄存器。 中文pdf 第18页:

			bit3:2 色彩深度设定 (Color Depth Setting)
				00b : 8-bpp 的通用TFT 接口， i.e. 256 色。
				1xb : 16-bpp 的通用TFT 接口， i.e. 65K 色。	 【选这个】

			bit1:0 MCUIF 选择
				00b : 8-位MCU 接口。
				1xb : 16-位MCU 接口。 【选这个】
		*/
	RA8875_WriteReg(0x10, (1 <<3 ) | (1 << 1));	/* 配置16位MCU并口，65K色 */
		/* REG[04h] Pixel Clock Setting Register (PCSR)
			bit7  PCLK Inversion
				0 : PDAT 是在PCLK 正缘上升 (Rising Edge) 时被取样。
				1 : PDAT 是在PCLK 负缘下降 (Falling Edge) 时被取样。
			bit1:0 PCLK 频率周期设定
				Pixel Clock ,PCLK 频率周期设定。
				00b: PCLK 频率周期= 系统频率周期。
				01b: PCLK 频率周期= 2 倍的系统频率周期。
				10b: PCLK 频率周期= 4 倍的系统频率周期。
				11b: PCLK 频率周期= 8 倍的系统频率周期。
		*/
	RA8875_WriteReg(0x04, 0x80); /* 设置PCLK反相 */
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


	  /* LCD 显示/关闭讯号 (LCD Display on) */
	RA8875_WriteReg(0x01, 0x80);

	/* 	REG[40h] Memory Write Control Register 0 (MWCR0)

		Bit 7	显示模式设定
			0 : 绘图模式。
			1 : 文字模式。

		Bit 6	文字写入光标/内存写入光标设定
			0 : 设定文字/内存写入光标为不显示。
			1 : 设定文字/内存写入光标为显示。

		Bit 5	文字写入光标/内存写入光标闪烁设定
			0 : 游标不闪烁。
			1 : 游标闪烁。

		Bit 4   NA

		Bit 3-2  绘图模式时的内存写入方向
			00b : 左 -> 右，然后上 -> 下。
			01b : 右 -> 左，然后上 -> 下。
			10b : 上 -> 下，然后左 -> 右。
			11b : 下 -> 上，然后左 -> 右。

		Bit 1 	内存写入光标自动增加功能设定
			0 : 当内存写入时光标位置自动加一。
			1 : 当内存写入时光标位置不会自动加一。

		Bit 0 内存读取光标自动增加功能设定
			0 : 当内存读取时光标位置自动加一。
			1 : 当内存读取时光标位置不会自动加一。
	*/
	RA8875_WriteReg(0x40, 0x00);	/* 选择绘图模式 */
	/* 	REG[41h] Memory Write Control Register1 (MWCR1)
		写入目的位置，选择图层1
	*/
	RA8875_WriteReg(0x41, 0x00);	/* 选择绘图模式, 目的为CGRAM */

	RA8875_SetDispWin(0, 0, 480, 800);
	
	RA8875_DispOn();
	
	//触摸屏初始化
	//TOUCH_InitHard();

}


// //读取触摸屏中断
// u8 RA8875_ReadTP(void)
// {

// 	return RA8875_ReadReg(0xF1);

// }
/*
*********************************************************************************************************
*	函 数 名: BTE_SetTarBlock
*	功能说明: 设置RA8875 BTE目标区块以及目标图层
*	形    参:
*			uint16_t _usX : 水平起点坐标
*			uint16_t _usY : 垂直起点坐标
*			uint16_t _usHeight : 区块高度
*			uint16_t _usWidth : 区块宽度
*			uint8_t _ucLayer ： 0 图层1； 1 图层2
*	返 回 值: 无
*********************************************************************************************************
*/
// static void BTE_SetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer)
// {
// 	/* 设置起点坐标 */
// 	RA8875_WriteReg(0x58, _usX);
// 	RA8875_WriteReg(0x59, _usX >> 8);

// 	RA8875_WriteReg(0x5A, _usY);
// 	if (_ucLayer == 0)	/* 图层2 */
// 	{
// 		RA8875_WriteReg(0x5B, _usY >> 8);
// 	}
// 	else
// 	{
// 		RA8875_WriteReg(0x5B, (1 << 7) | (_usY >> 8));	/* Bit7 表示图层， 0 图层1； 1 图层2*/
// 	}

// 	/* 设置区块宽度 */
// 	RA8875_WriteReg(0x5C, _usWidth);
// 	RA8875_WriteReg(0x5D, _usWidth >> 8);

// 	/* 设置区块高度 */
// 	RA8875_WriteReg(0x5E, _usHeight);
// 	RA8875_WriteReg(0x5F, _usHeight >> 8);
// }

// /*
// *********************************************************************************************************
// *	函 数 名: BTE_SetOperateCode
// *	功能说明: 设定BTE 操作码和光栅运算码
// *	形    参: _ucOperate : 操作码
// *	返 回 值: 无
// *********************************************************************************************************
// */
// static void BTE_SetOperateCode(uint8_t _ucOperate)
// {
// 	/*  设定BTE 操作码和光栅运算码  */
// 	RA8875_WriteReg(0x51, _ucOperate);
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_SetFrontColor
// *	功能说明: 设定前景色
// *	形    参: 颜色
// *	返 回 值: 无
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
*	函 数 名: RA8875_SetBackColor
*	功能说明: 设定背景色
*	形    参: 颜色
*	返 回 值: 无
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
*	函 数 名: BTE_Start
*	功能说明: 启动BTE操作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
// */
// void BTE_Start(void)
// {
// 	s_ucRA8875Busy = 1;
// 	/* RA8875_WriteReg(0x50, 0x80);  不能使用这个函数，因为内部已经操作了 s_ucRA8875Busy 标志 */
// 	RA8875_WriteCmd(0x50);	/* 设置寄存器地址 */
// 	RA8875_WriteData(0x80);	/* 写入寄存器值 */
// }

/*
*********************************************************************************************************
*	函 数 名: BTE_Wait
*	功能说明: 等待BTE操作结束
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// void BTE_Wait(void)
// {
// 	while ((RA8875_ReadStatus() & 0x40) == 0x40);
// 	s_ucRA8875Busy = 0;
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_IsBusy
*	功能说明: RA8875是否忙
*	形    参: 无
*	返 回 值: 1 表示忙； 0 表示空闲
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
*	函 数 名: RA8875_WaitBusy
*	功能说明: 等待RA8875空闲
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_WaitBusy(void)
// {
// 	while ((RA8875_ReadStatus() & 0x80) == 0x80);
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_Layer1Visable
*	功能说明: RA8875 图层1显示打开
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_Layer1Visable(void)
// {
// 	/* 0x52 寄存器的 Bit2:0
// 		图层显示模式
// 		000b : 只有图层1 显示。
// 		001b : 只有图层2 显示。
// 		010b : 显示图层1 与图层2 的渐进/渐出模式。
// 		011b : 显示图层1 与图层2 的通透模式。
// 		100b : Boolean OR。
// 		101b : Boolean AND。
// 		110b : 浮动窗口模式 (Floating window mode)。
// 		111b :保留。
// 	*/
// 	RA8875_WriteReg(0x52, RA8875_ReadReg(0x52) & 0xF8);	/* 只有图层1 显示 */
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_Layer2Visable
*	功能说明: 只显示图层2
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_Layer2Visable(void)
{
	/* 0x52 寄存器的 Bit2:0
		图层显示模式
		000b : 只有图层1 显示。
		001b : 只有图层2 显示。
		010b : 显示图层1 与图层2 的渐进/渐出模式。
		011b : 显示图层1 与图层2 的通透模式。
		100b : Boolean OR。
		101b : Boolean AND。
		110b : 浮动窗口模式 (Floating window mode)。
		111b :保留。
	*/
	RA8875_WriteReg(0x52, (RA8875_ReadReg(0x52) & 0xF8) | 0x01);	/* 只有图层2 显示 */
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_TouchInit
*	功能说明: 初始化触摸板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_TouchInit(void)
// {
// 	RA8875_WriteReg(0xF0, (1 << 2));	/* 开启触控面板中断位 */

// 	/*
// 		REG[71h] Touch Panel Control Register 1 (TPCR1)

// 		Bit7 N/A
// 		Bit6 触控面板模式设定
// 			0 : 自动模式。
// 			1 : 手动模式。
// 		Bit5 触控面板控制器ADC 参考电压(Vref)来源设定
// 			0 : 内部产生参考电压。
// 			1 : 外部输入参考电压，ADC 参考电压准位= 1/2 VDD。
// 		Bit4-3 N/A
// 		Bit2 触控中断讯号的消除弹跳电路选择
// 			0: 关闭消除弹跳电路。
// 			1: 开启消除弹跳电路。
// 		Bit1-0 触控面板手动模式之选择位
// 			00b : 闲置模式。触控控制单元进入闲置模式。
// 			01b : 侦测触摸事件发生。在此模式控制器会侦测触摸事件的发
// 				生，事件发生可以引发中断或是由缓存器得知(REG[F1h]
// 				Bit2)。
// 			10b : X 轴数据撷取模式。在此模式触摸位置的X 轴数据会被储
// 				存至 REG[72h] 和REG[74h]。
// 			11b : Y 轴数据撷取模式。在此模式触摸位置的Y 轴数据会被储
// 				存至REG[73h] and REG[74h]。
// 	*/
// 	RA8875_WriteReg(0x71, (0 << 6) | (0 << 5) | (1 << 2));	/* 选择自动模式 */

// 	/*
// 		REG[70h] Touch Panel Control Register 0 (TPCR0)

// 		Bit7 触控面板功能设定
// 			0 : 关闭。
// 			1 : 开启。
// 		Bit6-4 触控面板控制器取样时间设定
// 			000b : ADC 取样时间为512 个系统频率周期。
// 			001b : ADC 取样时间为 1024 个系统频率周期。
// 			010b : ADC 取样时间为 2048 个系统频率周期。
// 			011b : ADC 取样时间为 4096 个系统频率周期。
// 			100b : ADC 取样时间为 8192 个系统频率周期。
// 			101b : ADC 取样时间为 16384 个系统频率周期。
// 			110b : ADC 取样时间为 32768 个系统频率周期。
// 			111b : ADC 取样时间为65536 个系统频率周期。
// 		Bit3 触控面板唤醒模式
// 			0 : 关闭触控事件唤醒模式。
// 			1 : 触控事件可唤醒睡眠模式。
// 		Bit2-0 触控面板控制器ADC 频率设定
// 			000b : 系统频率。
// 			001b : 系统频率 / 2。
// 			010b : 系统频率 / 4。
// 			011b : 系统频率 / 8。
// 			100b : 系统频率 / 16。
// 			101b : 系统频率 / 32。
// 			110b : 系统频率 / 64。
// 			111b : 系统频率 / 128。

// 		注 : ADC 的输入频率设定不能超过10MHz。
// 	*/
// 	RA8875_WriteReg(0x70, (1 << 7) | (3 << 4) | (0 << 3) | (2 << 0));	/* 开启触摸功能； */
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_TouchReadX
// *	功能说明: 读取X位置ADC   （这个函数被定时中断服务程序调用，需要避免寄存器访问冲突 )
// *	形    参: 无
// *	返 回 值: ADC值，10Bit
// *********************************************************************************************************
// */
// uint16_t RA8875_TouchReadX(void)
// {
// 	uint16_t usAdc;
// 	uint8_t ucRegValue;
// 	uint8_t ucReg74;

// 	/* 软件读取中断事件标志 */
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
// 		bit2 写入功能?? 触控面板中断清除位
// 		0 : 未操作。
// 		1 : 清除触控面板中断。
// 		读取功能?? 触控面板中断状态
// 		0 : 未发生触控面板中断。
// 		1 : 发生触控面板中断。
// 	*/

// 	/*
// 		不要调用这个函数写寄存器，因为该函数改写了busy标志
// 		RA8875_WriteReg(0xF1, (1 << 2));	 必须清除， 才会下次采样.
// 	*/
// 	RA8875_WriteCmd(0xF1);
// 	RA8875_WriteData(1 << 2);

// 	return s_usTouchX;
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_TouchReadY
*	功能说明: 读取Y位置ADC ； 必须先调用 RA8875_TouchReadX()
*	形    参: 无
*	返 回 值: ADC值，10Bit
*********************************************************************************************************
*/
uint16_t RA8875_TouchReadY(void)
{
	return s_usTouchY;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_DispOn
*	功能说明: 打开显示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DispOn(void)
{
	RA8875_WriteReg(0x01, 0x80);
	//RA8875_WriteReg(0x8A, 1<<6);	//背光设置成最亮 屏蔽csf
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_DispOff
*	功能说明: 关闭显示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_DispOff(void)
// {
// 	RA8875_WriteReg(0x01, 0x00);
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_PutPixel
*	功能说明: 画1个像素
*	形    参：
*			_usX,_usY : 像素坐标
*			_usColor  ：像素颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	RA8875_SetCursor(_usX, _usY);	/* 设置光标位置 */

	s_ucRA8875Busy = 1;

	RA8875_WriteCmd(0x02); 		/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
	RA8875_WriteData16(_usColor);
	s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_GetPixel
*	功能说明: 读取1个像素
*	形    参：
*			_usX,_usY : 像素坐标
*			_usColor  ：像素颜色
*	返 回 值: RGB颜色值
*********************************************************************************************************
// */
uint16_t RA8875_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	RA8875_SetReadCursor(_usX, _usY);	/* 设置读取光标位置 */

	s_ucRA8875Busy = 1;

	RA8875_WriteCmd(0x02);
	usRGB = RA8875_ReadData16();

	s_ucRA8875Busy = 0;

	return usRGB;
}

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_ClrScr
// *	功能说明: 根据输入的颜色值清屏.RA8875支持硬件单色填充。该函数仅对当前激活的显示窗口进行清屏. 显示
// *			 窗口的位置和大小由 RA8875_SetDispWin() 函数进行设置
// *	形    参：_usColor : 背景色
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_ClrScr(uint16_t _usColor)
// {
// 	/* 也可以通过0x30-0x37寄存器获得获得当前激活的显示窗口 */

// 	/* 单色填满功能, 中文pdf 第162页
// 	此功能使用于将选定特定区域画面清除或是塡入给定某种前景色，R8875 填入的单色设定为BTE 前景色。

// 	操作步骤：
// 		1. 设定目的图层和位置 REG[58h], [59h], [5Ah], [5Bh]
// 		2. 设定BTE 宽度和高度 REG[5Ch], [5Dh], [5Eh], [5Fh]
// 		3. 设定BTE 操作码和光栅运算码  REG[51h] Bit[3:0] = 0Ch
// 		4. 设定前景色  REG[63h], [64h], [65h]
// 		5. 开启BTE 功能  REG[50h] Bit7 = 1
// 		6. 检查状态缓存器 STSR Bit6，确认BTE 是否完成
// 	*/
// 	BTE_SetTarBlock(s_WinX, s_WinY, s_WinHeight, s_WinWidth, 0);	/* 设置BTE位置和宽度高度以及目标图层（0或1） */
// 	BTE_SetOperateCode(0x0C);		/* 设定BTE 操作码和光栅运算码  REG[51h] Bit[3:0] = 0Ch */
// 	RA8875_SetFrontColor(_usColor);	/* 设置BTE前景色 */
// 	BTE_Start();					/* 开启BTE 功能 */
// 	BTE_Wait();						/* 等待操作结束 */
// 	
// // 	RA8875_Delaly1ms();
// // 	BTE_SetTarBlock(s_WinX, s_WinY, s_WinHeight, s_WinWidth, 1);	/* 设置BTE位置和宽度高度以及目标图层（0或1） */
// // 	BTE_SetOperateCode(0x0C);		/* 设定BTE 操作码和光栅运算码  REG[51h] Bit[3:0] = 0Ch */
// // 	RA8875_SetFrontColor(_usColor);	/* 设置BTE前景色 */
// // 	BTE_Start();					/* 开启BTE 功能 */
// // 	BTE_Wait();						/* 等待操作结束 */
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_DrawBMP
*	功能说明: 在LCD上显示一个BMP位图，位图点阵扫描次序：从左到右，从上到下
*	形    参：
*			_usX, _usY : 图片的坐标
*			_usHeight  ：图片高度
*			_usWidth   ：图片宽度
*			_ptr       ：图片点阵指针
*	返 回 值: 无
*********************************************************************************************************
// */
// void RA8875_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
// {
// 	uint32_t index = 0;
// 	const uint16_t *p;

// 	/* 设置图片的位置和大小， 即设置显示窗口 */
// 	RA8875_SetDispWin(_usX, _usY, _usHeight, _usWidth);

// 	s_ucRA8875Busy = 1;

// 	RA8875_WriteCmd(0x02); 		/* 准备读写显存 */

// 	p = _ptr;
// 	for (index = 0; index < _usHeight * _usWidth; index++)
// 	{
// 		/*
// 			armfly : 进行优化, 函数就地展开
// 			RA8875_WriteRAM(_ptr[index]);

// 			此处可考虑用DMA操作
// 		*/
// 		RA8875_WriteData16(*p++);
// 	}
// 	s_ucRA8875Busy = 0;

// 	/* 退出窗口绘图模式 */
// 	RA8875_QuitWinMode();
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_DrawLine
// *	功能说明: 采用RA8875的硬件功能，在2点间画一条直线。
// *	形    参：
// *			_usX1, _usY1 ：起始点坐标
// *			_usX2, _usY2 ：终止点Y坐标
// *			_usColor     ：颜色
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
// {
// 	/* pdf 第131页
// 		RA8875 支持直线绘图功能，让使用者以简易或低速的MCU 就可以在TFT 模块上画直线。先设
// 		定直线的起始点REG[91h~94h] 与结束点REG[95h~98h]，直线的颜色REG[63h~65h]，然后启
// 		动绘图设定REG[90h] Bit4 = 0, Bit0=0 且REG[90h] Bit7 = 1，RA8875 就会将直线的图形写入
// 		DDRAM，相对的在TFT 模块上就可以显示所画的直线。
// 	*/

// 	/* 设置起点坐标 */
// 	RA8875_WriteReg(0x91, _usX1);
// 	RA8875_WriteReg(0x92, _usX1 >> 8);
// 	RA8875_WriteReg(0x93, _usY1);
// 	RA8875_WriteReg(0x94, _usY1 >> 8);

// 	/* 设置终点坐标 */
// 	RA8875_WriteReg(0x95, _usX2);
// 	RA8875_WriteReg(0x96, _usX2 >> 8);
// 	RA8875_WriteReg(0x97, _usY2);
// 	RA8875_WriteReg(0x98, _usY2 >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* 设置直线的颜色 */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (0 << 4) | (0 << 0));	/* 开始画直线 */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));				/* 等待结束 */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_DrawRect
// *	功能说明: 采用RA8875硬件功能绘制矩形
// *	形    参：
// *			_usX,_usY：矩形左上角的坐标
// *			_usHeight ：矩形的高度
// *			_usWidth  ：矩形的宽度
// *			_usColor  ：颜色
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
// {
// 	/*
// 		RA8875 支持方形绘图功能，让使用者以简易或低速的MCU 就可以在TFT 模块上画方形。先设
// 	定方形的起始点REG[91h~94h]与结束点REG[95h~98h]，方形的颜色REG[63h~65h]，然后启
// 	动绘图设定REG[90h] Bit4=1, Bit0=0 且REG[90h] Bit7 = 1，RA8875 就会将方形的图形写入
// 	DDRAM，相对的在TFT 模块上就可以显示所画的方形。若设定REG[90h] Bit5 = 1，则可画出一
// 	实心方形 (Fill)

// 	 ---------------->---
// 	|(_usX，_usY)        |
// 	V                    V  _usHeight
// 	|                    |
// 	 ---------------->---
// 		  _usWidth
// 	*/

// 	/* 设置起点坐标 */
// 	RA8875_WriteReg(0x91, _usX);
// 	RA8875_WriteReg(0x92, _usX >> 8);
// 	RA8875_WriteReg(0x93, _usY);
// 	RA8875_WriteReg(0x94, _usY >> 8);

// 	/* 设置终点坐标 */
// 	RA8875_WriteReg(0x95, _usX + _usWidth - 1);
// 	RA8875_WriteReg(0x96, (_usX + _usWidth - 1) >> 8);
// 	RA8875_WriteReg(0x97, _usY + _usHeight - 1);
// 	RA8875_WriteReg(0x98, (_usY + _usHeight - 1) >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* 设置颜色 */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (0 << 5) | (1 << 4) | (0 << 0));	/* 开始画矩形  */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));							/* 等待结束 */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_FillRect
// *	功能说明: 采用RA8875硬件功能填充一个矩形为单色
// *	形    参：
// *			_usX,_usY：矩形左上角的坐标
// *			_usHeight ：矩形的高度
// *			_usWidth  ：矩形的宽度
// *			_usColor  ：颜色
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
// {
// 	/*
// 		RA8875 支持方形绘图功能，让使用者以简易或低速的MCU 就可以在TFT 模块上画方形。先设
// 	定方形的起始点REG[91h~94h]与结束点REG[95h~98h]，方形的颜色REG[63h~65h]，然后启
// 	动绘图设定REG[90h] Bit4=1, Bit0=0 且REG[90h] Bit7 = 1，RA8875 就会将方形的图形写入
// 	DDRAM，相对的在TFT 模块上就可以显示所画的方形。若设定REG[90h] Bit5 = 1，则可画出一
// 	实心方形 (Fill)

// 	 ---------------->---
// 	|(_usX，_usY)        |
// 	V                    V  _usHeight
// 	|                    |
// 	 ---------------->---
// 		  _usWidth
// 	*/

// 	/* 设置起点坐标 */
// 	RA8875_WriteReg(0x91, _usX);
// 	RA8875_WriteReg(0x92, _usX >> 8);
// 	RA8875_WriteReg(0x93, _usY);
// 	RA8875_WriteReg(0x94, _usY >> 8);

// 	/* 设置终点坐标 */
// 	RA8875_WriteReg(0x95, _usX + _usWidth - 1);
// 	RA8875_WriteReg(0x96, (_usX + _usWidth - 1) >> 8);
// 	RA8875_WriteReg(0x97, _usY + _usHeight - 1);
// 	RA8875_WriteReg(0x98, (_usY + _usHeight - 1) >> 8);

// 	RA8875_SetFrontColor(_usColor);	/* 设置颜色 */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 7) | (1 << 5) | (1 << 4) | (0 << 0));	/* 开始填充矩形  */
// //	while (RA8875_ReadReg(0x90) & (1 << 7));							/* 等待结束 */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

/*
// *********************************************************************************************************
// *	函 数 名: RA8875_DrawCircle
// *	功能说明: 绘制一个圆，笔宽为1个像素
// *	形    参：
// *			_usX,_usY  ：圆心的坐标
// *			_usRadius  ：圆的半径
// *			_usColor  ：颜色
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
// {
// 	/*
// 		RA8875 支持圆形绘图功能，让使用者以简易或低速的MCU 就可以在TFT 模块上画圆。先设定
// 		圆的中心点REG[99h~9Ch]，圆的半径REG[9Dh]，圆的颜色REG[63h~65h]，然后启动绘图
// 		REG[90h] Bit6 = 1，RA8875 就会将圆的图形写入DDRAM，相对的在TFT 模块上就可以显示所
// 		画的圆。若设定REG[90h] Bit5 = 1，则可画出一实心圆 (Fill)；若设定REG[90h] Bit5 = 0，则可
// 		画出空心圆 (Not Fill
// 	*/
// 	/* 设置圆的半径 */
// 	if (_usRadius > 255)
// 	{
// 		return;
// 	}

// 	/* 设置圆心坐标 */
// 	RA8875_WriteReg(0x99, _usX);
// 	RA8875_WriteReg(0x9A, _usX >> 8);
// 	RA8875_WriteReg(0x9B, _usY);
// 	RA8875_WriteReg(0x9C, _usY >> 8);

// 	RA8875_WriteReg(0x9D, _usRadius);	/* 设置圆的半径 */

// 	RA8875_SetFrontColor(_usColor);	/* 设置颜色 */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 6) | (0 << 5));				/* 开始画圆, 不填充  */
// //	while (RA8875_ReadReg(0x90) & (1 << 6));				/* 等待结束 */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_FillCircle
*	功能说明: 填充一个圆
*	形    参：
*			_usX,_usY  ：圆心的坐标
*			_usRadius  ：圆的半径
*			_usColor   ：颜色
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
// {
// 	/*
// 		RA8875 支持圆形绘图功能，让使用者以简易或低速的MCU 就可以在TFT 模块上画圆。先设定
// 		圆的中心点REG[99h~9Ch]，圆的半径REG[9Dh]，圆的颜色REG[63h~65h]，然后启动绘图
// 		REG[90h] Bit6 = 1，RA8875 就会将圆的图形写入DDRAM，相对的在TFT 模块上就可以显示所
// 		画的圆。若设定REG[90h] Bit5 = 1，则可画出一实心圆 (Fill)；若设定REG[90h] Bit5 = 0，则可
// 		画出空心圆 (Not Fill
// 	*/
// 	/* 设置圆的半径 */
// 	if (_usRadius > 255)
// 	{
// 		return;
// 	}

// 	/* 设置圆心坐标 */
// 	RA8875_WriteReg(0x99, _usX);
// 	RA8875_WriteReg(0x9A, _usX >> 8);
// 	RA8875_WriteReg(0x9B, _usY);
// 	RA8875_WriteReg(0x9C, _usY >> 8);

// 	RA8875_WriteReg(0x9D, _usRadius);	/* 设置圆的半径 */

// 	RA8875_SetFrontColor(_usColor);	/* 设置颜色 */

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteReg(0x90, (1 << 6) | (1 << 5));				/* 开始画圆, 填充  */
// //	while (RA8875_ReadReg(0x90) & (1 << 6));				/* 等待结束 */
// 	RA8875_WaitBusy();
// 	s_ucRA8875Busy = 0;
// }

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_SetFont
// *	功能说明: 文本模式，设置文字字体、行距和字距
// *	形    参：
// *			_ucFontType : 字体类型: RA_FONT_16, RA_FONT_24, RA_FONT_32
// *			_ucLineSpace : 行距，像素单位 (0-31)
// *			_ucCharSpace : 字距，像素单位 (0-63)
// *
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_SetFont(uint8_t _ucFontType, uint8_t _ucLineSpace, uint8_t _ucCharSpace)
// {
// 	/*
// 		[29H]在文字模式下，用来设定文字间的行距 (单位: 像素) 。
// 		只有低5个bit有效，0-31
// 	*/
// 	if (_ucLineSpace >31)
// 	{
// 		_ucLineSpace = 31;
// 	}
// 	RA8875_WriteReg(0x29, _ucLineSpace);

// 	/*
// 		[2EH] 设置字符间距（像素单位，0-63），和字体（16*16，24*24，32*32）
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
*	函 数 名: RA8875_SetFontZoom
*	功能说明: 文本模式，设置文字的放大模式，1X,2X,3X, 4X
*	形    参：
*			_ucHSize : 文字水平放大倍数，0-3 分别对应 X1、X2、X3、X4
*			_ucVSize : 文字处置放大倍数，0-3 分别对应 X1、X2、X3、X4
*		建议输入参数用枚举：RA_SIZE_X1、RA_SIZE_X2、RA_SIZE_X3、RA_SIZE_X4
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_SetTextZoom(uint8_t _ucHSize, uint8_t _ucVSize)
// {
// 	/*
// 		pdf 第22页		[22H]
// 		bit7 用于对齐，一般不用，缺省设0
// 		bit6 用于通透，一般不用，缺省设0
// 		bit4 用于旋转90读，一般不用，缺省设0
// 		bit3-2 水平放大倍数
// 		bit1-0 垂直放大倍数
// 	*/
// 	RA8875_WriteReg(0x22, ((_ucHSize & 0x03) << 2) | ( _ucVSize & 0x03));
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_DispAscii
*	功能说明: 显示ASCII字符串，字符点阵来自于RA8875内部ROM中的ASCII字符点阵（8*16），不支持汉字.
*			文字颜色、背景颜色、是否通透由其他的函数进行设置
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	形    参：_usX, _usY : 显示位置（坐标）
*			 _ptr : AascII字符串，已0结束
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_DispAscii(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 内建8x16 点的ASCII 字型ROM，提供使用者更方便的方式用特定编码 (Code) 输入文
// 		字。内建的字集支持ISO/IEC 8859-1~4 编码标准，此外，使用者可以透过REG[60h~62h] 选择
// 		文字前景颜色，以及透过REG[63h~65h] 选择背景颜色.

// 		ISO/IEC 8859-1，又称Latin-1或“西欧语言”，是国际标准化组织内ISO/IEC 8859的第一个8位字符集。
// 		它以ASCII为基础，在空置的0xA0-0xFF的范围内，加入192个字母及符号，藉以供使用变音符号的拉丁字母语言使用。

// 		ISO/IEC 8859-2 Latin-2或“中欧语言”，是国际标准化组织内ISO/IEC 8859的其中一个8位字符集 .
// 		ISO/IEC 8859-3 南欧语言字符集
// 		ISO/IEC 8859-4 北欧语言字符集
// 	*/

// 	/*
// 	(1) Text mode  REG[40h] bit7=1
// 	(2) Internal Font ROM Select   REG[21h] bit7=0, bit5=0
// 	(3) Font foreground and background color Select  REG[63h~65h], REG[60h~62h]
// 	(4) Write the font Code  CMD_WR[02h]    DATA_WR[font_code]
// 	*/

// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */

// 	/* 选择CGROM font; 选择内部CGROM; 内部CGROM 编码选择ISO/IEC 8859-1. */
// 	RA8875_WriteReg(0x2F, 0x00);
// 	RA8875_WriteReg(0x21, (0 << 7) | (0 << 5) | (0 << 1) | (0 << 0));

// 	s_ucRA8875Busy = 1;


// 	RA8875_WriteCmd(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

// 	/* 开始循环处理字符 */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_WriteData(*_ptr);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);	/* 必须等待内部写屏操作完成 */
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
// }


// /*
// *********************************************************************************************************
// *	函 数 名: LCD_DispStr
// *	功能说明: 在LCD指定坐标（左上角）显示一个字符串
// *	形    参：
// *		_usX : X坐标，对于3.0寸宽屏，范围为【0 - 399】
// *		_usY : Y坐标，对于3.0寸宽屏，范围为 【0 - 239】
// *		_ptr  : 字符串指针
// *		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
// {
// 	uint32_t i;
// 	uint8_t code1;
// 	uint8_t code2;
// 	uint32_t address;
// 	uint8_t buf[24 * 24 / 8];	/* 最大支持24点阵汉字 */
// 	uint8_t m, width;
// 	uint8_t font_width,font_height, font_bytes;
// 	uint16_t x, y;
// 	const uint8_t *pAscDot;
// 	const uint8_t *pHzDot;

// 	/* 如果字体结构为空指针，则缺省按16点阵 */
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
// 		/* 缺省是16点阵 */
// 		font_height = 16;
// 		font_width = 16;
// 		font_bytes = 32;
// 		pAscDot = g_Ascii16;
// 		pHzDot = g_Hz16;
// 	}
// 	
//   //RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */
// 	/* 开始循环处理字符 */
// 	while (*_ptr != 0)
// 	{
// 		code1 = *_ptr;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
// 		if (code1 < 0x80)
// 		{
// 			/* 将ascii字符点阵复制到buf */
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

// 			/* 计算16点阵汉字点阵地址
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
// 						/* 字库搜索完毕，未找到，则填充全FF */
// 						memset(buf, 0xFF, font_bytes);
// 						break;
// 					}
// 				}
// 			#else	/* 用全字库 */
// 				/* 此处需要根据字库文件存放位置进行修改 */
// 				address = ((code1-0xa1) * 94 + (code2-0xa1)) * font_bytes + HZK16_ADDR;
// 				memcpy(buf, (const uint8_t *)address, font_bytes);
// 			#endif

// 				width = font_width;
// 		}

// 		y = _usY;
// 		/* 开始刷LCD */
// 		for (m = 0; m < font_height; m++)	/* 字符高度 */
// 		{
// 			x = _usX;
// 			for (i = 0; i < width; i++)	/* 字符宽度 */
// 			{
// 				if ((buf[m * ((2 * width) / font_width) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
// 				{
// 					RA8875_PutPixel(x, y, _tFont->usTextColor);	/* 设置像素颜色为文字色 */
// 				}
// 				else
// 				{
// 					RA8875_PutPixel(x, y, _tFont->usBackColor);	/* 设置像素颜色为文字背景色 */
// 				}

// 				x++;
// 			}
// 			y++;
// 		}

// 		if (_tFont->usSpace > 0)
// 		{
// 			/* 如果文字底色按_tFont->usBackColor，并且字间距大于点阵的宽度，那么需要在文字之间填充(暂时未实现) */
// 		}
// 		_usX += width + _tFont->usSpace;	/* 列地址递增 */
// 		_ptr++;			/* 指向下一个字符 */
// 	}
// 	//RA8875_WriteReg(0x40, (0 << 7));	/* 设置为文本模式 */
// }
/*
*********************************************************************************************************
*	函 数 名: RA8875_DispStr1
*	功能说明: 显示字符串，字符点阵来自于RA8875外接的字库芯片，支持汉字.
*			文字颜色、背景颜色、是否通透由其他的函数进行设置
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	形    参：_usX, _usY : 显示位置（坐标）
*			 _ptr : AascII字符串，已0结束
*	返 回 值: 无
*********************************************************************************************************
*/
// void RA8875_DispStr1(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 透过使用 — 上海集通公司 (Genitop Inc) 外部串行式字体内存 (Font ROM)，可支持各样
// 		的文字写入到DDRAM 中。RA8875 与上海集通公司 (Genitop Inc) 兼容的产品包含 :
// 		GT21L16TW/GT21H16T1W 、GT23L16U2W 、GT23L24T3Y/GT23H24T3Y 、GT23L24M1Z 、
// 		及 GT23L32S4W/GT23H32S4W。这些字体包含16x16, 24x24, 32x32 点 (Dot) 与不同的字

// 		安富莱RA8875驱动板集成的字库芯片为 集通字库芯片_GT23l32S4W

// 		GT23L32S4W是一款内含11X12点阵、15X16点、24X24点阵、32X32点阵的汉字库芯片，支持GB2312
// 		国标汉字（含有国家信标委合法授权）及SCII字符。排列格式为横置横排。用户通过字符内码，利用本手
// 		册提供的方法计算出该字符点阵在芯片中的地址，可从该地址连续读出字符点阵信息。
// 	*/

// 	/* 设置文本显示位置，注意文本模式的写入光标和图形模式的写入光标是不同的寄存器 */
// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */

// 	/*
// 		Serial Flash/ROM 频率频率设定
// 			0xb: SFCL 频率 = 系统频率频率(当DMA 为致能状态，并且色彩深度为256 色，则SFCL 频率
// 				固定为=系统频率频率/ 2)
// 			10b: SFCL 频率 =系统频率频率/ 2
// 			11b: SFCL 频率 =系统频率频率/ 4

// 		安富莱驱动板系统频率为 68MHz

// 		GT23L32S4W的访问速度：SPI 时钟频率：20MHz(max.)

// 		因此需要设置 4 分频, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* 设置为文本模式 */

// 	/* 选择外部字体ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # 选择
// 				0:选择Serial Flash/ROM 0 接口。[安富莱RA8875驱动板字库芯片接在 0 接口]
// 				1:选择Serial Flash/ROM 1 接口。
// 		6	Serial Flash/ROM 寻址模式
// 				0: 24 位寻址模式。
// 				此位必须设为0。
// 		5	Serial Flash/ROM 波形模式
// 				0: 波形模式 0。
// 				1: 波形模式 3。
// 		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
// 			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
// 			01b: 5 bus ??1 byte 空周期。
// 			1Xb: 6 bus ??2 byte 空周期。
// 		2	Serial Flash /ROM 存取模式 (Access Mode)
// 			0: 字型模式 。
// 			1: DMA 模式。
// 		1-0	Serial Flash /ROM I/F Data Latch 选择模式
// 			0Xb: 单一模式。
// 			10b: 双倍模式0。
// 			11b: 双倍模式1。
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		设置外部字体芯片型号为 GT23L32S4W, 编码为GB2312,

// 		Bit1:0 决定ASCII字符的格式：
// 			0 = NORMAL		 [笔画细, 和汉字顶部对齐]
// 			1 = Arial		 [笔画粗，和汉字底部对齐]
// 			2 = Roman		 [笔画细, 和汉字底部对齐]
// 			3 = Bold		 [乱码,不可用]
// 	 */
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));

// 	s_ucRA8875Busy = 1;
// 	RA8875_WriteCmd(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

// 	/* 开始循环处理字符 */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_WriteData(*_ptr);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
// }

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetDispWin
*	功能说明: 设置显示窗口，进入窗口显示模式。在窗口显示模式，连续写显存，光标会自动在设定窗口内进行递增
*	形    参：
*		_usX : 水平坐标
*		_usY : 垂直坐标
*		_usHeight: 窗口高度
*		_usWidth : 窗口宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{

	uint16_t usTemp;

	/* 坐标系统示意图： （横屏）
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

		左上角是坐标零点, 扫描方向，先从左到右，再从上到下。

		如果需要做竖屏方式，你需要进行物理坐标和逻辑坐标的转换
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

	/* 保存当前窗口信息，提高以后单色填充操作的效率.
	另外一种做法是通过读取0x30-0x37寄存器获得当前窗口，不过效率较低 */
	s_WinX = _usX;
	s_WinY = _usY;
	s_WinHeight = _usHeight;
	s_WinWidth = _usWidth;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetCursor
*	功能说明: 设置写显存的光标位置（图形模式）
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存写光标的坐标 【注意0x80-83 是光标图形的坐标】 */
	RA8875_WriteReg(0x46, _usX);
	RA8875_WriteReg(0x47, _usX >> 8);
	RA8875_WriteReg(0x48, _usY);
	RA8875_WriteReg(0x49, _usY >> 8);
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetCursor
*	功能说明: 设置读显存的光标位置。 很多其他的控制器写光标和读光标是相同的寄存器，但是RA8875是不同的。
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetReadCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存读光标的坐标 */
	RA8875_WriteReg(0x4A, _usX);
	RA8875_WriteReg(0x4B, _usX >> 8);
	RA8875_WriteReg(0x4C, _usY);
	RA8875_WriteReg(0x4D, _usY >> 8);
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetTextCursor
*	功能说明: 设置文本写入光标位置
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetTextCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存读光标的坐标 */
	RA8875_WriteReg(0x2A, _usX);
	RA8875_WriteReg(0x2B, _usX >> 8);
	RA8875_WriteReg(0x2C, _usY);
	RA8875_WriteReg(0x2D, _usY >> 8);
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_QuitWinMode
*	功能说明: 退出窗口显示模式，变为全屏显示模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_QuitWinMode(void)
{
	RA8875_SetDispWin(0, 0, 480, 800);
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetBackLight
*	功能说明: 配置RA8875芯片的PWM1相关寄存器，控制LCD背光
*	形    参：_bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetBackLight(uint8_t _bright)
{
	if (_bright == 0)
	{
		/* 关闭PWM, PWM1引脚缺省输出LOW  */
		RA8875_WriteReg(0x8A, 0 << 6);
	}
	else if (_bright == 1)	/* 最大亮度 */
	{
		/* 关闭PWM, PWM1引脚缺省输出HIGH */
		RA8875_WriteReg(0x8A, 1 << 6);
	}
	else
	{
		/* 使能PWM1, 进行占空比调节 */

		/* 	REG[8Ah] PWM1 Control Register (P1CR)

			Bit7 脉波宽度调变 (PWM1) 设定
				0 : 关闭，此状态下，PWM1 输出准位依照此缓存器Bit6 决定。
				1 : 开启。

			Bit6 PWM1 关闭时的准位
				0 : 当PWM 关闭或于睡眠模式时，PWM1 输出为”Low” 状态。
				1 : 当PWM 关闭或于睡眠模式时，PWM1 输出为”High” 状态。

			Bit5 保留

			Bit4 PWM1 功能选择
				0 : PWM1 功能。
				1 : PWM1 固定输出一频率为外部晶体振荡器Clock (Fin) 频率 1 /16 的Clock

			Bit3-0  PWM1 电路的频率来源选择PWM_CLK	【不是PWM输出频率】
				0000b : SYS_CLK / 1 1000b : SYS_CLK / 256
				0001b : SYS_CLK / 2 1001b : SYS_CLK / 512
				0010b : SYS_CLK / 4 1010b : SYS_CLK / 1024
				0011b : SYS_CLK / 8 1011b : SYS_CLK / 2048
				0100b : SYS_CLK / 16 1100b : SYS_CLK / 4096
				0101b : SYS_CLK / 32 1101b : SYS_CLK / 8192
				0110b : SYS_CLK / 64 1110b : SYS_CLK / 16384
				0111b : SYS_CLK / 128 1111b : SYS_CLK / 32768

				“SYS_CLK” 代表系统频率， 例如SYS_CLK 为20MHz， 当Bit[3:0] =0001b 时，PWM1 频率来源为10MHz。

				对于安富莱的4.3寸模块，SYS_CLK =  68.75MHz
				Bit3-0 选择8时，PWM_CLK = 68.75MHz / 256 = 0.2685546875 MHz = 268.55KHz；
				实际输出的PWM波形频率 = PWM_CLK / 256 = 1.049kHz。【用示波器已验证，PWM频率很准】

				选择10时，频率256Hz左右，避免1KHz的啸叫音
		*/

//		RA8875_WriteReg(0x8A, (1 << 7) | 10);
		RA8875_WriteReg(0x8A, (1 << 7) | 7);

		/* REG[8Bh] PWM1 Duty Cycle Register (P1DCR) */
		RA8875_WriteReg(0x8B, _bright);
	}
}

// /*
// *********************************************************************************************************
// *	函 数 名: RA8875_DispStr
// *	功能说明: 显示字符串，字符点阵来自于RA8875外接的字库芯片，支持汉字.
// *			文字颜色、背景颜色、是否通透由其他的函数进行设置
// *			void RA8875_SetFrontColor(uint16_t _usColor);
// *			void RA8875_SetBackColor(uint16_t _usColor);
// *	形    参：_usX, _usY : 显示位置（坐标）
// *			 _ptr : AascII字符串，已0结束
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DispStrChip(uint16_t _usX, uint16_t _usY, char *_ptr)
// {
// 	/*
// 		RA8875 透过使用 — 上海集通公司 (Genitop Inc) 外部串行式字体内存 (Font ROM)，可支持各样
// 		的文字写入到DDRAM 中。RA8875 与上海集通公司 (Genitop Inc) 兼容的产品包含 :
// 		GT21L16TW/GT21H16T1W 、GT23L16U2W 、GT23L24T3Y/GT23H24T3Y 、GT23L24M1Z 、
// 		及 GT23L32S4W/GT23H32S4W。这些字体包含16x16, 24x24, 32x32 点 (Dot) 与不同的字

// 		安富莱RA8875驱动板集成的字库芯片为 集通字库芯片_GT23l32S4W

// 		GT23L32S4W是一款内含11X12点阵、15X16点、24X24点阵、32X32点阵的汉字库芯片，支持GB2312
// 		国标汉字（含有国家信标委合法授权）及SCII字符。排列格式为横置横排。用户通过字符内码，利用本手
// 		册提供的方法计算出该字符点阵在芯片中的地址，可从该地址连续读出字符点阵信息。
// 	*/

// 	/* 设置文本显示位置，注意文本模式的写入光标和图形模式的写入光标是不同的寄存器 */
// 	RA8875_SetTextCursor(_usX, _usY);

// 	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */

// 	/*
// 		Serial Flash/ROM 频率频率设定
// 			0xb: SFCL 频率 = 系统频率频率(当DMA 为致能状态，并且色彩深度为256 色，则SFCL 频率
// 				固定为=系统频率频率/ 2)
// 			10b: SFCL 频率 =系统频率频率/ 2
// 			11b: SFCL 频率 =系统频率频率/ 4

// 		安富莱驱动板系统频率为 68MHz

// 		GT23L32S4W的访问速度：SPI 时钟频率：20MHz(max.)

// 		因此需要设置 4 分频, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* 设置SCL频率 */

// 	/* 选择外部字体ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # 选择
// 				0:选择Serial Flash/ROM 0 接口。[安富莱RA8875驱动板字库芯片接在 0 接口]
// 				1:选择Serial Flash/ROM 1 接口。
// 		6	Serial Flash/ROM 寻址模式
// 				0: 24 位寻址模式。
// 				此位必须设为0。
// 		5	Serial Flash/ROM 波形模式
// 				0: 波形模式 0。
// 				1: 波形模式 3。
// 		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
// 			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
// 			01b: 5 bus ??1 byte 空周期。
// 			1Xb: 6 bus ??2 byte 空周期。
// 		2	Serial Flash /ROM 存取模式 (Access Mode)
// 			0: 字型模式 。
// 			1: DMA 模式。
// 		1-0	Serial Flash /ROM I/F Data Latch 选择模式
// 			0Xb: 单一模式。
// 			10b: 双倍模式0。
// 			11b: 双倍模式1。
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		设置外部字体芯片型号为 GT23L32S4W, 编码为GB2312,

// 		Bit1:0 决定ASCII字符的格式：
// 			0 = NORMAL		 [笔画细, 和汉字顶部对齐]
// 			1 = Arial		 [笔画粗，和汉字底部对齐]
// 			2 = Roman		 [笔画细, 和汉字底部对齐]
// 			3 = Bold		 [乱码,不可用]
// 	 */
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (1 << 0));

// 	s_ucRA8875Busy = 1;
// 	RA8875_REG = 0x02; 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

// 	/* 开始循环处理字符 */
// 	while (*_ptr != 0)
// 	{
// 		RA8875_RAM = *_ptr;
// 		while ((RA8875_REG & 0x80) == 0x80);
// 		_ptr++;
// 	}
// 	s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
// }



/*
*********************************************************************************************************
*	函 数 名: RA8875_DrawHLine
*	功能说明: 绘制一条固定颜色的水平线 （主要用于UCGUI的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usX2    ：结束点X坐标
*			  _usColor : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);

	RA8875_WriteCmd(0x02); 		/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
	for (i = 0; i < _usX2 - _usX1 + 1; i++)
	{
		RA8875_WriteData16(_usColor);
	}

}

/*
*********************************************************************************************************
*	函 数 名: RA8875_DrawHColorLine
*	功能说明: 绘制一条彩色水平线 （主要用于UCGUI的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usWidth ：直线的宽度
*			  _pColor : 颜色缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);


	RA8875_WriteCmd(0x02); 		/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

	for (i = 0; i < _usWidth; i++)
	{
		RA8875_WriteData16(*_pColor++);
	}

}

/*
*********************************************************************************************************
*	函 数 名: RA8875_DrawVLine
*	功能说明: 绘制一条垂直线条 （主要用于UCGUI的接口函数）
*	形    参： _usX1    : 垂直线的X坐标
*			  _usY1    : 起始点Y坐标
*			  _usY2    : 结束点Y坐标
*			  _usColor : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor)
{
	uint16_t i;

	RA8875_SetCursor(_usX1, _usY1);

	for (i = 1; i <= _usY2 - _usY1 + 1; i++)
	{
		s_ucRA8875Busy = 1;

		RA8875_WriteCmd(0x02); 		/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
		RA8875_WriteData16(_usColor);

		RA8875_SetCursor(_usX1, _usY1 + i);
	}

}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawHColorLine
*	功能说明: 绘制一条彩色水平线 （主要用于UCGUI的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usWidth ：直线的宽度
*			  _pColor : 颜色缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{

		RA8875_DrawHColorLine(_usX1, _usY1, _usWidth, _pColor);
	

}

/*
*********************************************************************************************************
*	函 数 名: LCD_DrawLineV
*	功能说明: 绘制一条垂直线条 （主要用于UCGUI的接口函数）
*	形    参： _usX1    : 垂直线的X坐标
*			  _usY1    : 起始点Y坐标
*			  _usY2    : 结束点Y坐标
*			  _usColor : 颜色
*	返 回 值: 无
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
*	函 数 名: LCD_GetPixel
*	功能说明: 读取1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: RGB颜色值
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
	WriteData(XL>>8);  //工作窗口水平起始点
	RA8875_WAITSTATUS();//	
	WriteCommand(0x34);//HEAW0
	WriteData(XR);
	WriteCommand(0x35);//HEAW1	   
	WriteData(XR>>8);  //工作窗口水平结束点
	RA8875_WAITSTATUS();//	
	//setting active window Y
	
	WriteCommand(0x32);//VSAW0
	WriteData(YT);  
	WriteCommand(0x33);//VSAW1	   
	WriteData(YT>>8);  //工作窗口垂直起始点
	RA8875_WAITSTATUS();//	
	WriteCommand(0x36);//VEAW0
	WriteData(YB); 
	WriteCommand(0x37);//VEAW1	   
	WriteData(YB>>8);  //工作窗口垂直结束点
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

void Text_Mode(void) //进入文字模式
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

// void SERIAL_ROM_Dual_mode0(void)	//模式：双倍模式0
// {
// 	u8 temp;
// 	WriteCommand(0x05);
// 	temp = ReadData();
// 	temp &=0xFC;
// 	temp |=0x02;
// 	WriteData(temp);
// }

 void SERIAL_ROM_DMA_mode(void)	 //DMA模式
{
	u8 temp;
	WriteCommand(0x05);
	temp = ReadData();
	temp |= 0x04;
	WriteData(temp);
}

//  void SERIAL_ROM_COMMON_mode(void)	 //普通模式
// {
// 	u8 temp;
// 	WriteCommand(0x05);
// 	temp = ReadData();
// 	temp &= 0xFB;
// 	WriteData(temp);
// }

void Graphic_Mode(void)		//常规图形模式，内存写入光标加1 内存读取光标加1
{
	WriteCommand(0x40);//MWCR0
	WriteData(0x00);

	//Cmd_Write(0x40,LCD_CmdRead(0x40)&0x7f); //GRAPHIC MODE
    RA8875_WriteReg(0x05,0xac); //选择FLASH1
	RA8875_WriteReg(0x06,0x0);		//
}

void SERIAL_ROM_Font_mode(void)	  //字型模式
{
	u8 temp;
  	WriteCommand(0x05);
	temp = ReadData();
	temp &=0xfb;
	WriteData(temp);
}

void bmp_dispflash(u32 bmpaddress,u16 x0,u16 y0,u16 bmp_x,u16 bmp_y)	
{

	u32 readcnt = 1024;		//一次读取的字节数

	//u32 i;
	u32 pointcnt,bytecnt;//位图像素数
	u32 addr,addrend;
	u32 readtimes;
	u32 lastbytes; 
	u16 x1,y1;
	char mem[512];
	//FLASHBMPHEADER fbmpheader;

	addr=bmpaddress;

    //图形工作模式
	RA8875_WriteReg(0x06, (2 << 0));	/* 设置SCL频率 */
	s_ucRA8875Busy = 1;
	RA8875_REG = 0x02; 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */

	/* 开始循环处理字符 */


	pointcnt= bmp_x*bmp_y;
	bytecnt= pointcnt;	
	//readtimes=bytecnt/512;		 //计算读取块的数量
    //lastbytes=bytecnt%512;		 //计算剩余字节数	
	x1=x0+bmp_x-1;
	y1=y0+bmp_y-1;
  DMA_Continuous_mode();	     // bfh=0
	Text_Mode();
	RA8875_REG = 0x02; 

	//Graphic_Mode();
	Active_Window(x0,x1,y0,y1);   //设置工作窗口   30h-37h
	XY_Coordinate(x0,y0);         //设置内存写入位置  46h-49h
	Serial_ROM_select1();         //选择SPIFLASH  05h
	SERIAL_ROM_DMA_mode();
	//Select_Serial_Waveform_mode3();
	//SERIAL_ROM_Dual_mode1();
	//Serial_ROM_Address_set_32u8();
	DMA_Start_address_setting(addr);  //设置DMA数据来源起始位置	b0-b3h
	DMA_Continuous_mode_size_setting(pointcnt); //设置DMA搬移数目 b4 b6 b8
	//Serial_ROM_direct_access_mode_enable();
	//WriteCommand(0xBF);
	//WriteData(0x01);

	//
	DMA_Start_enable();
 	//LcdPrintStr("◆检查DMA",10,100,RED,WHITE,1);

	Chk_DMA_Busy();
	//LcdPrintStr("◆已显示",10,130,RED,WHITE,1);
	//DMA_Continuous_mode();//
	//DMA_Start_enable();

		s_ucRA8875Busy = 0;
 }


// void RA8875_SetTextMode(uint8_t Mode)
// {
// 	/*
// 		pdf 第22页		[22H]
// 		bit7 用于对齐，一般不用，缺省设0
// 		bit6 用于通透，一般不用，缺省设0
// 		bit4 用于旋转90读，一般不用，缺省设0
// 		bit3-2 水平放大倍数
// 		bit1-0 垂直放大倍数
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
// 	Color_B=(tempcolor&0x00f80000)>>19;//取高5BIT
// 	Color_G=(tempcolor&0x0000fc00)>>5;
// 	Color_R=(tempcolor&0x000000f8)<<8;
// 	
// 	return((u16)((Color_R)|(Color_G)|Color_B));
// 	
// }
//  /*
// *********************************************************************************************************
// *	函 数 名: RA8875_DispStr
// *	功能说明: 显示字符串，字符点阵来自于RA8875外接的字库芯片，支持汉字.
// *			文字颜色、背景颜色、是否通透由其他的函数进行设置
// *			void RA8875_SetFrontColor(uint16_t _usColor);
// *			void RA8875_SetBackColor(uint16_t _usColor);
// *	形    参：_usX, _usY : 显示位置（坐标）
// *			 _ptr : AascII字符串，已0结束
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void RA8875_DispChar(unsigned short c)
// {
// 	u8 i=0;
// 	unsigned char ch[2];
// 	ch[0]=(c>>8)&0x00ff;
// 	ch[1]=c&0xff;
// 	/*
// 		RA8875 透过使用 — 上海集通公司 (Genitop Inc) 外部串行式字体内存 (Font ROM)，可支持各样
// 		的文字写入到DDRAM 中。RA8875 与上海集通公司 (Genitop Inc) 兼容的产品包含 :
// 		GT21L16TW/GT21H16T1W 、GT23L16U2W 、GT23L24T3Y/GT23H24T3Y 、GT23L24M1Z 、
// 		及 GT23L32S4W/GT23H32S4W。这些字体包含16x16, 24x24, 32x32 点 (Dot) 与不同的字

// 		安富莱RA8875驱动板集成的字库芯片为 集通字库芯片_GT23l32S4W

// 		GT23L32S4W是一款内含11X12点阵、15X16点、24X24点阵、32X32点阵的汉字库芯片，支持GB2312
// 		国标汉字（含有国家信标委合法授权）及SCII字符。排列格式为横置横排。用户通过字符内码，利用本手
// 		册提供的方法计算出该字符点阵在芯片中的地址，可从该地址连续读出字符点阵信息。
// 	*/

// 	/* 设置文本显示位置，注意文本模式的写入光标和图形模式的写入光标是不同的寄存器 */
// 	//RA8875_SetTextCursor(_usX, _usY);
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/*
// 		Serial Flash/ROM 频率频率设定
// 			0xb: SFCL 频率 = 系统频率频率(当DMA 为致能状态，并且色彩深度为256 色，则SFCL 频率
// 				固定为=系统频率频率/ 2)
// 			10b: SFCL 频率 =系统频率频率/ 2
// 			11b: SFCL 频率 =系统频率频率/ 4

// 		

// 		GT23L32S4W的访问速度：SPI 时钟频率：20MHz(max.)

// 		因此需要设置 4 分频, 17MHz
// 	*/
// 	RA8875_WriteReg(0x06, (3 << 0));	/* 设置为文本模式 */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* 选择外部字体ROM */
// 	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
// 		7	Serial Flash/ROM I/F # 选择
// 				0:选择Serial Flash/ROM 0 接口。[创为科技RA8875驱动板字库芯片接在 0 接口]
// 				1:选择Serial Flash/ROM 1 接口。
// 		6	Serial Flash/ROM 寻址模式
// 				0: 24 位寻址模式。
// 				此位必须设为0。
// 		5	Serial Flash/ROM 波形模式
// 				0: 波形模式 0。
// 				1: 波形模式 3。
// 		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
// 			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
// 			01b: 5 bus ??1 byte 空周期。
// 			1Xb: 6 bus ??2 byte 空周期。
// 		2	Serial Flash /ROM 存取模式 (Access Mode)
// 			0: 字型模式 。
// 			1: DMA 模式。
// 		1-0	Serial Flash /ROM I/F Data Latch 选择模式
// 			0Xb: 单一模式。
// 			10b: 双倍模式0。
// 			11b: 双倍模式1。
// 	*/
// 	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

// 	/*
// 		设置外部字体芯片型号为 GT23L32S4W, 编码为GB2312,

// 		Bit1:0 决定ASCII字符的格式：
// 			0 = NORMAL		 [笔画细, 和汉字顶部对齐]
// 			1 = Arial		 [笔画粗，和汉字底部对齐]
// 			2 = Roman		 [笔画细, 和汉字底部对齐]
// 			3 = Bold		 [乱码,不可用]
// 	 */
// 	 //while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (0 << 0));
//   RA8875_WriteReg(0x22, 0<<6);//文字通透
// 	//s_ucRA8875Busy = 1;
// 	RA8875_WriteCmd(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
//   while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	/* 开始循环处理字符 */
// 	for(i=0;i<2;i++)
// 	{
// 		//RA8875_WriteData("我");
// 		//ch[0]="我";
// 		RA8875_WriteData(ch[i]);
// 		while ((RA8875_ReadStatus() & 0x80) == 0x80);

// 		//i++;
// 	}
// 	//s_ucRA8875Busy = 0;

// 	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
// 	while ((RA8875_ReadStatus() & 0x80) == 0x80);
// 	
// 	while((RA8875_ReadReg(0xf0)&0x01)==0x01) ;
// }



