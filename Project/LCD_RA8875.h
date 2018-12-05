/*
*********************************************************************************************************
*
*	模块名称 : RA8875芯片驱动模块器驱动模块
*	文件名称 : LCD_RA8875.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _LCD_RA8875_H
#define _LCD_RA8875_H

#include "stm32f4xx.h"
#include "fonts.h"
#include "bsp_tft_lcd.h"

/* 定义LCD显示区域的分辨率 */
#define LCD_30_HEIGHT	240		/* 3.0寸宽屏 高度，单位：像素 */
#define LCD_30_WIDTH	400		/* 3.0寸宽屏 宽度，单位：像素 */

#define LCD_43_HEIGHT	272		/* 4.3寸宽屏 高度，单位：像素 */
#define LCD_43_WIDTH	480		/* 4.3寸宽屏 宽度，单位：像素 */

#define LCD_70_HEIGHT	480		/* 7.0寸宽屏 高度，单位：像素 */
#define LCD_70_WIDTH	800		/* 7.0寸宽屏 宽度，单位：像素 */
#define LENGTH_BUTTON_X        110
#define LENGTH_BUTTON_Y        30
#define LENGTH_BLOOD_X         23
#define LENGTH_BLOOD_Y         91
#define LENGTH_LEFT_X          680
#define LENGTH_LEFT_Y          440
#define LENGTH_RIGHT_X         120
#define LENGTH_RIGHT_Y         440
#define LENGTH_SERIAL_X        30
#define LENGTH_SERIAL_Y        18
#define LENGTH_COMPUTER_X      20
#define LENGTH_COMPUTER_Y      20
#define LENGTH_WINDOW_X        177
#define LENGTH_WINDOW_Y        112
#define LENGTH_LOGIN_X         800
#define LENGTH_LOGIN_Y         480
#define LENGTH_CARD_X          54
#define LENGTH_CARD_Y          78
#define LENGTH_ICON_WARING_X   80
#define LENGTH_ICON_WARING_Y   69
#define LENGTH_ICON_ERROR_X    80
#define LENGTH_ICON_ERROR_Y    69
#define LENGTH_ICON_DOWN_X     56
#define LENGTH_ICON_DOWN_Y     49
#define LENGTH_ICON_CHOICE_X   539
#define LENGTH_ICON_CHOICE_Y   44
#define LENGTH_ICON_CONFIRM_X  539
#define LENGTH_ICON_CONFIRM_Y  44
#define LENGTH_TEXT_BK_X       360
#define LENGTH_TEXT_BK_Y       50


#define ADRESS_LIS             0x3fffe8
#define ADRESS_UNLIS           0x4019c0
#define ADRESS_BLOOD           0x403398
#define ADRESS_TEST_LEFT       0x68A45A
#define ADRESS_TEST_RIGHT1     0x4920d2
#define ADRESS_TEST_RIGHT2     0x4abd62
#define ADRESS_TEST_RIGHT3     0x4c59f2
#define ADRESS_SERIAL          0x4df682
#define ADRESS_UNSERIAL        0x4dfaca
#define ADRESS_COMPUTER        0x4dff12
#define ADRESS_UNCOMPUTER      0x4E3B72
#define ADRESS_CLOSE_WINDOW    0x4e0572
#define ADRESS_LOGIN           0x4EE552
#define ADRESS_CAED            0x5a5872
#define ADRESS_CONTROL_LEFT    0x5a796a
#define ADRESS_CONTROL_RIGHT1  0x639afa
#define ADRESS_CONTROL_RIGHT2  0x65378a
#define ADRESS_ENGLISH         0x66d41a
#define ADRESS_POWER_DOWN      0x66edf2  
#define ADRESS_HISTORY_R       0x6707ca
#define ADRESS_ICON_WARING     0x730bf2
#define ADRESS_ICON_ERROR      0x730bf2
#define ADRESS_ICON_DOWN       0x73f07a
#define ADRESS_ICON_CONFIRM    0x72529a
#define ADRESS_ICON_CHOICE     0x733722
#define ADRESS_TEXT_BK         0x71C5EA


typedef unsigned long LCD_COLOR;
// enum
// {
// 	CL_WHITE        = 0xFFFF,	/* 白色 */
// 	CL_BLACK        = 0x0000,	/* 黑色 */
// 	CL_GREY         = 0xF7DE,	/* 灰色 */
// 	CL_BLUE         = 0x001F,	/* 蓝色 */
// 	CL_BLUE2        = 0x051F,	/* 浅蓝色 */
// 	CL_RED          = 0xF800,	/* 红色 */
// 	CL_MAGENTA      = 0xF81F,	/* 红紫色，洋红色 */
// 	CL_GREEN        = 0x07E0,	/* 绿色 */
// 	CL_CYAN         = 0x7FFF,	/* 蓝绿色，青色 */
// 	CL_YELLOW       = 0xFFE0,	/* 黄色 */
// 	CL_MASK			= 0x9999	/* 颜色掩码，用于文字背景透明 */
// };

enum
{
	RA_FONT_16 = 0,		/* RA8875 字体 16点阵 */
	RA_FONT_24 = 1,		/* RA8875 字体 24点阵 */
	RA_FONT_32 = 2		/* RA8875 字体 32点阵 */
};
/* 字体属性结构, 用于LCD_DispStr() */

// /* 文字放大参数 */
// enum
// {
// 	RA_SIZE_X1 = 0,		/* 原始大小 */
// 	RA_SIZE_X2 = 1,		/* 放大2倍 */
// 	RA_SIZE_X3 = 2,		/* 放大3倍 */
// 	RA_SIZE_X4 = 3		/* 放大4倍 */
// };

// typedef struct
// {
// 	uint16_t usFontCode;	/* 字体代码 0 表示16点阵 */
// 	uint16_t usTextColor;	/* 字体颜色 */
// 	uint16_t usBackColor;	/* 文字背景颜色，透明 */
// 	uint16_t usSpace;		/* 文字间距，单位 = 像素 */
// }FONT_T;

/* 可供外部模块调用的函数 */
uint8_t RA8875_ReadReg(uint8_t _ucRegAddr);
void RA8875_InitHard(void);
uint8_t RA8875_IsBusy(void);
void RA8875_Layer1Visable(void);
void RA8875_Layer2Visable(void);
void RA8875_DispOn(void);
void RA8875_DispOff(void);
void RA8875_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t RA8875_GetPixel(uint16_t _usX, uint16_t _usY);
void RA8875_SetFrontColor(uint16_t _usColor);
void RA8875_SetBackColor(uint16_t _usColor);
void RA8875_SetFont(uint8_t _ucFontType, uint8_t _ucLineSpace, uint8_t _ucCharSpace);
void RA8875_SetTextZoom(uint8_t _ucHSize, uint8_t _ucVSize);
void RA8875_DispAscii(uint16_t _usX, uint16_t _usY, char *_ptr);
void RA8875_DispStrChip(uint16_t _usX, uint16_t _usY, char *_ptr);
void RA8875_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
void RA8875_ClrScr(uint16_t _usColor);
void RA8875_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void RA8875_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void RA8875_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void RA8875_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void RA8875_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void RA8875_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth);
void RA8875_QuitWinMode(void);
void RA8875_SetBackLight(uint8_t _bright);

void RA8875_TouchInit(void);
uint16_t RA8875_TouchReadX(void);
uint16_t RA8875_TouchReadY(void);

void RA8875_InitSPI(void);

void RA8875_Delaly1ms(void);
void clear_TP_interrupt(void);	
u8   RA8875_ReadTP(void);
void RA8875_SetFrontColor(uint16_t _usColor);


/* 定义LCD驱动器的访问地址 */
#define RA8875_BASE		((uint32_t)(0x60000000 | 0x08000000))
//#define RA8875_REG		*(__IO uint16_t *)(RA8875_BASE + 2)
//#define RA8875_REG		*(__IO uint16_t *)(RA8875_BASE | 0x00020000)//CSF1.11,A16
//#define RA8875_RAM		*(__IO uint16_t *)(RA8875_BASE)

#define RA8875_REG		(*((volatile unsigned short *) 0x60020000))
#define RA8875_RAM		(*((volatile unsigned short *) 0x60000000)) /* RS = 1 */

#define ReadData()  (RA8875_RAM)	   //数据读取
#define ReadCOMM()  (RA8875_REG)	   //状态读取
#define WriteCommand(cmd)	{RA8875_WAITSTATUS();RA8875_REG = cmd;}//RA8875_WAITSTATUS();
#define WriteData(data)		{RA8875_RAM = data;}
void RA8875_WriteCmd(uint8_t _ucRegAddr);
void RA8875_WriteData(uint8_t _ucRegValue);
uint16_t RA8875_ReadData16(void);
void RA8875_WriteReg(uint8_t _ucRegAddr, uint8_t _ucRegValue);
uint8_t RA8875_ReadReg(uint8_t _ucRegAddr);
uint8_t RA8875_ReadStatus(void);
void RA8875_SetCursor(uint16_t _usX, uint16_t _usY);
void RA8875_SetReadCursor(uint16_t _usX, uint16_t _usY);
void RA8875_SetTextCursor(uint16_t _usX, uint16_t _usY);
void RA8875_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor);
void RA8875_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor);
void RA8875_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor);
void LCD_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor);
void LCD_DrawLineV(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2, uint16_t _usColor);
void LCD_DrawLineH(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2, uint16_t _usColor);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY);
void Active_Window(u16 XL,u16 XR ,u16 YT ,u16 YB);
void DMA_Continuous_mode_size_setting(u32 set_size);
void DMA_block_mode_size_setting(u16 BWR,u16 BHR,u16 SPWR);
void DMA_Continuous_mode(void);
void DMA_Block_mode(void);
void DMA_Start_enable(void);
void XY_Coordinate(u16 X,u16 Y);
void Serial_ROM_select1(void);
void Text_Mode(void);
void Chk_DMA_Busy(void);
void DMA_Start_address_setting(u32 set_address);
void SERIAL_ROM_Dual_mode0(void);
void SERIAL_ROM_DMA_mode(void);
void SERIAL_ROM_COMMON_mode(void);
void Graphic_Mode(void);
void SERIAL_ROM_Font_mode(void);
void bmp_dispflash(u32 bmpaddress,u16 x0,u16 y0,u16 bmp_x,u16 bmp_y);
void RA8875_SetTextMode(uint8_t Mode);
u16 GUIColorToRGB(LCD_COLOR color);
void RA8875_DispChar(unsigned short c);

void BTE_Start(void);
void BTE_Wait(void);


extern __IO uint8_t s_ucRA8875Busy;

#endif


