#include "glcd.h"
#include "LCD_RA8875.h"
#include "stm32f4xx_fsmc.h"
 /******************************************************************************
  * 文件路径  ：   Hardware/glcd/glcd.c 
  * 作者      ：   农晓明   QQ：382421307
  * 固件库版本：   V1.0.0
  * 日期      ：   2012-4-5
  * 文件描述  ：   提供对板上LCD的FSMC驱动
  ******************************************************************************/
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
PD7    -----FSMC_NE1  ----CS
PD11 -----FSMC_A16   ----RS

PB5-------------------LCD_BL

***************************************************************/
//  extern uint8_t Flag_Test_buzzer;

  /*液晶屏的字体*/

  /*液晶屏的前景色和背景色*/
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

/**************************************************************************************************************
 * 函数名：GPIO_AF_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD连接的GPIO复用连接到FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void GPIO_AF_FSMC_Config(void)
{
  /*配置GPIOD相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	        //配置GPIOD的Pin0复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);			//配置GPIOD的Pin1复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);			//配置GPIOD的Pin4复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);			//配置GPIOD的Pin5复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOD的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOD的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOD的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOD的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOD的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOD的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOD的Pin15复用连接到FSMC
   /*配置GPIOE相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOE的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOE的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOE的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOE的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOE的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);		//配置GPIOE的Pin12复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	    //配置GPIOE的Pin13复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOE的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOE的Pin15复用连接到FSMC
}
/**************************************************************************************************************
 * 函数名：LCD_GPIO_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的GPIO管脚与FSMC连接
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
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
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
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
   FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  Timing_read, Timing_write;
	LCD_GPIO_Config();                                                     //LCD管脚配置
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
                                // 用模式A控制LCD
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;

	//add by zl
	Timing_read.FSMC_AddressSetupTime = 2;        //4     
	Timing_read.FSMC_DataSetupTime = 2;  		  //4
	Timing_read.FSMC_AddressHoldTime = 0;                                            //地址保持时间
  	Timing_read.FSMC_BusTurnAroundDuration = 0;
  	Timing_read.FSMC_CLKDivision = 0;
  	Timing_read.FSMC_DataLatency = 0;
	Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;    
	  
	Timing_write.FSMC_AddressSetupTime = 2;   	 //1
	Timing_write.FSMC_DataSetupTime = 2;    	 //2
	Timing_write.FSMC_AddressHoldTime = 0;                                            //地址保持时间
  	Timing_write.FSMC_BusTurnAroundDuration = 0;
  	Timing_write.FSMC_CLKDivision = 0;
  	Timing_write.FSMC_DataLatency = 0;
	Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;  


	//add by zl
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
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;	   //&p
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;		   //&p
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}
/**************************************************************************************************************
 * 函数名：GLCD_WriteReg()
 * 输入  ：uint8_t LCD_Reg 寄存器地址, uint16_t LCD_RegValue 寄存器的值
 * 输出  ：void
 * 描述  ：写LCD寄存器函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
//   /*写寄存器地址*/
//   LCD_REG = LCD_Reg;
//   /*写入寄存器值*/
//   LCD_RAM = LCD_RegValue;
	RA8875_WriteReg(LCD_Reg,LCD_RegValue);
}
/**************************************************************************************************************
 * 函数名：LCD_ReadReg()
 * 输入  ：uint8_t LCD_Reg 需要读取的寄存器地址
 * 输出  ：uint16_t 寄存器的值
 * 描述  ：读取LCD寄存器的值
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t GLCD_ReadReg(uint8_t LCD_Reg)
{
//   /*写寄存器地址*/
//   LCD_REG = LCD_Reg;
//   /*读出寄存器值并返回*/
//   return (LCD_RAM);
	return (RA8875_ReadReg(LCD_Reg));
}
/**************************************************************************************************************
 * 函数名：GLCD_WriteRAM_Start()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：开始写LCD的RAM
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_WriteRAM_Start(void)
{
  LCD_REG = 0x02;
}

/**************************************************************************************************************
 * 函数名：GLCD_WriteRAM()
 * 输入  ：uint16_t RGB_data  16bit颜色数据
 * 输出  ：void
 * 描述  ：向LCD的RAM写数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_WriteRAM(uint16_t RGB_data)
{
  /*向LCD的RAM写数据*/
  LCD_RAM = RGB_data;
}

/**************************************************************************************************************
 * 函数名：LCD_ReadRAM()
 * 输入  ：void
 * 输出  ：uint16_t  16bit颜色值
 * 描述  ：读取LCD的RAM数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t GLCD_ReadRAM(void)
{ 
// 	//uint16_t dumry;
//   LCD_REG = 0x02; /*选择RAM寄存器*/
// 	//dumry=LCD_RAM;   /*假读*/
//   /*读取16bit RAM值*/
//   return LCD_RAM;
	return RA8875_ReadData16();
}

/**************************************************************************************************************
 * 函数名：LCD_DisplayOn()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD打开显示
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_DisplayOn(void)
{
  /*打开显示 */
//   GLCD_WriteReg(0x01, 0x80); /*设置LCD为262K色并打开显示*/
//   GLCD_WaitStatus();
	RA8875_DispOn();
}

/**************************************************************************************************************
 * 函数名：LCD_DisplayOff()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD关闭显示
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_DisplayOff(void)
{
  /*关闭显示*/
//   GLCD_WriteReg(0x01, 0x00); 
	RA8875_DispOff();
}
void MemoryWrite_Cursor_autoIncrease(void)
{
	u8 temp;
	LCD_WriteCmd(0x40);//MWCR0
	temp = LCD_REG;
	temp &= 1<<1 ;
	LCD_WriteData(temp);
}
/**************************************************************************************************************
 * 函数名：LCD_Init()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD初始化函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_Init(void)
{ 
  	u32 lcdid=0;
	  u16 temp = 0;
   	LCD_FSMC_Config();
	//新的LCD需加上延时
	/*
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);	
	delay_nms(50);
	GPIO_SetBits(GPIOC,GPIO_Pin_4);	
	delay_nms(50);
  	GLCD_WriteReg(0x88,0x11);	//SYS_CLK = FIN*(PLLDIVN[4:0] +1)/((PLLDIVM+1)*(2^PLLDIVK[2:0])),PLLDIVN[4:0] = 20 FIN = 16MHz  PLLDIVM =0   0x10
	  delay_nms(1);
	GLCD_WriteReg(0x89,0x02);   //PLLDIVK[2:0] =2
	delay_nms(10);
	*/
	RA8875_WriteCmd(0x88);
	delay_nms(1);		/* 延迟1us */
	RA8875_WriteData(11);	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 12 */
	delay_nms(1);
	RA8875_WriteCmd(0x89);
	delay_nms(1);		/* 延迟1us */
	RA8875_WriteData(2);//RA8875_WriteData(3);//RA8875_WriteData(3);	/* PLLDIVK[2:0] = 2, 除以4 */
	delay_nms(1);


	GLCD_WriteReg(0x10,0x0f);  //SYSR   u8[4:3]=00 256 color  u8[2:1]=  00 8u8 MPU interface,if 8u8 MCU interface   and 65k color display----- 0x0c
	delay_nms(1);
	GLCD_WriteReg(0x04,0x80);  //PCLK inverse	0x81	modify by zl
	delay_nms(1);
	//Horizontal set
	GLCD_WriteReg(0x14,0x63); //HDWR//Horizontal Display Width Setting Bit[6:0] ,Horizontal display width(pixels) = (HDWR + 1)*8  
	GLCD_WriteReg(0x15,0x00);//Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR),Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]      
	GLCD_WriteReg(0x16,0x03); //HNDR//Horizontal Non-Display Period Bit[4:0],Horizontal Non-Display Period (pixels) = (HNDR + 1)*8                                       
	GLCD_WriteReg(0x17,0x03); //HSTR//HSYNC Start Position[4:0],HSYNC Start Position(PCLK) = (HSTR + 1)*8                                                     
	GLCD_WriteReg(0x18,0x0b); //HPWR//HSYNC Polarity ,The period width of HSYNC. HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8                    
	delay_nms(1);
	//Vertical set                             
	GLCD_WriteReg(0x19,0xdf); //VDHR0 //Vertical Display Height Bit [7:0],Vertical pixels = VDHR + 1                                                                    
	GLCD_WriteReg(0x1a,0x01); //VDHR1 //Vertical Display Height Bit [8],Vertical pixels = VDHR + 1                                                                    
	GLCD_WriteReg(0x1b,0x20); //VNDR0 //Vertical Non-Display Period Bit [7:0]                                                  
	GLCD_WriteReg(0x1c,0x00); //VNDR1 //Vertical Non-Display Period Bit [8],Vertical Non-Display area = (VNDR + 1)                                                   
	GLCD_WriteReg(0x1d,0x16); //VSTR0 //VSYNC Start Position[7:0],VSYNC Start Position(PCLK) = (VSTR + 1)                                                    
	GLCD_WriteReg(0x1e,0x00); //VSTR1 //VSYNC Start Position[8],VSYNC Start Position(PCLK) = (VSTR + 1)                                                               
	GLCD_WriteReg(0x1f,0x01); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0],VSYNC Pulse Width(PCLK) = (VPWR + 1)                                                     
	delay_nms(1);
//	GPO_data(0x03);                      //ud=1,lr=1	
	//Active window  set 
	//setting active window X
	GLCD_WriteReg(0x20, 0x08);  //add by zl 2013.1.17
	
	GLCD_WriteReg(0x30,0x00); //Horizontal Start Point 0 of Active Window (HSAW0),Horizontal Start Point of Active Window [7:0]
	GLCD_WriteReg(0x31,0x00); //Horizontal Start Point 1 of Active Window (HSAW1),Horizontal Start Point of Active Window [9:8]	   
	GLCD_WriteReg(0x34,0x1f); //Horizontal End Point 0 of Active Window (HEAW0),Horizontal End Point of Active Window [7:0]
	GLCD_WriteReg(0x35,0x03); //Horizontal End Point 1 of Active Window (HEAW1),Horizontal End Point of Active Window [9:8]	   
	delay_nms(1);
	//setting active window Y
	GLCD_WriteReg(0x32,0x00); //Vertical Start Point 0 of Active Window (VSAW0),Vertical Start Point of Active Window [7:0]
	GLCD_WriteReg(0x33,0x00); //Vertical Start Point 1 of Active Window (VSAW1),Vertical Start Point of Active Window [8]	   
	GLCD_WriteReg(0x36,0xdf); //Vertical End Point of Active Window 0 (VEAW0),Vertical End Point of Active Window [7:0]
	GLCD_WriteReg(0x37,0x01); //Vertical End Point of Active Window 1 (VEAW1),Vertical End Point of Active Window [8]	   
	delay_nms(1);
	
	 RA8875_SetBackColor(CL_BLACK);
	 
	/**** add by zl	   *******/
	GLCD_WriteReg(0x40, 0x00);	/* 选择绘图模式 */
	/* 	REG[41h] Memory Write Control Register1 (MWCR1)
		写入目的位置，选择图层1
	*/
	GLCD_WriteReg(0x41, 0x00);	/* 选择绘图模式, 目的为CGRAM */
	/**** add by zl    *******/
	GLCD_SetActiveWindow(0,0,799,479);	//设定面板分辨率为800*480 pixel

	//GLCD_ClearActiveWindow();
	
	//GLCD_MemClear();
	MemoryWrite_Cursor_autoIncrease();
	//delay_nms(50);
	GLCD_DisplayOn();
	delay_nms(5);
	//PWM_Init();
	RA8875_SetBackLight(127);//背光
	RA8875_TouchInit();
}
/**************************************************************************************************************
 * 函数名：LCD_SetMode()
 * 输入  ：uint8_t mode 设置屏幕刷屏模式,0为→↓，1为←↓，2为↑→，3为↓→
 * 输出  ：void
 * 描述  ：设置LCD刷屏模式
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetMode(uint8_t mode)
{
  GLCD_WriteReg(0x40,mode<<3);
}

/**************************************************************************************************************
 * 函数名：LCD_Clear()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD清屏函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_Clear(uint16_t color)
{
// 	GLCD_SetBackColor(color); // 设定颜色
// 	GLCD_MemClear();
	RA8875_ClrScr(color);
}
/**************************************************************************************************************
 * 函数名：GLCD_MemClear()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD内存清屏函数
 * 调用  ：此函数会将背景颜色全部填充RAM内存，外部调用        
 *****************************************************************************************************************/
void GLCD_MemClear(void)
{
//   u8 temp;
// 	LCD_WriteCmd(0x8e);//MCLR
// 	temp = LCD_REG;
// 	temp |= 1<<7 ;
// 	LCD_WriteData(temp);
	Memory_Clear();
	//GLCD_WaitStatus();
}

void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
      ;;;;
  }
}
/**************************************************************************************************************
 * 函数名：GLCD_ClearActiveWindow()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD清除当前活动窗口函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_ClearActiveWindow(void)
{
  u8 temp;
	LCD_WriteCmd(0x8e);//MCLR
	temp = LCD_REG;
	temp |= 1<<6 ;
	LCD_WriteData(temp);
	//GLCD_WaitStatus();
}
void GLCD_WaitStatus(void)
{
// 	u8 temp;
// 	u8 flag =0;
// 	while(1)
// 	{
// 		temp=LCD_REG;
// 		if((temp&0x80)==0x80)
// 		{
// 			if(flag == 0)
// 			{
// // 				Flag_Test_buzzer=1;
// 				delay_nms(1000);
// // 				Flag_Test_buzzer=0;//蜂鸣器关闭
// 				flag = 1;
// 			}	
// 		}else
// 		{
// 			break;
// 		}
// 	}
	RA8875_WAITSTATUS();
}
/**************************************************************************************************************
 * 函数名：GLCD_SetCursor()
 * 输入  ：uint16_t Xpos, uint16_t Ypos 设定的屏幕的X值和Y值
 * 输出  ：void
 * 描述  ：LCD设置光标位置函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
//   LCD_WriteCmd(0x80);
// 	LCD_WriteData(Xpos);  
// 	LCD_WriteCmd(0x81);	   
// 	LCD_WriteData(Xpos>>8);
// 	LCD_WriteCmd(0x82);
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x83);	   
// 	LCD_WriteData(Ypos>>8);
//	RA8875_SetCursor(Xpos,Ypos);
  RA8875_WriteReg(0x80, Xpos);
	RA8875_WriteReg(0x81, Xpos >> 8);
	RA8875_WriteReg(0x82, Ypos);
	RA8875_WriteReg(0x83, Ypos >> 8);
}

/**************************************************************************************************************
 * 函数名：GLCD_MemSetCursor()
 * 输入  ：uint16_t Xpos, uint16_t Ypos 设定的屏幕的X值和Y值
 * 输出  ：void
 * 描述  ：LCD设置内存写入位置函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_MemSetCursor(uint16_t Xpos, uint16_t Ypos)
{
//   LCD_WriteCmd(0x46);
// 	LCD_WriteData(Xpos);  
// 	LCD_WriteCmd(0x47);	   
// 	LCD_WriteData(Xpos>>8);
// 	LCD_WriteCmd(0x48);
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x49);	   
// 	LCD_WriteData(Ypos>>8);
	RA8875_SetCursor(Xpos,Ypos);
	
}
/**************************************************************************************************************
 * 函数名：GLCD_SetActiveWindow()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width 区域的起点，宽和高
 * 输出  ：void
 * 描述  ：设置活动窗口区域
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetActiveWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Xend, uint16_t Yend)
{
  //setting active window X
	
// 	LCD_WriteCmd(0x30);//HSAW0
// 	LCD_WriteData(Xpos); 
// 	LCD_WriteCmd(0x31);//HSAW1	   
// 	LCD_WriteData(Xpos>>8);
// 	
// 	LCD_WriteCmd(0x34);//HEAW0
// 	LCD_WriteData(Xend);
// 	LCD_WriteCmd(0x35);//HEAW1	   
// 	LCD_WriteData(Xend>>8);
// 	
// 	//setting active window Y
// 	
// 	LCD_WriteCmd(0x32);//VSAW0
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x33);//VSAW1	   
// 	LCD_WriteData(Ypos>>8);
// 	
// 	LCD_WriteCmd(0x36);//VEAW0
// 	LCD_WriteData(Yend); 
// 	LCD_WriteCmd(0x37);//VEAW1	   
// 	LCD_WriteData(Yend>>8);
  RA8875_SetDispWin(Xpos,Ypos,Xend,Yend);

	//GLCD_WaitStatus();
}

/**************************************************************************************************************
 * 函数名：LCD_SetWindow()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width 区域的起点，宽和高
 * 输出  ：void
 * 描述  ：设置某个特定的填充区域
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Xend, uint16_t Yend)
{
  //setting active window X
	
// 	LCD_WriteCmd(0x91);//HSAW0
// 	LCD_WriteData(Xpos); 
// 	LCD_WriteCmd(0x92);//HSAW1	   
// 	LCD_WriteData(Xpos>>8);
// 	
// 	LCD_WriteCmd(0x95);//HEAW0
// 	LCD_WriteData(Xend);
// 	LCD_WriteCmd(0x96);//HEAW1	   
// 	LCD_WriteData(Xend>>8);
// 	
// 	//setting active window Y
// 	
// 	LCD_WriteCmd(0x93);//VSAW0
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x94);//VSAW1	   
// 	LCD_WriteData(Ypos>>8);
// 	
// 	LCD_WriteCmd(0x97);//VEAW0
// 	LCD_WriteData(Yend); 
// 	LCD_WriteCmd(0x98);//VEAW1	   
// 	LCD_WriteData(Yend>>8);
// 	GLCD_WaitStatus();
	
		/* 设置起点坐标 */
	RA8875_WriteReg(0x91, Xpos);
	RA8875_WriteReg(0x92, Xpos >> 8);
	RA8875_WriteReg(0x93, Ypos);
	RA8875_WriteReg(0x94, Ypos >> 8);

	/* 设置终点坐标 */
	RA8875_WriteReg(0x95, Xend);
	RA8875_WriteReg(0x96, Xend >> 8);
	RA8875_WriteReg(0x97, Yend);
	RA8875_WriteReg(0x98, Yend >> 8);

	RA8875_SetFrontColor(TextColor);	/* 设置颜色 */

	s_ucRA8875Busy = 1;
	RA8875_WriteReg(0x90, (1 << 7) | (1 << 5) | (1 << 4) | (0 << 0));	/* 开始填充矩形  */
//	while (RA8875_ReadReg(0x90) & (1 << 7));							/* 等待结束 */
	RA8875_WaitBusy();
	s_ucRA8875Busy = 0;
	
	
}
/**************************************************************************************************************
 * 函数名：LCD_SetColors()
 * 输入  ：_TextColor 前景色,_BackColor 背景色
 * 输出  ：void
 * 描述  ：设置LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
// 	LCD_WriteCmd(0x63);//BGCR0
// 	LCD_WriteData((_TextColor&0x001f));
// 	
// 	LCD_WriteCmd(0x64);//BGCR0
// 	LCD_WriteData((_TextColor&0x07e0)>>5);
// 	
// 	LCD_WriteCmd(0x65);//BGCR0
// 	LCD_WriteData((_TextColor&0xf800)>>11);
// 	LCD_WriteCmd(0x60);//BGCR0
// 	LCD_WriteData((_BackColor&0x001f));

// 	LCD_WriteCmd(0x61);//BGCR0
// 	LCD_WriteData((_BackColor&0x07e0)>>5);
// 	
// 	LCD_WriteCmd(0x62);//BGCR0
// 	LCD_WriteData((_BackColor&0xf800)>>11);
// 	
// 	GLCD_WaitStatus();
	RA8875_SetFrontColor(_TextColor);
	RA8875_SetBackColor(_BackColor);
}
/**************************************************************************************************************
 * 函数名：LCD_GetColors()
 * 输入  ：*_TextColor 前景色的指针,*_BackColor 背景色的指针
 * 输出  ：void
 * 描述  ：获取LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}
/**************************************************************************************************************
 * 函数名：LCD_SetTextColor()
 * 输入  ：uint16_t Color 前景色
 * 输出  ：void
 * 描述  ：设置LCD的前景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetTextColor(__IO uint16_t Color)
{
// 	LCD_WriteCmd(0x63);//BGCR0
// 	LCD_WriteData((Color&0x001f));
// 	
// 	LCD_WriteCmd(0x64);//BGCR0
// 	LCD_WriteData((Color&0x07e0)>>5);
// 	
// 	LCD_WriteCmd(0x65);//BGCR0
// 	LCD_WriteData((Color&0xf800)>>11);
	RA8875_SetFrontColor(Color);
}


/**************************************************************************************************************
 * 函数名：LCD_SetBackColor()
 * 输入  ：uint16_t Color 背景色
 * 输出  ：void
 * 描述  ：设置LCD的背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_SetBackColor(__IO uint16_t Color)
{
//   LCD_WriteCmd(0x60);//BGCR0
// 	LCD_WriteData((Color&0x001f));

// 	LCD_WriteCmd(0x61);//BGCR0
// 	LCD_WriteData((Color&0x07e0)>>5);
// 	
// 	LCD_WriteCmd(0x62);//BGCR0
// 	LCD_WriteData((Color&0xf800)>>11);	
// 	GLCD_WaitStatus();
	RA8875_SetBackColor(Color);
}


/**************************************************************************************************************
 * 函数名：GLCD_DrawHLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画水平线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
// 			/*设置起点X坐标*/
//   	LCD_WriteCmd(0x91);
// 	LCD_WriteData(Xpos); 
// 	LCD_WriteCmd(0x92);  
// 	LCD_WriteData(Xpos>>8);
// 	/*设置终点X坐标*/
// 	LCD_WriteCmd(0x95);
// 	LCD_WriteData(Xpos+Length);
// 	LCD_WriteCmd(0x96);	   
// 	LCD_WriteData((Xpos+Length)>>8);
// 	
// 	/*设置起点Y坐标*/
// 	LCD_WriteCmd(0x93);
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x94);   
// 	LCD_WriteData(Ypos>>8);
// 	/*设置终点Y坐标*/
// 	LCD_WriteCmd(0x97);
// 	LCD_WriteData(Ypos); 
// 	LCD_WriteCmd(0x98);	   
// 	LCD_WriteData((Ypos)>>8);
// 	/*画直线*/
// 	LCD_WriteCmd(0x90);
// 	LCD_WriteData(0xe0); 
// 	GLCD_WaitStatus();
GLCD_DrawUniLine(Xpos,Ypos,Xpos+Length,Ypos);
}
/**************************************************************************************************************
 * 函数名：GLCD_DrawVLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画垂直线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
// 		/*设置起点X坐标*/
//   	LCD_WriteCmd(0x91);
// 	LCD_WriteData(Xpos); 
// 	LCD_WriteCmd(0x92);  
// 	LCD_WriteData(Xpos>>8);
// 	/*设置终点X坐标*/
// 	LCD_WriteCmd(0x95);
// 	LCD_WriteData(Xpos);
// 	LCD_WriteCmd(0x96);	   
// 	LCD_WriteData(Xpos>>8);
// 	
// 	/*设置起点Y坐标*/
// 	LCD_WriteCmd(0x93);
// 	LCD_WriteData(Ypos);  
// 	LCD_WriteCmd(0x94);   
// 	LCD_WriteData(Ypos>>8);
// 	/*设置终点Y坐标*/
// 	LCD_WriteCmd(0x97);
// 	LCD_WriteData(Ypos+Length); 
// 	LCD_WriteCmd(0x98);	   
// 	LCD_WriteData((Ypos+Length)>>8);
// 	/*画直线*/
// 	LCD_WriteCmd(0x90);
// 	LCD_WriteData(0xe0); 
// 	GLCD_WaitStatus();
  GLCD_DrawUniLine(Xpos,Ypos,Xpos,Ypos+Length);
}
/**************************************************************************************************************
 * 函数名：LCD_DrawRect()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint8_t Height 矩形左上角点的坐标及宽和高
 * 输出  ：void
 * 描述  ：画矩形函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
// void GLCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
// {
// //  /*设置起点X坐标*/
// //   	LCD_WriteCmd(0x91);
// // 	LCD_WriteData(Xpos); 
// // 	LCD_WriteCmd(0x92);  
// // 	LCD_WriteData(Xpos>>8);
// // 	/*设置终点X坐标*/
// // 	LCD_WriteCmd(0x95);
// // 	LCD_WriteData(Xpos+Width);
// // 	LCD_WriteCmd(0x96);	   
// // 	LCD_WriteData((Xpos+Width)>>8);
// // 	
// // 	/*设置起点Y坐标*/
// // 	LCD_WriteCmd(0x93);
// // 	LCD_WriteData(Ypos);  
// // 	LCD_WriteCmd(0x94);   
// // 	LCD_WriteData(Ypos>>8);
// // 	/*设置终点Y坐标*/
// // 	LCD_WriteCmd(0x97);
// // 	LCD_WriteData(Ypos+Height); 
// // 	LCD_WriteCmd(0x98);	   
// // 	LCD_WriteData((Ypos+Height)>>8);
// // 	/*画矩形*/
// // 	LCD_WriteCmd(0x90);
// // 	LCD_WriteData(0x90); 
// // 	GLCD_WaitStatus();
//   RA8875_DrawRect(Xpos,Ypos,Width,Height,TextColor);
// }
/**************************************************************************************************************
 * 函数名：LCD_DrawCircle()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Radius 圆心坐标点及半径
 * 输出  ：void
 * 描述  ：画圆函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
// void GLCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
// {
// //  /*设置圆心X坐标*/
// //   	LCD_WriteCmd(0x99);
// // 	LCD_WriteData(Xpos); 
// // 	LCD_WriteCmd(0x9a);  
// // 	LCD_WriteData(Xpos>>8);
// // 	/*设置圆心Y坐标*/
// // 	LCD_WriteCmd(0x9b);
// // 	LCD_WriteData(Ypos);
// // 	LCD_WriteCmd(0x9c);	   
// // 	LCD_WriteData((Ypos)>>8);
// // 	
// // 	/*设置起点Y坐标*/
// // 	LCD_WriteCmd(0x9d);
// // 	LCD_WriteData(Radius);  
// // 	/*画圆形*/
// // 	LCD_WriteCmd(0x90);
// // 	LCD_WriteData(0x40); 
// // 	GLCD_WaitStatus();
// 	RA8875_DrawCircle(Xpos,Ypos,Radius,TextColor);
// }
/**************************************************************************************************************
 * 函数名：LCD_DrawMonoPict()
 * 输入  ：const uint32_t *Pict 画一个全屏单色的取膜数据
 * 输出  ：void
 * 描述  ：画一个单色的全屏图片函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
// void GLCD_DrawMonoPict(const uint32_t *Pict)
// {
//  
// }
/**************************************************************************************************************
 * 函数名：LCD_FillRect()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height 填充矩形左上角点、宽和高
 * 输出  ：void
 * 描述  ：画一个填充的矩形
 * 调用  ：外部调用        
 *****************************************************************************************************************/
// void GLCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
// { 
// 	 
// // 	/*设置起点X坐标*/
// //   	LCD_WriteCmd(0x91);
// // 	LCD_WriteData(Xpos); 
// // 	LCD_WriteCmd(0x92);  
// // 	LCD_WriteData(Xpos>>8);
// // 	/*设置终点X坐标*/
// // 	LCD_WriteCmd(0x95);
// // 	LCD_WriteData(Xpos+Width);
// // 	LCD_WriteCmd(0x96);	   
// // 	LCD_WriteData((Xpos+Width)>>8);
// // 	
// // 	/*设置起点Y坐标*/
// // 	LCD_WriteCmd(0x93);
// // 	LCD_WriteData(Ypos);  
// // 	LCD_WriteCmd(0x94);   
// // 	LCD_WriteData(Ypos>>8);
// // 	/*设置终点Y坐标*/
// // 	LCD_WriteCmd(0x97);
// // 	LCD_WriteData(Ypos+Height); 
// // 	LCD_WriteCmd(0x98);	   
// // 	LCD_WriteData((Ypos+Height)>>8);
// // 	/*画填充矩形*/
// // 	LCD_WriteCmd(0x90);
// // 	LCD_WriteData(0xb0); 
// // 	GLCD_WaitStatus();

//   RA8875_FillRect(Xpos,Ypos,Width,Height,TextColor);
// }
/**************************************************************************************************************
 * 函数名：LCD_FillCircle()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Radius 填充圆的圆心和半径
 * 输出  ：void
 * 描述  ：画一个填充圆
 * 调用  ：外部调用        
 *****************************************************************************************************************/
// void GLCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
// {
// //   /*设置圆心X坐标*/
// //   LCD_WriteCmd(0x99);
// // 	LCD_WriteData(Xpos); 
// // 	LCD_WriteCmd(0x9a);  
// // 	LCD_WriteData(Xpos>>8);
// // 	/*设置圆心Y坐标*/
// // 	LCD_WriteCmd(0x9b);
// // 	LCD_WriteData(Ypos);
// // 	LCD_WriteCmd(0x9c);	   
// // 	LCD_WriteData((Ypos)>>8);
// // 	
// // 	/*设置起点Y坐标*/
// // 	LCD_WriteCmd(0x9d);
// // 	LCD_WriteData(Radius);  
// // 	/*画填充圆形*/
// // 	LCD_WriteCmd(0x90);
// // 	LCD_WriteData(0x60); 
// // 	GLCD_WaitStatus();
// 	RA8875_FillCircle(Xpos,Ypos,Radius,TextColor);
// }
/**************************************************************************************************************
 * 函数名：LCD_DrawUniLine()
 * 输入  ：uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 起始点坐标和终点坐标
 * 输出  ：void
 * 描述  ：画任意方向的直线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void GLCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
// 	/*设置起点X坐标*/
//   	LCD_WriteCmd(0x91);
// 	LCD_WriteData(x1); 
// 	LCD_WriteCmd(0x92);  
// 	LCD_WriteData(x1>>8);
// 	/*设置终点X坐标*/
// 	LCD_WriteCmd(0x95);
// 	LCD_WriteData(x2);
// 	LCD_WriteCmd(0x96);	   
// 	LCD_WriteData(x2>>8);
// 	
// 	/*设置起点Y坐标*/
// 	LCD_WriteCmd(0x93);
// 	LCD_WriteData(y1);  
// 	LCD_WriteCmd(0x94);   
// 	LCD_WriteData(y1>>8);
// 	/*设置终点Y坐标*/
// 	LCD_WriteCmd(0x97);
// 	LCD_WriteData(y2); 
// 	LCD_WriteCmd(0x98);	   
// 	LCD_WriteData(y2>>8);
// 	/*画直线*/
// 	LCD_WriteCmd(0x90);
// 	LCD_WriteData(0xe0); 
// 	GLCD_WaitStatus();

  RA8875_DrawLine(x1,y1,x2,y2,TextColor);
}
// /**************************************************************************************************************
//  * 函数名：DrawPixel()
//  * 输入  ：int16_t x, int16_t y  点的坐标
//  * 输出  ：void
//  * 描述  ：画一个象素点
//  * 调用  ：外部调用        
//  *****************************************************************************************************************/
// static void GLCD_DrawPixel(uint16_t x, uint16_t y)
// { 
// //  /*设置起点X坐标*/
// //   LCD_WriteCmd(0x91);
// // 	LCD_WriteData(x); 
// // 	LCD_WriteCmd(0x92);  
// // 	LCD_WriteData(x>>8);
// // 	/*设置终点X坐标*/
// // 	LCD_WriteCmd(0x95);
// // 	LCD_WriteData(x);
// // 	LCD_WriteCmd(0x96);	   
// // 	LCD_WriteData(x>>8);
// // 	
// // 	/*设置起点Y坐标*/
// // 	LCD_WriteCmd(0x93);
// // 	LCD_WriteData(y);  
// // 	LCD_WriteCmd(0x94);   
// // 	LCD_WriteData(y>>8);
// // 	/*设置终点Y坐标*/
// // 	LCD_WriteCmd(0x97);
// // 	LCD_WriteData(y); 
// // 	LCD_WriteCmd(0x98);	   
// // 	LCD_WriteData(y>>8);
// // 	/*画直线*/
// // 	LCD_WriteCmd(0x90);
// // 	LCD_WriteData(0xe0); 
// // 	GLCD_WaitStatus();
//   RA8875_PutPixel(x,y,TextColor);
// }
// /**************************************************************************************************************
//  * 函数名：GLCD_Draw16bitBmp()
//  * 输入  ：uint16_t x,uint16_t y,uint16_t widht,uint16_t height,uint16_t * image 图片的位置及大小和数据
//  * 输出  ：void
//  * 描述  ：画一张16位图片
//  * 调用  ：外部调用        
//  *****************************************************************************************************************/
// void GLCD_Draw16bitBmp(uint16_t x,uint16_t y,uint16_t widht,uint16_t height,uint16_t * image)
// {
// // 	u16 w,h;
// // 	for(h=0;h<height;h++)
// // 	{
// // 		GLCD_MemSetCursor(x,y+h);
// // 		LCD_WriteCmd(0x02);		//MRWC  REG[02h] -- Memory Read/Write Command
// // 		for(w = 0; w< widht; w++)
// // 		{
// // 			LCD_WriteData(*image++);
// // 		}
// // 	}
// 	RA8875_DrawBMP(x,y,widht,height,image);
// }

