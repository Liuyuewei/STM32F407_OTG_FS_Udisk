/*-------------------------------------------------------------------------

                           外部字库 程序

                              
-------------------------------------------------------------------------*/




#include <string.h>

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
//#include "spiflash.h" 
//#include "includes.h"
#include "LCD_RA8875.h"


//extern __IO uint8_t s_ucRA8875BusyNow;
//GUI_CONTEXT *GUI_Context;


#define USRR_EXTERN_HZK  0

#define BYTES_PER_FONT      128
//U8  GUI_MoveRTL;
static U8 GUI_FontDataBuf[BYTES_PER_FONT];

/*
********************************************************************
*  特别说明：第一次使用，根据自己的屏，修正一下触摸部分，下面有两个函数
*  一个是用于修正触摸的，一个是用于官方DEMO演示的。
********************************************************************
*/
void BTE_SetSourse(U16 x0,U16 y0,U16 Layer)
{
	RA8875_WriteReg(0x54,x0);
	RA8875_WriteReg(0x55,x0>>8);
	RA8875_WriteReg(0x56,y0);
	if(Layer==1)
			RA8875_WriteReg(0x57,(y0>>8)|0x80);
	else
			RA8875_WriteReg(0x57,(y0>>8)&0x7f);
	
}
void BTE_SetDist(U16 x0,U16 y0,U16 Layer)
{
	RA8875_WriteReg(0x58,x0);
	RA8875_WriteReg(0x59,x0>>8);
	RA8875_WriteReg(0x5A,y0);
	if(Layer==1)
			RA8875_WriteReg(0x5B,(y0>>8)|0x80);
	else
			RA8875_WriteReg(0x5B,(y0>>8)&0x7f);
	
}

void BTE_SetArea(U16 x0,U16 y0)
{
	RA8875_WriteReg(0x5C,x0);
	RA8875_WriteReg(0x5D,x0>>8);
	RA8875_WriteReg(0x5E,y0);
	RA8875_WriteReg(0x5F,y0>>8);
}

//static U8 FontDataBuf[128];
void BTE_DRAW(U16 x,U16 y,U16 ch,U8 u_Font)
{
	RA8875_SetFrontColor(GUIColorToRGB(GUI_GetColor()));
	RA8875_SetBackColor(GUIColorToRGB(GUI_GetBkColor()));

	RA8875_SetFont(u_Font,1,1);
	RA8875_DispChar(ch);	
	BTE_SetSourse(x,y,0);
	BTE_SetDist(x,y,0);
	BTE_SetArea(32,32);
	//RA8875_SetFrontColor(GUIColorToRGB(GUI_CYAN)); //取的颜色  
	RA8875_WriteReg(0x51,0x82);
	BTE_Start();
	BTE_Wait();	
}




#if (USRR_EXTERN_HZK==1)
//字模数据的暂存数组,以单个字模的最大字节数为设定值, 根据自己需要用到的字体然后可以算出每个字体最大占用多少空间



void GUIPROP_X_DispChar(U16P c) 
{
    int BytesPerLine;
	  //u8 i;
    GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_Context.pAFont->p.pProp;

    //搜索定位字库数据信息 
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
		
    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        //取字模数据
        if (c < 0x80)
        {
            U8 BytesPerFont;

            pCharInfo = pProp->paCharInfo + (c - pProp->First);           
            BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine;
					
            if (BytesPerFont > BYTES_PER_FONT)
            {
                BytesPerFont = BYTES_PER_FONT;
            }
            memcpy(GUI_FontDataBuf, pCharInfo->pData, BytesPerFont);
        }
        else
        {
            u32 oft;
            u8 BytesPerFont;

            u32 base = (U32)pProp->paCharInfo->pData;
            
            BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine;
            if (BytesPerFont > BYTES_PER_FONT)
            {
                BytesPerFont = BYTES_PER_FONT;
            }

						oft = base + (((c>>8) - 0xa1) * 94 + ((c&0xff) - 0xa1)) * BytesPerFont; //中文字符地址偏移算法包括符号

						spiflash_read2buf(&GUI_FontDataBuf[0], oft, BytesPerFont);//取出字模数据  

						
        }
        
        BytesPerLine = pCharInfo->BytesPerLine;
        OldDrawMode = LCD_SetDrawMode(DrawMode);
				
        if (GUI_MoveRTL) 
				{
              GUI_Context.DispPosX -= pCharInfo->XDist * GUI_Context.pAFont->XMag;
        }

        LCD_DrawBitmap(     GUI_Context.DispPosX,
                            GUI_Context.DispPosY,
                            pCharInfo->XSize,
                            GUI_Context.pAFont->YSize,
                            GUI_Context.pAFont->XMag,
                            GUI_Context.pAFont->YMag,
                            1, /* Bits per Pixel 每像素bit数*/
                            BytesPerLine,
                            &GUI_FontDataBuf[0],
                            &LCD_BKCOLORINDEX    //背景颜色
                            );

        /* Fill empty pixel lines */
        if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) 
				{
					
          int YMag = GUI_Context.pAFont->YMag;
          int YDist = GUI_Context.pAFont->YDist * YMag;
          int YSize = GUI_Context.pAFont->YSize * YMag;
					
          if (DrawMode != LCD_DRAWMODE_TRANS) 
					{
            LCD_COLOR OldColor = GUI_GetColor();
            GUI_SetColor(GUI_GetBkColor());
            LCD_FillRect(GUI_Context.DispPosX, 
                         GUI_Context.DispPosY + YSize, 
                         GUI_Context.DispPosX + pCharInfo->XSize, 
                         GUI_Context.DispPosY + YDist);
            GUI_SetColor(OldColor);
          }
        }
				
        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
				
        if (!GUI_MoveRTL) 
				{
          GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
        }
    }
}
#else


void GUIPROP_X_DispChar(U16P c) 
{

	  GUI_COLOR GUI_Color;
	  GUI_COLOR GUI_BkColor;
	  u16 Width;
	  u8 Font;
	  int BytesPerLine;
	  //int OldTextMode;
	
    GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_Context.pAFont->p.pProp;

    //搜索定位字库数据信息 
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
		
    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        
        if (c < 0x80)
        {
            U8 BytesPerFont;

            pCharInfo = pProp->paCharInfo + (c - pProp->First);           
            BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine;
					
            if (BytesPerFont > BYTES_PER_FONT)
            {
                BytesPerFont = BYTES_PER_FONT;
            }
						
						//if(GUI_Context.pAFont==&GUI_FontHZ16)
							//BytesPerFont=0x10;						
						//else if(GUI_Context.pAFont==&GUI_FontHZ24)
							//BytesPerFont=0x20;
            memcpy(GUI_FontDataBuf, pCharInfo->pData, BytesPerFont);//要想显示ASC成功,要在SIMSUM16.C,SIMSUM24.C,SIMSUM32.C里加进字模
        }
        else
        {
            //u32 oft;
            u8 BytesPerFont;

            //u32 base = (U32)pProp->paCharInfo->pData;
            
            BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine;		 //0x48
            if (BytesPerFont > BYTES_PER_FONT)
            {
                BytesPerFont = BYTES_PER_FONT;
            }

						//oft = base + (((c>>8) - 0xa1) * 94 + ((c&0xff) - 0xa1)) * BytesPerFont; //中文字符地址偏移算法包括符号
						//spiflash_read(&GUI_FontDataBuf[0], oft, BytesPerFont);//取出字模数据  

						
        }
				
       /* Fill empty pixel lines */
        if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) 
				{
					
          int YMag = GUI_Context.pAFont->YMag;
          int YDist = GUI_Context.pAFont->YDist * YMag;
          int YSize = GUI_Context.pAFont->YSize * YMag;
					
          if (DrawMode != LCD_DRAWMODE_TRANS) 
					{
            LCD_COLOR OldColor = GUI_GetColor();
            GUI_SetColor(GUI_GetBkColor());
            LCD_FillRect(GUI_Context.DispPosX, 
                         GUI_Context.DispPosY + YSize, 
                         GUI_Context.DispPosX + pCharInfo->XSize, 
                         GUI_Context.DispPosY + YDist);
            GUI_SetColor(OldColor);
          }
        }
        
        //BytesPerLine = pCharInfo->BytesPerLine;
        OldDrawMode = LCD_SetDrawMode(DrawMode);
				
				
        //if (GUI_MoveRTL) 
				//{
						//GUI_Context.DispPosX -= pCharInfo->XDist * GUI_Context.pAFont->XMag;
        //}
        /* Fill empty pixel lines */
	
        if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) 
				{
					
          int YMag = GUI_Context.pAFont->YMag;
          int YDist = GUI_Context.pAFont->YDist * YMag;
          int YSize = GUI_Context.pAFont->YSize * YMag;
					
          if (DrawMode != LCD_DRAWMODE_TRANS) 
					{
            LCD_COLOR OldColor = GUI_GetColor();
            GUI_SetColor(GUI_GetBkColor());
            LCD_FillRect(GUI_Context.DispPosX, 
                         GUI_Context.DispPosY + YSize, 
                         GUI_Context.DispPosX + pCharInfo->XSize, 
                         GUI_Context.DispPosY + YDist);
            GUI_SetColor(OldColor);
          }
        }	
        
				
				if(OldDrawMode==GUI_DRAWMODE_NORMAL)
				{
					 RA8875_SetTextMode(0);//不穿透
				 }
				if(OldDrawMode==GUI_DRAWMODE_TRANS) 
				{
					RA8875_SetTextMode(1);//穿透
				}
				
							  RA8875_SetTextCursor(GUI_Context.DispPosX,GUI_Context.DispPosY);
							  //颜色数据转换
							  
								GUI_Color=GUI_GetColor();
								//RA8875_Color=GUIColorToRGB(GUI_Color);
								//RA8875_SetFrontColor(RA8875_Color);//设定前景颜色

							  GUI_BkColor=GUI_GetBkColor();//GUI_GetBkColor();
							  //颜色数据转换
								//RA8875_Color=GUIColorToRGB(GUI_BkColor);
								//RA8875_SetBackColor(RA8875_Color);//设定背景颜色
								
							 #if 1
				
							  if(c<0x80)
								{
										BytesPerLine = pCharInfo->BytesPerLine;
										OldDrawMode = LCD_SetDrawMode(DrawMode);
										
										if (GUI_MoveRTL) 
										{
													GUI_Context.DispPosX -= pCharInfo->XDist * GUI_Context.pAFont->XMag;
										}
										
										RA8875_SetFrontColor(GUIColorToRGB(GUI_GetColor()));
										RA8875_SetBackColor(GUIColorToRGB(GUI_GetBkColor()));

										LCD_DrawBitmap(     GUI_Context.DispPosX,
																				GUI_Context.DispPosY,
																				pCharInfo->XSize,
																				GUI_Context.pAFont->YSize,
																				GUI_Context.pAFont->XMag,
																				GUI_Context.pAFont->YMag,
																				1, /* Bits per Pixel 每像素bit数*/
																				BytesPerLine,
																				&GUI_FontDataBuf[0],
																				&LCD_BKCOLORINDEX    //背景颜色
																				);
									


										if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) 
										{
											
											int YMag = GUI_Context.pAFont->YMag;
											int YDist = GUI_Context.pAFont->YDist * YMag;
											int YSize = GUI_Context.pAFont->YSize * YMag;
											
											if (DrawMode != LCD_DRAWMODE_TRANS) 
											{
												LCD_COLOR OldColor = GUI_GetColor();
												GUI_SetColor(GUI_GetBkColor());
												LCD_FillRect(GUI_Context.DispPosX, 
																		 GUI_Context.DispPosY + YSize, 
																		 GUI_Context.DispPosX + pCharInfo->XSize, 
																		 GUI_Context.DispPosY + YDist);
												GUI_SetColor(OldColor);
											}
										}
										
										
										LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */										
								}
								else 
								{
										if(GUI_Context.pAFont==&GUI_FontHZ16)
										{
											Font=0;
											Width=16;
										}
											//RA8875_SetFont(RA_FONT_16,GUI_Context.pAFont->XMag,GUI_Context.pAFont->YMag);

										else if(GUI_Context.pAFont==&GUI_FontHZ24)
										{
											Font=1;
											Width=24;
										}
												//RA8875_SetFont(RA_FONT_24,GUI_Context.pAFont->XMag,GUI_Context.pAFont->YMag);

										else if(GUI_Context.pAFont==&GUI_FontHZ32)
										{
											Font=2;
											Width=32;
										}
												//RA8875_SetFont(RA_FONT_32,GUI_Context.pAFont->XMag,GUI_Context.pAFont->YMag); 
										
									
										//BTE_DRAW(GUI_Context.DispPosX,GUI_Context.DispPosY,c,Font);
										RA8875_SetFrontColor(GUIColorToRGB(GUI_GetColor()));
										RA8875_SetBackColor(GUIColorToRGB(GUI_GetBkColor()));

										RA8875_SetFont(Font,GUI_Context.pAFont->XMag,GUI_Context.pAFont->YMag);
										RA8875_DispChar(c);	
										BTE_SetSourse(GUI_Context.DispPosX,GUI_Context.DispPosX,0);
										BTE_SetDist(GUI_Context.DispPosX,GUI_Context.DispPosX,0);
										BTE_SetArea(Width+GUI_Context.pAFont->XMag,Width+GUI_Context.pAFont->YMag);
										//RA8875_SetFrontColor(GUIColorToRGB(GUI_CYAN)); //取的颜色  
										RA8875_WriteReg(0x51,0x82);
										BTE_Start();
										BTE_Wait();	
										
										
										
								}
								
								//恢复颜色
								GUI_SetBkColor(GUI_BkColor);
								//GUI_SetBkColor(GUI_INVALID_COLOR);
							  GUI_SetColor(GUI_Color);

								
								#endif		

        if (!GUI_MoveRTL) 
				{
          GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag; //X轴计算
        }
    }		
		
}
#endif
/*********************************************************************

*

*       GUIPROP_GetCharDistX

*/

int GUIPROP_X_GetCharDistX(U16P c) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_Context.pAFont->p.pProp;
    
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_Context.pAFont->XMag : 0;
}

