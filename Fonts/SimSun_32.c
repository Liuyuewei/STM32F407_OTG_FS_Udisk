// 新宋体　16x16
// 本字库文件由“uC/GUI汉字库生成器 V1.0”创建
// 作者：黄培雄  E-mail: dialsun@sohu.com

#include "GUI.H"
#include "GUIType.h"

//#include "GUICharPex.h"

#ifndef GUI_FLASH
	#define GUI_FLASH
#endif


extern GUI_FLASH const GUI_FONT GUI_FontHZ32;
extern const GUI_CHARINFO GUI_Font32_ASCII_CharInfo[95];







GUI_FLASH const GUI_CHARINFO GUI_FontHZ_32_[] =
{
    { 32, 32, 4, (void GUI_FLASH *)GUI_FontHZ32_Flash_BaseAddr},//汉字在FLASH中的偏移量
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_32_Prop_a1 =
{
	0xa1a1,
	0xf7fe,
	&GUI_FontHZ_32_[0],
	(void *)0
};



GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_32_Prop_ASC =
{
	0x20,
  0x7F,
	&GUI_Font32_ASCII_CharInfo[0], // address of first character
	(void *)&GUI_FontHZ_32_Prop_a1 // pointer to next GUI_FONT_PROP
};

GUI_FLASH const GUI_FONT GUI_FontHZ32 =
{
	GUI_FONTTYPE_PROP_X, // type of font
	32, // height of font
	32, // space of font y
	1,  // magnification x
	1,  // magnification y
	(void GUI_FLASH *)&GUI_FontHZ_32_Prop_ASC
};



