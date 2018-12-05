#include "DIALOG.h"
#include "stm32f4xx_gpio.h"
#include "test.h"
#include "PROGBAR.h"
#include "glcd.h"
#include <stdio.h>
#include "WM.h"
#include "bsp_tft_lcd.h"
#include "main.h"
#include "GUI_task.h"
#include "main.h"
#include "ucOS_Task.h"
#include "stdio.h"
#include "GUI_typein.h"
#include "Uart_communication.h"

//FR1000_ERROR_FLAG = 0:����״̬         
//FR1000_ERROR_FLAG = 1:�ӻ����͹��������ݳ���   
//FR1000_ERROR_FLAG = 2:�ȴ��ӻ������忨��ɳ�ʱ
//FR1000_ERROR_FLAG = 3:�ȴ��ӻ�ɨ��һά�볬ʱ
//FR1000_ERROR_FLAG = 4:�ȴ��ӻ������Ѫ�ܳ�ʱ
//FR1000_ERROR_FLAG = 5:�ȴ���Ѫ��ɳ�ʱ
//FR1000_ERROR_FLAG = 6:�ȴ�ѹѪ��ɳ�ʱ
//FR1000_ERROR_FLAG = 7:�ȴ�������ɳ�ʱ
//FR1000_ERROR_FLAG = 8:���������е���˽���������ť
//FR1000_ERROR_FLAG = 9:����δɨ�� ��ɨ�����
//FR1000_ERROR_FLAG = 10:���������аγ��Լ���
//FR1000_ERROR_FLAG = 11:���������аγ���Ѫ��
//FR1000_ERROR_FLAG = 12:�ӻ����ڲ���װ�ó�ʱ
//FR1000_ERROR_FLAG = 13:û�в�����¼ �޷���ӡ
//FR1000_ERROR_FLAG = 14:û���ʿؼ�¼ �޷���ӡ
//FR1000_ERROR_FLAG = 15:��ӡ������ �޷���ӡ
//FR1000_ERROR_FLAG = 16:��ӡ��ȱֽ �޷���ӡ


const unsigned char *Error_string_ch[20] = {		
	                                           "�ӻ�ͨѶ����",	
	                                           "�ȴ��忨��ɳ���",
	                                           "�ȴ�ɨ��һά�볬ʱ",
	                                           "�ȴ������Ѫ�ܳ�ʱ",
	                                           "�ȴ���Ѫ�����Ȳ۳�ʱ",
	                                           "�ȴ�ѹѪ�������۳�ʱ",
	                                           "�ȴ��ɼ���ɳ�ʱ",
	                                           "���������е���˽�����ť",
	                                           "����δɨ�� ��ɨ�����",
	                                           "���������аγ��Լ���",
	                                           "���������аγ���Ѫ��",
	                                           "�ӻ����ڲ���װ�ó�ʱ",
	                                           "û�в��Լ�¼ �޷���ӡ",
	                                           "û���ʿؼ�¼ �޷���ӡ",
	                                           "��ӡ������ �޷���ӡ",
	                                           "��ӡ��ȱֽ �޷���ӡ",
                                           };




const unsigned char *Error_string_en[20] = {		
	                                           "Communication Error",	
	                                           "Insert Card Timeout",
	                                           "Scan ODC Timeout",
	                                           "Insert Tube Timeout",
	                                           "Absorb Blood Timeout",
	                                           "Press Blood Timeout",
	                                           "Acquisition Timeout",
	                                           "End When Test",
	                                           "Standard Curve Unscaned",
	                                           "Pull Out the Card When Test",
	                                           "Pull Out the Tube When Test",
	                                           "Adjusting Measurement Timeout",
	                                           "Unable to Print: No Record",
	                                           "Unable to Print: No Record",
	                                           "Unable to Print: Printer Stoppage",
	                                           "Unable to Print: Lack Paper",
                                           };

/**************************������ʾ�Ի��� *************************/
/**************************�Ի����ʽ��   *************************/
/**************************�ϲ���ʾ��ͼƬ *************************/
/**************************����           *************************/
/**************************ȷ�ϰ��� ��������󷵻�*****************/
void Error_popup(unsigned char Error_code)
{
	unsigned char string_length = 0;
	unsigned int  str_position_x = 0;
	unsigned int  Icon_length_x = 0;
	unsigned int  Icon_length_y = 0;
	
	//��ʾ���ֵ�ͼ
	bmp_dispflash(ADRESS_TEXT_BK,TEXT_BK_POSITION_X,TEXT_BK_POSITION_Y,LENGTH_TEXT_BK_X,LENGTH_TEXT_BK_Y);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	//������ʾ����
	if(Chinese_display == 1)
	{
		string_length = strlen(Error_string_ch[Error_code - 1]);
		str_position_x = (400 - string_length*12)/2+200;
		
		GUI_SetFont(&GUI_FontHZ24);		   //���
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(Error_string_ch[Error_code - 1],str_position_x,STR_POSITION_Y);
		
		
		
// 		RA8875_SetFrontColor(CL_BLACK);
// 		RA8875_SetBackColor(BACK_COLOUR_16);
// 		RA8875_SetFont(RA_FONT_24,1,1);
// 		RA8875_DispStr1(str_position_x, STR_POSITION_Y,Error_string_ch[Error_code - 1]);	
	}
	else
	{
		string_length = strlen(Error_string_en[Error_code - 1]);
		str_position_x = (400 - string_length*12)/2+200;
		
		
		GUI_SetFont(&GUI_Font24_ASCII);		   //���
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(Error_string_en[Error_code - 1],str_position_x,STR_POSITION_Y);
		
		
// 		RA8875_SetFrontColor(CL_BLACK);
// 		RA8875_SetBackColor(BACK_COLOUR_16);
// 		RA8875_SetFont(RA_FONT_24,1,1);
// 		RA8875_DispStr1(str_position_x, STR_POSITION_Y,Error_string_en[Error_code - 1]);	
		
	}
	//��ʾͼƬ
	Icon_length_x = (800-LENGTH_ICON_WARING_X)/2;
	bmp_dispflash(ADRESS_ICON_WARING,Icon_length_x,WARNING_POSITION_Y,LENGTH_ICON_WARING_X,LENGTH_ICON_WARING_Y);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	if(Error_code == 12)    //�������ΪLED����ʧ�� ��ʾ4��ͨ����DAֵ
	{
		GUI_SetFont(&GUI_Font24_ASCII);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("CH1:",str_position_x,STR_POSITION_Y-30);			
		GUI_DispStringAt("CH1:",str_position_x,STR_POSITION_Y-30);	
		
		GUI_DispDecAt(Test_DA_value[0],str_position_x,STR_POSITION_Y-30,4);
		GUI_DispDecAt(Test_DA_value[1],str_position_x+80,STR_POSITION_Y-30,4);
		GUI_DispDecAt(Test_DA_value[2],str_position_x+160,STR_POSITION_Y-30,4);
		GUI_DispDecAt(Test_DA_value[3],str_position_x+240,STR_POSITION_Y-30,4);
		
	}
	
	//���ȷ�ϰ�ť
	Icon_length_x = (400-LENGTH_ICON_CONFIRM_X)/2+200;
	Icon_length_y = DIALOG_Y1 - 10 - LENGTH_ICON_CONFIRM_Y;
	
	if(Chinese_display == 1)
	{
	  bmp_dispflash(ADRESS_ICON_CONFIRM,Icon_length_x,Icon_length_y,LENGTH_ICON_CONFIRM_X,LENGTH_ICON_CONFIRM_Y);	
	}
	else
	{
	  bmp_dispflash(ADRESS_EN_ICON_CONFIRM,Icon_length_x,Icon_length_y,LENGTH_ICON_CONFIRM_X,LENGTH_ICON_CONFIRM_Y);	
	}
	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	while(1)
	{
		if ((TOUCH_InRect(ts_event.x1, ts_event.y1, Icon_length_x, Icon_length_y, LENGTH_ICON_CONFIRM_Y, LENGTH_ICON_CONFIRM_X)) && (g_ucTouch == 1))
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			break;
		}
    OSTimeDlyHMSM(0, 0, 0, 300);		
	}
}

const unsigned char *Warning_string_ch[] = {	
                                              "�������β���",
	                                            "�ػ�"
                                             };

const unsigned char *Warning_string_en[] = {	
                                              "End This Measurement",
	                                            "Shut Down"
                                             };

/**************************��Ϣȷ�϶Ի���    *************************/
/**************************�Ի����ʽ��      *************************/
/**************************����              *************************/
/**************************ȷ�ϰ��� ȡ����ť *************************/
/**************************�����ť�󷵻�    *************************/
/**************************ȷ�Ϸ���1ȡ������0*************************/
unsigned char Warning_popup(unsigned char Warning_code)
{
	unsigned char string_length = 0;
	unsigned int  str_position_x = 0;
	unsigned int  Icon_length_x = 0;
	unsigned int  Icon_length_y = 0;
	unsigned int  YES_X0 = 0, YES_X1 = 0, YES_Y0 = 0, YES_Y1 = 0;
	unsigned int  NO_X0 = 0, NO_X1 = 0, NO_Y0 = 0, NO_Y1 = 0;
	 OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	
	//��ʾ���ֵ�ͼ
	bmp_dispflash(ADRESS_TEXT_BK,TEXT_BK_POSITION_X,TEXT_BK_POSITION_Y,LENGTH_TEXT_BK_X,LENGTH_TEXT_BK_Y);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	//��ʾ���� 
	if(Chinese_display == 1)
	{
		string_length = strlen(Warning_string_ch[Warning_code]);
		str_position_x = (400 - string_length*12)/2+200;	
		GUI_SetFont(&GUI_FontHZ24);		   //���
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(Warning_string_ch[Warning_code],str_position_x,STR_POSITION_Y);
		
// 		RA8875_SetFrontColor(CL_BLACK);
// 		RA8875_SetBackColor(BACK_COLOUR_16);
// 		RA8875_SetFont(RA_FONT_24,1,1);
// 		RA8875_DispStr1(str_position_x, STR_POSITION_Y,Warning_string_ch[Warning_code]);	
	}
	else
	{
		string_length = strlen(Warning_string_en[Warning_code]);
		str_position_x = (400 - string_length*12)/2+200;

		GUI_SetFont(&GUI_Font24_ASCII);		   //���
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
    GUI_DispStringAt(Warning_string_en[Warning_code],str_position_x,STR_POSITION_Y);
// 		
// 		RA8875_SetFrontColor(CL_BLACK);
// 		RA8875_SetBackColor(BACK_COLOUR_16);
// 		RA8875_SetFont(RA_FONT_24,1,1);
// 		RA8875_DispStr1(str_position_x, STR_POSITION_Y,Warning_string_en[Warning_code]);	
	}
	Icon_length_x = (800-LENGTH_ICON_WARING_X)/2;
	bmp_dispflash(ADRESS_ICON_WARING,Icon_length_x,WARNING_POSITION_Y,LENGTH_ICON_WARING_X,LENGTH_ICON_WARING_Y);	
	SERIAL_ROM_Font_mode();
  Graphic_Mode();
	
  //ȷ�Ϻ�ȡ����ť
	Icon_length_x = (800 - LENGTH_ICON_CHOICE_X)/2;
	Icon_length_y = DIALOG_Y1 - 10 - LENGTH_ICON_CHOICE_Y;	
	if(Chinese_display == 1)
	{
	  bmp_dispflash(ADRESS_ICON_CHOICE,Icon_length_x,Icon_length_y,LENGTH_ICON_CHOICE_X,LENGTH_ICON_CHOICE_Y);	
	}
	else
	{
	  bmp_dispflash(ADRESS_EN_ICON_CHOICE,Icon_length_x,Icon_length_y,LENGTH_ICON_CHOICE_X,LENGTH_ICON_CHOICE_Y);	
	}
	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
  
	YES_X0 = Icon_length_x;
	YES_Y0 = Icon_length_y;
	
	YES_X1 = Icon_length_x+CONFIRM_SIZE_X;
	YES_Y1 = Icon_length_y+CONFIRM_SIZE_Y;
	
	NO_X0 = Icon_length_x + LENGTH_ICON_CHOICE_X - CONFIRM_SIZE_X;
	NO_Y0 = Icon_length_y + LENGTH_ICON_CHOICE_Y - CONFIRM_SIZE_Y;
	
	NO_X1 = Icon_length_x + LENGTH_ICON_CHOICE_X;
	NO_Y1 = Icon_length_y + LENGTH_ICON_CHOICE_Y;
	OS_EXIT_CRITICAL();
	OSIntExit();        //���������л����ж�
	while(1)
	{
		
		//�����ȡ����ť
		if (TOUCH_InRect(ts_event.x1, ts_event.y1, YES_X0, YES_Y0,CONFIRM_SIZE_Y, CONFIRM_SIZE_X ) && (g_ucTouch == 1))
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			return 0;
		}
		//�����ȷ�ϰ�ť
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, NO_X0, NO_Y0, CONFIRM_SIZE_Y, CONFIRM_SIZE_X ) && (g_ucTouch == 1))
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			return 1;
		}
    OSTimeDlyHMSM(0, 0, 0, 50);		
	}
}