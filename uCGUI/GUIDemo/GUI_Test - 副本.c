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
#include "FPGA_deal.h"

#define TEST_timeout   50000
#define Display_time   300          //��ʾ��Ϣˢ�¼��ʱ��  ��λΪms

#define ID_FRAMEWIN_END              (GUI_ID_USER + 0x30)
#define ID_TEXT_END                  (GUI_ID_USER + 0x31)
#define ID_BUTTON_END_YES            (GUI_ID_USER + 0x32)
#define ID_BUTTON_END_NO             (GUI_ID_USER + 0x33)

#define ID_WINDOW_INFORMATION        (GUI_ID_USER + 0x70)
#define ID_EDIT_INFORMATION          (GUI_ID_USER + 0x71)
#define ID_TEXT_INFORMATION          (GUI_ID_USER + 0x72)

#define ID_INFORMATION_WORD_1        (GUI_ID_USER + 0x73)
#define ID_INFORMATION_WORD_2        (GUI_ID_USER + 0x74)
#define ID_INFORMATION_WORD_3        (GUI_ID_USER + 0x75)
#define ID_INFORMATION_WORD_4        (GUI_ID_USER + 0x76)
#define ID_INFORMATION_WORD_5        (GUI_ID_USER + 0x77)
#define ID_INFORMATION_WORD_6        (GUI_ID_USER + 0x78)
#define ID_INFORMATION_WORD_7        (GUI_ID_USER + 0x79)
#define ID_INFORMATION_WORD_8        (GUI_ID_USER + 0x7A)
#define ID_INFORMATION_WORD_9        (GUI_ID_USER + 0x7B)
#define ID_INFORMATION_WORD_A        (GUI_ID_USER + 0x7C)
#define ID_INFORMATION_WORD_B        (GUI_ID_USER + 0x7D)

#define ID_INFORMATION_KEY_1         (GUI_ID_USER + 0x40)
#define ID_INFORMATION_KEY_2         (GUI_ID_USER + 0x41)
#define ID_INFORMATION_KEY_3         (GUI_ID_USER + 0x42)
#define ID_INFORMATION_KEY_4         (GUI_ID_USER + 0x43)
#define ID_INFORMATION_KEY_5         (GUI_ID_USER + 0x44)
#define ID_INFORMATION_KEY_6         (GUI_ID_USER + 0x45)
#define ID_INFORMATION_KEY_7         (GUI_ID_USER + 0x46)
#define ID_INFORMATION_KEY_8         (GUI_ID_USER + 0x47)
#define ID_INFORMATION_KEY_9         (GUI_ID_USER + 0x48)
#define ID_INFORMATION_KEY_0         (GUI_ID_USER + 0x49)
#define ID_INFORMATION_KEY_DELETE    (GUI_ID_USER + 0x4A)
#define ID_INFORMATION_KEY_Q         (GUI_ID_USER + 0x4B)
#define ID_INFORMATION_KEY_W         (GUI_ID_USER + 0x4C)
#define ID_INFORMATION_KEY_E         (GUI_ID_USER + 0x4D)
#define ID_INFORMATION_KEY_R         (GUI_ID_USER + 0x4E)
#define ID_INFORMATION_KEY_T         (GUI_ID_USER + 0x4F)
#define ID_INFORMATION_KEY_Y         (GUI_ID_USER + 0x50)
#define ID_INFORMATION_KEY_U         (GUI_ID_USER + 0x51)
#define ID_INFORMATION_KEY_I         (GUI_ID_USER + 0x52)
#define ID_INFORMATION_KEY_O         (GUI_ID_USER + 0x53)
#define ID_INFORMATION_KEY_P         (GUI_ID_USER + 0x54)
#define ID_INFORMATION_KEY_PLUS      (GUI_ID_USER + 0x55)
#define ID_INFORMATION_KEY_CAPITIAL  (GUI_ID_USER + 0x56)
#define ID_INFORMATION_KEY_A         (GUI_ID_USER + 0x57)
#define ID_INFORMATION_KEY_S         (GUI_ID_USER + 0x58)
#define ID_INFORMATION_KEY_D         (GUI_ID_USER + 0x59)
#define ID_INFORMATION_KEY_F         (GUI_ID_USER + 0x5A)
#define ID_INFORMATION_KEY_G         (GUI_ID_USER + 0x5B)
#define ID_INFORMATION_KEY_H         (GUI_ID_USER + 0x5C)
#define ID_INFORMATION_KEY_J         (GUI_ID_USER + 0x5D)
#define ID_INFORMATION_KEY_K         (GUI_ID_USER + 0x5E)
#define ID_INFORMATION_KEY_L         (GUI_ID_USER + 0x5F)
#define ID_INFORMATION_KEY_ENTER     (GUI_ID_USER + 0x60)
#define ID_INFORMATION_KEY_SWITCH    (GUI_ID_USER + 0x61)
#define ID_INFORMATION_KEY_Z         (GUI_ID_USER + 0x62)
#define ID_INFORMATION_KEY_X         (GUI_ID_USER + 0x63)
#define ID_INFORMATION_KEY_C         (GUI_ID_USER + 0x64)
#define ID_INFORMATION_KEY_V         (GUI_ID_USER + 0x65)
#define ID_INFORMATION_KEY_B         (GUI_ID_USER + 0x66)
#define ID_INFORMATION_KEY_N         (GUI_ID_USER + 0x67)
#define ID_INFORMATION_KEY_M         (GUI_ID_USER + 0x68)
#define ID_INFORMATION_KEY_SLASH     (GUI_ID_USER + 0x69)
#define ID_INFORMATION_KEY_POINT     (GUI_ID_USER + 0x6A)
#define ID_INFORMATION_KEY_EXIT      (GUI_ID_USER + 0x6B)


extern const char * py_ime(char *);
char chinese_string[255];
char input_string[8]= "de";   //���6����ĸ


unsigned char test_choice = 0;
unsigned char start_flag = 0; 
//start_flag = 0:����״̬         
//start_flag = 1:FPGA���͹��������ݳ���   
//start_flag = 2:�ȴ�FPGA�����Լ�������״̬��ʱ
//start_flag = 3:�ȴ�FPGA����SPOT CODEɨ����ɳ�ʱ
//start_flag = 4:�ȴ�FPGA������Ѫ�ܲ���ɹ�״̬��ʱ
//start_flag = 5:�ȴ�FPGA����ѪҺ������Ȳ����״̬��ʱ 
//start_flag = 6:�ȴ�FPGA����ѪҺѹ����������״̬��ʱ 
//start_flag = 7:�ȴ�FPGA�����������״̬��ʱ
//start_flag = 7:���������е���˽���������ť

uint8_t FPGA_IO_data = 0;                               //����FPGA���͹���������
uint8_t FPGA_IO_flag = 0;                               //��1���ʾFPGA�������ݴ������ 
uint16_t measure_data_CH1[1000] = {0};
uint16_t measure_data_CH2[1000] = {0};
uint16_t measure_data_CH3[1000] = {0};
uint16_t measure_data_CH4[1000] = {0};
uint16_t SPI1_data_spi[4000] = {0};
uint16_t SPOT_CODE_Data[2] = {0};                       //�洢SPOT CODE���� 
uint32_t Test_time = 0;                                 //��ʱʱ�� 

/*���������ť����Ҫ�õ���һЩ����*/
uint8_t END_Measure = 0;                                //0�������ť�󲻽������� 1�������ť��������� 

uint8_t  *Error_warning[] = {
// 	                             "The communication with FPGA is wrong!",
// 	                             "Waiting for card insert is timeout!",
// 	                             "Waiting for spot code is timeout!",
// 	                             "Waiting for tube insert is timeout!",
// 	                             "Waiting for suck blood is timeout!",
// 	                             "Waiting for press blood is timeout!",
// 	                             "Waiting for measure finish is timeout!",
	                             "Communication Wrong!",
	                             "Card Insert Timeout!",
	                             "Spot Code Timeout!",
	                             "Tube Insert Timeout!",
	                             "Suck Blood Timeout!",
	                             "Press Blood Timeout!",
	                             "Measure Timeout!",
                              };


static const char Lower_keys[]=
{
    '0','1','2','3','4','5','6','7','8','9',
    'q','w','e','r','t','y','u','i','o','p',
    'a','s','d','f','g','h','j','k','l','-',
    'z','x','c','v','b','n','m',' ','.','?'
};
static const char Capital_keys[]=
{
    '!','@','#','$','%','^','&','*','(',')',
    'Q','W','E','R','T','Y','U','I','O','P',
    'A','S','D','F','G','H','J','K','L',':',
    'Z','X','C','V','B','N','M','<','>','/'
};


/*********************************************************************
*
*       _aDialogCreate_warning       ������ʾ���� ��һ����ť�����ô��� 
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate_warning[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 300, 210, 200, 120},
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 10, 20, 180, 20},
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 50, 50, 80, 40},
};

/*********************************************************************
*
*       _cbDialog_warning ������ʾ���ڻص�����
*/
static void _cbDialog_warning(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;
	hItem = pMsg->hWin;
	
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                          //�Դ�����ʾ���ڵĳ�ʼ�� �����ı���Ϣ��
		
    FRAMEWIN_SetText(hItem, "WARNING");
    FRAMEWIN_SetTitleHeight(hItem, 20);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
	  TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER);
	  if(start_flag > 1)
    {
		  TEXT_SetText(hItem, Error_warning[start_flag-1]);
		}
		else
		{
			TEXT_SetText(hItem, "UNKNOWN WRONG!");
		}
	  
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetText(hItem, "OK");
	  
    break;
	  case WM_NOTIFY_PARENT:                        //�԰�����Ϣ������Ӧ
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
	  switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //�ڰ����ɿ�ʱ������Ӧ
				switch(Id) 
				{
					case ID_BUTTON_0: 
					{
						GUI_EndDialog(hItem,0);               //�����ô�����ʾ����
						break;
					}
			 }
			 break;
	  }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       CreateFramewin    ����������ʾ���� �ɻص����������ý���
*/
WM_HWIN CreateFramewin_warning(void);
WM_HWIN CreateFramewin_warning(void) 
{
  WM_HWIN hWin;
  WM_SetCreateFlags(WM_CF_MEMDEV);
  hWin = GUI_ExecDialogBox(_aDialogCreate_warning, GUI_COUNTOF(_aDialogCreate_warning), _cbDialog_warning, WM_HBKWIN, 0, 0);
  return hWin;
}


/*********************************************************************
*
*       _aDialogCreate_end       ������ʾ���� ��һ����ť�����ô��� 
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate_end[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_END, 300, 210, 200, 120},
//   { TEXT_CreateIndirect, "Text", ID_TEXT_END, 10, 20, 180, 40},
//   { BUTTON_CreateIndirect, "Button", ID_BUTTON_END_YES, 30, 70, 80, 40},
//   { BUTTON_CreateIndirect, "Button", ID_BUTTON_END_NO, 120, 70, 80, 40},
  { TEXT_CreateIndirect, "Text", ID_TEXT_END, 10, 20, 180, 20},
  { BUTTON_CreateIndirect, "��", ID_BUTTON_END_YES, 5, 45, 90, 50},
  { BUTTON_CreateIndirect, "��", ID_BUTTON_END_NO, 105, 45, 90, 50},
};

/*********************************************************************
*
*       _cbDialog_end ������ʾ���ڻص�����
*/
static void _cbDialog_end(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;
	hItem = pMsg->hWin;
	
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                          //�Դ�����ʾ���ڵĳ�ʼ�� �����ı���Ϣ��
		
    FRAMEWIN_SetText(hItem, "WARNING");
    FRAMEWIN_SetTitleHeight(hItem, 20);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_END);
	  TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER);
 	  TEXT_SetText(hItem, "END THIS MEASUREMENT?");
	  	
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_END_YES);
	  BUTTON_SetTextColor(hItem,0,GUI_WHITE);
	  BUTTON_SetTextColor(hItem,1,GUI_WHITE);
 	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
	  sprintf(chinese_string,"%s",py_ime(input_string));
	  chinese_string[2] = '\0';
	  BUTTON_SetText(hItem, (const char*)chinese_string);
//     BUTTON_SetText(hItem, "YES");
	
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_END_NO);
	  BUTTON_SetTextColor(hItem,0,GUI_WHITE);
	  BUTTON_SetTextColor(hItem,1,GUI_WHITE);
	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
	  sprintf(chinese_string,"%s",py_ime(input_string));
	  chinese_string[2] = '\0';
	  BUTTON_SetText(hItem, (const char*)chinese_string);
//     BUTTON_SetText(hItem, "NO");
	  
    break;
	  case WM_NOTIFY_PARENT:                        //�԰�����Ϣ������Ӧ
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
	  switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //�ڰ����ɿ�ʱ������Ӧ
				switch(Id) 
				{
					case ID_BUTTON_END_YES: 
					{
						END_Measure = 1;
						GUI_EndDialog(hItem,0);               //�����ô�����ʾ����
						break;
					}
					case ID_BUTTON_END_NO: 
					{
						END_Measure = 0;
						GUI_EndDialog(hItem,0);               //�����ô�����ʾ����
						break;
					}
					default:
						break;
			 }
			 break;
	  }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*   CreateFramewin_End �������������ť��Ĵ������  �ɻص����������ý���
*/
WM_HWIN CreateFramewin_End(void);
WM_HWIN CreateFramewin_End(void) 
{
  WM_HWIN hWin;
  WM_SetCreateFlags(WM_CF_MEMDEV);
  hWin = GUI_ExecDialogBox(_aDialogCreate_end, GUI_COUNTOF(_aDialogCreate_end), _cbDialog_end, WM_HBKWIN, 0, 0);
  return hWin;
}


static const GUI_WIDGET_CREATE_INFO _aDialogCreate_INFORMATION[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_INFORMATION, 20, 50, 640, 345},
  { TEXT_CreateIndirect, "Text", ID_TEXT_INFORMATION, 20, 0, 600, 30},
  { EDIT_CreateIndirect, "Edit", ID_EDIT_INFORMATION, 20, 35, 600, 30},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_WORD_1, 20, 70, 50, 50},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_WORD_2, 75, 70, 50, 50},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_KEY_1,  20, 125, 50, 50},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_KEY_Q,  20, 180, 50, 50},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_KEY_A,  20, 235, 50, 50},
  { BUTTON_CreateIndirect, "Button", ID_INFORMATION_KEY_Z,  20, 290, 50, 50},
  // USER START (Optionally insert additional widgets)
  // USER END
};


static void _cbDialog_INFORMATION(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:    
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION);
	  TEXT_SetFont(hItem,&GUI_FontHZ_SimSun_24);//GUI_FontHZ_SimSun_24);
    TEXT_SetText(hItem, "��������");
//     TEXT_SetText(hItem, "text");
	
	  //EDIT_SetDefaultFont(&GUI_FontHZ24);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION);
 	  EDIT_SetFont(hItem,&GUI_FontHZ24);
    //EDIT_SetText(hItem, "���¸�");
     //EDIT_SetText(hItem, "Edit");
	  
 	  BUTTON_SetDefaultFont(&GUI_FontHZ24);
	  //GUI_UC_SetEncodeUTF8();
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_WORD_1);
// 	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
    BUTTON_SetText(hItem, "��");
//     BUTTON_SetText(hItem, "Q");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_WORD_2);
// 	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
    BUTTON_SetText(hItem, "��");
//     BUTTON_SetText(hItem, "1");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_KEY_1);
	  BUTTON_SetFont(hItem,&GUI_Font24_ASCII);
    BUTTON_SetText(hItem, "1");
//     BUTTON_SetText(hItem, "Z");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_KEY_Q);
	  BUTTON_SetFont(hItem,&GUI_Font24_ASCII);
    BUTTON_SetText(hItem, "Q");
//     BUTTON_SetText(hItem, "A");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_KEY_A);
	  BUTTON_SetFont(hItem,&GUI_Font24_ASCII);
    BUTTON_SetText(hItem, "A");
//     BUTTON_SetText(hItem, "Q");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_KEY_Z);
	  BUTTON_SetFont(hItem,&GUI_Font24_ASCII);
    BUTTON_SetText(hItem, "Z");
//     BUTTON_SetText(hItem, "Q");
    
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //�ڰ����ɿ�ʱ������Ӧ
				switch(Id) 
				{
					case ID_INFORMATION_WORD_1: 
					{
						hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION);
         	  EDIT_SetFont(hItem,&GUI_FontHZ24);
            EDIT_SetText(hItem, "���¸�");               
						break;
					}
					case ID_INFORMATION_WORD_2: 
					{
						;;        
						break;
					}
					default:
						break;
			 }
			 break;
	  }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


WM_HWIN CreateFramewin_INFORMATION(void);
WM_HWIN CreateFramewin_INFORMATION(void) 
{
  WM_HWIN hWin;
  WM_SetCreateFlags(WM_CF_MEMDEV);
  hWin = GUI_ExecDialogBox(_aDialogCreate_INFORMATION, GUI_COUNTOF(_aDialogCreate_INFORMATION), _cbDialog_INFORMATION, WM_HBKWIN, 0, 0);
  return hWin;
}





/*�����������*/
WM_HWIN Create_Test(void) {
  
	BUTTON_Handle btn_Test_hl,btn_Set_hl,btn_Control_hl,btn_History_hl;
	static unsigned char btn_Xsize = 200, btn_Ysize = 60;
	static int i = 0;
	int key_num = 0;
	
	//��ʾ������
	bmp_dispflash(0xBB810,0,0);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	do
	{
		if (!GUI_Exec()) 
		{
			GUI_WaitEvent();      
		}
		
// 		key_num =  GUI_GetKey();
		
		if (TOUCH_InRect(ts_event.x1, ts_event.y1, 5, 420, 60, 200) && (g_ucTouch == 1)) //��test���� ������Ӧ
		{
			test_choice = 0;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 200, 420, 60, 200) && (g_ucTouch == 1))//��setting���� 
		{
			test_choice = 1;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 395, 420, 60, 200) && (g_ucTouch == 1))//��control����
		{
			test_choice = 2;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 590, 420, 60, 200) && (g_ucTouch == 1))//��history���� 
		{
			test_choice = 3;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))   //�����ʼ���԰�ť
		{
			TEST_begin();                             //���ò��Ժ���
			bmp_dispflash(0xBB810,0,0);	              //�˴β��Խ��� ������ʾ���Խ��� 
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,50, 100, 115) && (g_ucTouch == 1))   //���������Ϣ��ť
		{
			TEST_type_information();
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		
		
		if(test_choice != 0)                       //�㿪�������������� ��Ҫ������Ӧ���� �� ���� �ʿ� ��ʷ��¼��ѯ��
		{
// 			BUTTON_Delete(btn_Test_hl);
// 			BUTTON_Delete(btn_Set_hl);
// 			BUTTON_Delete(btn_Control_hl);
// 			BUTTON_Delete(btn_History_hl);
			break;
		}
	}
	while(1);
	return hWin;
}

void TEST_begin(void)
{
	FONT_T tFont;			/* ����һ������ṹ���������������������� */
	static unsigned long time_out_old = 0; //����ˢ��ʱʹ��
	unsigned char Prompt_flag = 0;         //0����ʱ������ʾ����ʾΪ�� 1����ʱ������ʾ����ʾΪ��Ӧ����
	unsigned int i = 0,data_receive = 0,j,k,l,m,data_in;  //��������ʱʹ�õ�һЩ�м����
	unsigned char ADC_num = 0;
	char buffer[64] ={0};
	char clear[64] = "         ";
	unsigned char spot_code_flag = 0;                     //0��spot code����δ��� 1��spot code���ݴ������
  unsigned char spot_code_h = 0;                        //0��spot code��λ����δ���  1��spot code��λ���ݴ��������
	unsigned char spot_code_l = 0;                        //0��spot code��λ����δ���  1��spot code��λ���ݴ��������
  PROGBAR_Handle progbar_test;                          //��������ʾ�ľ��
  unsigned char spot_code_str[16] = {0};                //����spot code��ʾ�Ĳ�������
  unsigned char progbar_data = 0;                       //����������
  unsigned char close_window_time = 0;     //��ʾ10�ιرպںи���
  
	// 	 �����������
	{
		tFont.usFontCode = FC_ST_16;	/* ������� 16���� */ // ���ִ�СΪ16*16 ��ĸ��СΪ ��8��16
		tFont.usTextColor = CL_BLACK;	/* ������ɫ */
		tFont.usBackColor = CL_WHITE;	/* ���ֱ�����ɫ */
		tFont.usSpace = 0;				/* ���ּ�࣬��λ = ���� */
	}
	
	/*��λFPGA״̬*/
	PG6_IO_reset();
	delay_nms(30);
	printf("\r\nFPGA RESET!\r\n");
	PG6_IO_set();
  
	/*�Ա������г�ʼ��*/
	start_flag = 0;
	FPGA_IO_flag = 0;
	FPGA_IO_data = 0;
	
	/*�����������ؼ� ��ʾ���Խ���*/
  progbar_test = PROGBAR_Create(40,200, 500, 20, WM_CF_SHOW);  
	PROGBAR_EnableMemdev(progbar_test);
  //PROGBAR_SetFont(progbar_test, &tFont);
  PROGBAR_SetBarColor(progbar_test, 0, GUI_GREEN);       //��ˢ�µĽ�������ɫ
  PROGBAR_SetBarColor(progbar_test, 1, GUI_LIGHTBLUE);       //δˢ�µĽ�������ɫ
  PROGBAR_SetTextAlign(progbar_test, GUI_TA_CENTER);     //����λ�ڽ���������
	PROGBAR_SetMinMax(progbar_test,0,100);                 //�����������ֵ����Сֵ
	PROGBAR_SetTextColor(progbar_test,0,GUI_BLACK);        //����������ɫ
	PROGBAR_SetTextColor(progbar_test,1,GUI_BLACK);        //����������ɫ
	//PROGBAR_SetValue(progbar_test, 0);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
	//GUI_Exec();
	
	//CreateFramewin_warning();
	
	//RA8875_DispStr(50, 0, "Insert Card!            ", &tFont);	
	
	//���Ϳ�ʼ�����ź�
	send_command_FPGA(INSERT_CARD_COMMAND);
	
// 	Test_time = TEST_timeout;
// 	time_out_old = TEST_timeout;
	while(FPGA_IO_flag == 0)                       //�ȴ�FPGA�����Լ�������״̬����
	{
// 		if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
// 		{
// 			start_flag = 2;
// 			break;
// 		}
// 		if((time_out_old - Test_time < 400)&&(Prompt_flag == 0))
// 		{
// 			RA8875_FillRect(100,180,100,60,0xF79E);
// 			Prompt_flag = 1;
// 		}
// 		else if(((time_out_old - Test_time > 400)&&(time_out_old - Test_time < 800)&&(Prompt_flag == 1)))
// 		{
// 			bmp_dispflash(0xBB810,0,0);	
// 			SERIAL_ROM_Font_mode();
// 			Graphic_Mode();
// 			Prompt_flag = 0;
// 		}
// 		else if(time_out_old - Test_time > 800)
// 		{
// 			time_out_old = Test_time;
// 			Prompt_flag = 0;
// 		}
    /*��˸�忨��־ ��ʾ�û������Լ���*/
		RA8875_FillRect(100,170,100,60,0xF79E);
		OSTimeDlyHMSM(0, 0, 0, Display_time);
		bmp_dispflash(0xBB810,0,0);	
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
		OSTimeDlyHMSM(0, 0, 0, Display_time);
		/*ɨ���Ƿ����˽���������ť �������˽�����ť���ٴ�ȷ�Ͻ������� ������ò�������*/
		END_Measure = 0;
		if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
		{
			PROGBAR_Delete(progbar_test);
	    PROGBAR_DisableMemdev(progbar_test);
			CreateFramewin_End();
			//test_choice = 2;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		if(END_Measure == 1)
		{
			start_flag = 8;
			break;
		}
	}
	
	//ˢ�� ��ֹ���治��
	bmp_dispflash(0xBB810,0,0);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	if(start_flag == 0)
	{
		if(FPGA_IO_flag == 1)                           //���յ���FPGA����������
		{
			if(FPGA_IO_data == CARD_FINISHED_STATUES)     //���յ��Լ�������ɹ�״̬����
			{
				//RA8875_DispStr(50, 0, "SACN SPOT CODE!                 ", &tFont);
				//PROGBAR_SetValue(progbar_test, 10);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
	      //GUI_Exec();
				FPGA_IO_flag = 0;
				send_command_FPGA(SCAN_SPOT_CODE_COMMAND); 
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "CARD INSERT IS WRONG!     ", &tFont);
				delay_nms(5000);
			}
		}
  }
	
	if(start_flag == 0)
	{                       
    Test_time = TEST_timeout;
	  time_out_old = TEST_timeout;
    while(FPGA_IO_flag == 0)                       //�ȴ�FPGA����SPOT CODEɨ��״̬����
	  {
		  if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
		  {
			  start_flag = 3;
			  break;
		  }
	  }
		
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == SPOT_FINISHED_STATUES)     //���յ�SPOT CODEɨ�����״̬ ��ʼ��ȡSPOT CODE����
			{
				while(spot_code_flag == 0)
				{
					delay_nus(500);
					data_receive = SPI1_Receive();                           //����SPOT CODE����
					if(data_receive != 0xFFFF)
					{
						if(((data_receive&0x5400) == 0x5400))                                         //���յ���λ����
						{
							spot_code_h = 1;
							SPOT_CODE_Data[1] = data_receive;
						}
						else if(((data_receive&0x5000) == 0x5000))                                    //���յ���λ����
						{
							spot_code_l = 1;
							SPOT_CODE_Data[0] = data_receive;
						}
						if ((spot_code_h == 1)&&(spot_code_l == 1))      //�ߵ�λ���ݾ��ѽ���
						{
							spot_code_flag = 1;
							//PROGBAR_SetValue(progbar_test, 20);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
	            //GUI_Exec();
						}
					}
				}
				if(1)                                        //��SOPT CODE���ݽ����ж� ���� �������
				{
					;;
				}
				/*��SPOT CODE������ʾ����Ļ��*/
				for(i = 0;i < 16;i++)
				{
					if((SPOT_CODE_Data[1]&0x8000) != 0)
					{
						spot_code_str[i] = '1';
					}
					else
					{
						spot_code_str[i] = '0';
					}
					SPOT_CODE_Data[1] = SPOT_CODE_Data[1] << 1;					
				}			
				//RA8875_DispStr(0, 200, "HIGH BYTE", &tFont);
				sprintf(&buffer[0], "%d", SPOT_CODE_Data[0]);
				//RA8875_DispStr(100, 200, spot_code_str, &tFont);
				
				for(i = 0;i < 16;i++)
				{
					if((SPOT_CODE_Data[0]&0x8000) != 0)
					{
						spot_code_str[i] = '1';
					}
					else
					{
						spot_code_str[i] = '0';
					}
					SPOT_CODE_Data[0] = SPOT_CODE_Data[0] << 1;					
				}			
				//RA8875_DispStr(0, 300, "LOW BYTE", &tFont);
				sprintf(&buffer[1], "%d", SPOT_CODE_Data[0]);
				//RA8875_DispStr(100, 300, spot_code_str, &tFont);
				
				
        //PROGBAR_SetValue(progbar_test, 30);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        //GUI_Exec();
				
				//RA8875_DispStr(50, 0, "INSERT THE TUBE!            ", &tFont);
				FPGA_IO_flag = 0;
				send_command_FPGA(INSERT_TUBE_COMMAND);      //���Ͳ��Ѫ�ܵ�ָ��
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "SACN SPOT DATA IS WRONG!     ", &tFont);
  		  delay_nms(5000);
			}
		}
  }
	                   
	if(start_flag == 0)
	{            
// 		Test_time = TEST_timeout;
// 	  time_out_old = TEST_timeout;
// 		Prompt_flag = 0;
    while(FPGA_IO_flag == 0)                       //�ȴ�FPGA������Ѫ�ܲ���״̬
	  {
// 		  if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
// 		  {
// 			  start_flag = 4;
// 			  break;
// 		  }
// 			if((time_out_old - Test_time < 400)&&(Prompt_flag == 0))
// 			{
// 				//RA8875_DispStr(50, 0, "Insert Card!            ", &tFont);
// 				RA8875_FillRect(300,170,100,40,0xF79E);
// 				Prompt_flag = 1;
// 			}
// 			else if(((time_out_old - Test_time > 400)&&(time_out_old - Test_time < 800)&&(Prompt_flag == 1)))
// 			{
// 				//RA8875_DispStr(50, 0, "                        ", &tFont);
// 				bmp_dispflash(0xBB810,0,0);	
// 				SERIAL_ROM_Font_mode();
// 				Graphic_Mode();
// 				Prompt_flag = 0;
// 			}
// 			else if(time_out_old - Test_time > 800)
// 			{
// 				time_out_old = Test_time;
// 				Prompt_flag = 0;
// 			}
	    /*��˸��Ѫ�ܱ�־ ��ʾ�û������Ѫ��*/
			RA8875_FillRect(300,170,100,40,0xF79E);
			OSTimeDlyHMSM(0, 0, 0, Display_time);
			bmp_dispflash(0xBB810,0,0);	
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			OSTimeDlyHMSM(0, 0, 0, Display_time);
			END_Measure = 0;
			if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
			{
				PROGBAR_Delete(progbar_test);
	      PROGBAR_DisableMemdev(progbar_test);
				CreateFramewin_End();
				//test_choice = 2;
			  ts_event.x1 = -1;
			  ts_event.y1 = -1;
			  g_ucTouch = 0;
			  _StoreUnstable(-1, -1);
			}
			if(END_Measure == 1)
			{
				start_flag = 8;
				break;
			}
	  }
		//ˢ�� ��ֹ���治��
		bmp_dispflash(0xBB810,0,0);	
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
		
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == TUBE_FINISHED_STATUES)     //���յ���Ѫ�ܲ�������ָ�� ���ͳ�Ѫ�����Ȳ�ָ��
			{
				//RA8875_DispStr(50, 0, "SUCK THE BLOOD!                 ", &tFont);
				FPGA_IO_flag = 0;
				//PROGBAR_SetValue(progbar_test, 40);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        //GUI_Exec();
				for(close_window_time=0;close_window_time<6;close_window_time++)
				{
				  RA8875_FillRect(450,170,100,125,0xF79E);
					OSTimeDlyHMSM(0, 0, 0, Display_time);
					END_Measure = 0;
					if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
					{
				    PROGBAR_Delete(progbar_test);
	          PROGBAR_DisableMemdev(progbar_test);
						CreateFramewin_End();
						//test_choice = 2;
						ts_event.x1 = -1;
						ts_event.y1 = -1;
						g_ucTouch = 0;
						_StoreUnstable(-1, -1);
					}
					if(END_Measure == 1)
					{
						start_flag = 8;
						break;
					}
				  bmp_dispflash(0xBB810,0,0);	
				  SERIAL_ROM_Font_mode();
				  Graphic_Mode();
					OSTimeDlyHMSM(0, 0, 0, Display_time);
// 				  Test_time = 400;
// 					while(Test_time != 0);
// 				  Test_time = 400;
// 					while(Test_time != 0);
				}
				
				
				send_command_FPGA(SUCK_BLOOD_COMMAND);      //���ͳ�Ѫ�����Ȳ۵�ָ��
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "TUBE INSERT IS WRONG!          ", &tFont);
  		  delay_nms(5000);
			}
		}
	}
                      
	if(start_flag == 0)
	{
		bmp_dispflash(0x00,0,0);
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
    PROGBAR_SetValue(progbar_test, 0);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
    GUI_Exec();		
		Test_time = TEST_timeout;
    while(FPGA_IO_flag == 0)                       //�ȴ�FPGA������Ѫ�����Ȳ۵�״̬
	  {
		  if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
		  {
			  start_flag = 5;
			  break;
		  }
			OSTimeDlyHMSM(0, 0, 0, 50);
			END_Measure = 0;
			if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
			{
				PROGBAR_Delete(progbar_test);
	      PROGBAR_DisableMemdev(progbar_test);
				CreateFramewin_End();
				//test_choice = 2;
				ts_event.x1 = -1;
				ts_event.y1 = -1;
				g_ucTouch = 0;
				_StoreUnstable(-1, -1);
			}
			if(END_Measure == 1)
			{
				start_flag = 8;
				break;
			}
	  }
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == SUCK_FINISHED_STATUES)     //��Ѫ�����Ȳ����
			{
				Heat_enable();                              //��ʼ����ѪҺ
				delay_nms(50);	
				Heat_disable();
				//RA8875_DispStr(50, 0, "PRESS THE BLOOD!                 ", &tFont);
				FPGA_IO_flag = 0;
				PROGBAR_SetValue(progbar_test, 10);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        GUI_Exec();
				send_command_FPGA(HEAT_FINISHED_COMMAND);      //����ѪҺ��� ���Խ�ѪҺѹ�������
				
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "SUKE BLOOD IS WRONG!          ", &tFont);
  		  delay_nms(5000);
			}
		}
	}		
               
	if(start_flag == 0)
	{                        
		Test_time = TEST_timeout;
    while(FPGA_IO_flag == 0)                       //�ȴ�FPGA����ѪҺ��������۳ɹ�״̬
	  {
		  if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
		  {
			  start_flag = 6;
			  break;
		  }
			OSTimeDlyHMSM(0, 0, 0, 50);
			END_Measure = 0;
			if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
			{
				PROGBAR_Delete(progbar_test);
	      PROGBAR_DisableMemdev(progbar_test);
				CreateFramewin_End();
				//test_choice = 2;
				ts_event.x1 = -1;
				ts_event.y1 = -1;
				g_ucTouch = 0;
				_StoreUnstable(-1, -1);
			}
			if(END_Measure == 1)
			{
				start_flag = 8;
				break;
			}
	  }
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == PRESS_FINISHED_STATUES)     //ѹѪ�������۳ɹ�
			{
				//send_command_FPGA(START_MEASURE_COMMAND);      //��ʼ����
				//RA8875_DispStr(50, 0, "START MEASURE!                 ", &tFont);
				PROGBAR_SetValue(progbar_test, 30);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        GUI_Exec();
				FPGA_IO_flag = 0;
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "PRESS BLOOD IS WRONG!          ", &tFont);
  		  delay_nms(5000);
			}
		}	
	}
	
	if(start_flag == 0)
	{
		Test_time = TEST_timeout;
    while(FPGA_IO_flag == 0)                       //�ȴ�FPGA�����������ָ��
	  {
		  if(Test_time == 0)                           //�ȴ���ʱ ��ʾ������Ϣ������
		  {
			  start_flag = 7;
			  break;
		  }
			OSTimeDlyHMSM(0, 0, 0, 50);
			END_Measure = 0;
			if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
			{
				PROGBAR_Delete(progbar_test);
	      PROGBAR_DisableMemdev(progbar_test);
				CreateFramewin_End();
				//test_choice = 2;
				ts_event.x1 = -1;
				ts_event.y1 = -1;
				g_ucTouch = 0;
				_StoreUnstable(-1, -1);
			}
			if(END_Measure == 1)
			{
				start_flag = 8;
				break;
			}
	  }
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == MEASURE_FINISHED_STATUES)     //FPGA�������
			{
				//RA8875_DispStr(50, 0, "FPGA FINISHED!                 ", &tFont);
				FPGA_IO_flag = 0;
				PROGBAR_SetValue(progbar_test, 50);                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        GUI_Exec();
				send_command_FPGA(RESET_COMMAND); 
			}
			else 
			{
				FPGA_IO_flag = 0;
				if(start_flag != 8)
				{
				  start_flag = 1;
				}
				//RA8875_DispStr(50, 0, "FPGA READ DATA WRONG!          ", &tFont);
  		  delay_nms(5000);
			}
		}	
		
		j = 0;k = 0;l = 0;m = 0;
				
		//����4096��FPGA�����������ݲ��ֱ�洢��4�������� ͨ�����ݵĸ�4λ�ж�����λ��
		for(i = 0;i < 4096;i++)
		{
			delay_nus(100);
			data_receive = SPI1_Receive();
			SPI1_data_spi[i] = data_receive;
			ADC_num = data_receive >> 12;
			switch(ADC_num)
			{
				case 1:
				{
					measure_data_CH1[j] = data_receive & 0x0FFF;
					j = j+1;
					break;
				}
				case 2:
				{
					measure_data_CH2[k] = data_receive & 0x0FFF;
					k = k+1;
					break;
				}
				case 3:
				{
					measure_data_CH3[l] = data_receive & 0x0FFF;
					l = l+1;
					break;
				}
				case 4:
				{
					measure_data_CH4[m] = data_receive & 0x0FFF;
					m = m+1;
					break;
				}
				default:
				{
					i = i-1;
				}					
			}
			if(i%135 == 0)
			{
				PROGBAR_SetValue(progbar_test, (50+(i/81)));                     //���ý���ֵ ��ʾΪ�ٷֱ� ���ڷ�ΧΪ(0��100)������ʾΪ�˰ٷֱ�
        GUI_Exec();
			}
		}
	
		//RA8875_DispStr(50, 0, "END THIS MEASUREMENT!                 ", &tFont);				
		delay_nms(500);	
		//RA8875_DispStr(50, 0, "BEGIN THE NEXT MEASUREMENT!                 ", &tFont);
		delay_nms(500);
  }
	else if(start_flag != 8)
	{
	  PROGBAR_Delete(progbar_test);
	  PROGBAR_DisableMemdev(progbar_test);
		CreateFramewin_warning();
	}
	PROGBAR_Delete(progbar_test);
	PROGBAR_DisableMemdev(progbar_test);
	start_flag = 1;
	FPGA_IO_flag = 0;
	FPGA_IO_data = 0;
	PG6_IO_reset();
	delay_nms(30);
	printf("\r\nFPGA RESET!\r\n");
	PG6_IO_set();
 	send_command_FPGA(RESET_COMMAND);
}

void Test_time_measure(void)
{
	if(Test_time != 0)
	{
		Test_time--;
	}
}


void TEST_type_information(void)
{
	BUTTON_Handle btn_handle_0;
	EDIT_Handle   edit_handle_0;
  RA8875_FillRect(35,85,290,640,0xF79E);    //ˢ�� ���ǲ�����ʾ�� ��ʾ¼����Ϣ�Ի���
	
// 	sprintf(chinese_string,"%s",py_ime(input_string));
// //   sprintf(chinese_string,"%s",py_ime(input_string));
// //   chinese_string[2] = '\0';
	btn_handle_0 = BUTTON_Create(100,100,50,50,0xEF,WM_CF_SHOW);
  BUTTON_SetFont(btn_handle_0,&GUI_FontHZ24);
  BUTTON_SetText(btn_handle_0, "��");
	
	edit_handle_0 = EDIT_Create(200,300,100,30,0xFF,100,WM_CF_SHOW);
	EDIT_SetFont(edit_handle_0,&GUI_FontHZ24);
  EDIT_SetText(edit_handle_0, "���¸�");
	
	GUI_Exec();
//  	CreateFramewin_INFORMATION();
}


// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/