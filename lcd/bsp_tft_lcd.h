/*
*********************************************************************************************************
*
*	ģ������ : TFTҺ����ʾ������ģ��
*	�ļ����� : LCD_tft_lcd.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-08-21 armfly  ST�̼���汾 V3.5.0�汾��
*		v2.0    2011-10-16 armfly  ST�̼���汾 V3.5.0�汾���淶�����ӿڣ��Ż��ڲ��ṹ
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _BSP_TFT_LCD_H
#define _BSP_TFT_LCD_H

#include "stm32f4xx.h"
#include "bsp_tft_lcd.h"

/* ����LCD��ʾ����ķֱ��� */
#define LCD_30_HEIGHT	240		/* 3.0����� �߶ȣ���λ������ */
#define LCD_30_WIDTH	400		/* 3.0����� ��ȣ���λ������ */

#define LCD_43_HEIGHT	272		/* 4.3����� �߶ȣ���λ������ */
#define LCD_43_WIDTH	480		/* 4.3����� ��ȣ���λ������ */

#define LCD_70_HEIGHT	480		/* 7.0����� �߶ȣ���λ������ */
#define LCD_70_WIDTH	800		/* 7.0����� ��ȣ���λ������ */

#define GUI_BLUE          0xFF0000
#define GUI_GREEN         0x00FF00
#define GUI_RED           0x0000FF
#define GUI_CYAN          0xFFFF00
#define GUI_MAGENTA       0xFF00FF
#define GUI_YELLOW        0x00FFFF
#define GUI_LIGHTBLUE     0xFF8080
#define GUI_LIGHTGREEN    0x80FF80
#define GUI_LIGHTRED      0x8080FF
#define GUI_LIGHTCYAN     0xFFFF80
#define GUI_LIGHTMAGENTA  0xFF80FF
#define GUI_LIGHTYELLOW   0x80FFFF
#define GUI_DARKBLUE      0x800000
#define GUI_DARKGREEN     0x008000
#define GUI_DARKRED       0x000080
#define GUI_DARKCYAN      0x808000
#define GUI_DARKMAGENTA   0x800080
#define GUI_DARKYELLOW    0x008080
#define GUI_WHITE         0xFFFFFF
#define GUI_LIGHTGRAY     0xD3D3D3
#define GUI_GRAY          0x808080
#define GUI_DARKGRAY      0x404040
#define GUI_BLACK         0x000000

#define GUI_BROWN         0x2A2AA5

/* ֧�ֵ�����оƬID */
enum
{
	IC_5420		= 0x5420,
	IC_4001		= 0x4001,
	IC_61509 	= 0xB509,
	IC_8875 	= 0x0075
};

#define CHIP_STR_5420	"SPFD5420A"
#define CHIP_STR_4001	"OTM4001A"
#define CHIP_STR_61509	"R61509V"
#define CHIP_STR_8875	"RA8875"

/* LCD ��ɫ���룬CL_��Color�ļ�д */
enum
{
	CL_WHITE        = 0xFFFF,	/* ��ɫ */
	CL_BLACK        = 0x0000,	/* ��ɫ */
	CL_GREY         = 0xF7DE,	/* ��ɫ */
	CL_BLUE         = 0x001F,	/* ��ɫ */
	CL_BLUE2        = 0x051F,	/* ǳ��ɫ */
	CL_RED          = 0xF800,	/* ��ɫ */
	CL_MAGENTA      = 0xF81F,	/* ����ɫ�����ɫ */
	CL_GREEN        = 0x07E0,	/* ��ɫ */
	CL_CYAN         = 0x7FFF,	/* ����ɫ����ɫ */
	CL_YELLOW       = 0xFFE0,	/* ��ɫ */
	CL_BLUE3        = 0x00CD,
	CL_MASK			= 0x9999	/* ��ɫ���룬�������ֱ���͸�� */
	
};

/* ������� */

/* ������� */
enum
{
	FC_ST_16 = 0,		/* ����15x16���� ����x�ߣ� */
	FC_ST_12 = 1		/* ����12x12���� ����x�ߣ� */
};

/* �������Խṹ, ����LCD_DispStr() */
typedef struct
{
	uint16_t usFontCode;	/* ������� 0 ��ʾ16���� */
	uint16_t usTextColor;	/* ������ɫ */
	uint16_t usBackColor;	/* ���ֱ�����ɫ��͸�� */
	uint16_t usSpace;		/* ���ּ�࣬��λ = ���� */
}FONT_T;

/* ��������� */
#define BRIGHT_MAX		255
#define BRIGHT_MIN		0
#define BRIGHT_DEFAULT	200
#define BRIGHT_STEP		5

/* �ɹ��ⲿģ����õĺ��� */
void LCD_InitHard(void);
void LCD_GetChipDescribe(char *_str);
uint16_t LCD_GetHeight(void);
uint16_t LCD_GetWidth(void);
void LCD_DispOn(void);
void LCD_DispOff(void);
void LCD_ClrScr(uint16_t _usColor);
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY);
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor);
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint8_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void LCD_DrawIcon(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void LCD_DrawIconActive(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void LCD_SetBackLight(uint8_t _bright);
uint8_t LCD_GetBackLight(void);

/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
extern uint16_t g_ChipID;			/* ����оƬID */
extern uint16_t g_LcdHeight;		/* ��ʾ���ֱ���-�߶� */
extern uint16_t g_LcdWidth;			/* ��ʾ���ֱ���-��� */

#endif


