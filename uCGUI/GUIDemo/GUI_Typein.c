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
#include "stdio.h"
#include "GUI_typein.h"


#define TYPE_BUTTON_CLR_PRESSED     (0xB8B8B8)           //�������µı���ɫ
#define TYPE_BUTTON_CLR_UNPRESSED   (0xDCDADA)           //����δ���µı���ɫ
#define NUM_MAX_LENGTH              (15)                 //���뻼�߱����󳤶� 
#define NAME_MAX_LENGTH             (5)                  //���뻼��������󳤶� 


extern const char * py_ime(char *);


unsigned char capital_lower_flag = 0;      //��Сд��־λ 0��Сд 1����д
unsigned char Shift_flag = 0;              //���Shift֮�� �����Ͻ��д�Сд�л� �����ַ���ģʽ�� ���һ�κ󼴻ָ���Сд����ģʽ

static const char Lower_keys[]=
{
    '1','2','3','4','5','6','7','8','9','0','<',
    'q','w','e','r','t','y','u','i','o','p','-',
    '^','a','s','d','f','g','h','j','k','l',' ',
    ' ','z','x','c','v','b','n','m','/','.',' '
};
static const char Capital_keys[]=
{
    '1','2','3','4','5','6','7','8','9','0','<',
    'Q','W','E','R','T','Y','U','I','O','P','-',
    '^','A','S','D','F','G','H','J','K','L',' ',
    ' ','Z','X','C','V','B','N','M','/','.',' '
};

static const char* Lower_button_keys[44]=
{
    "1","2","3","4","5","6","7","8","9","0","<--",
    "q","w","e","r","t","y","u","i","o","p","-",
    "Cap","a","s","d","f","g","h","j","k","l","Enter",
    "Shift","z","x","c","v","b","n","m","/",".","Exit"
};

static const char* Capital_button_keys[44]=
{
    "1","2","3","4","5","6","7","8","9","0","<--",
    "Q","W","E","R","T","Y","U","I","O","P","-",
    "Cap","A","S","D","F","G","H","J","K","L","Enter",
    "Shift","Z","X","C","V","B","N","M","/",".","Exit"
};

#define ID_WINDOW_INFORMATION        (GUI_ID_USER + 0x70)
#define ID_EDIT_INFORMATION_name     (GUI_ID_USER + 0x71)
#define ID_EDIT_INFORMATION_number   (GUI_ID_USER + 0x7E)
#define ID_TEXT_INFORMATION_name     (GUI_ID_USER + 0x72)
#define ID_TEXT_INFORMATION_number   (GUI_ID_USER + 0x7F)


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
#define ID_INFORMATION_KEY_BACKSPACE (GUI_ID_USER + 0x4A)
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
#define ID_INFORMATION_KEY_SHIFT     (GUI_ID_USER + 0x61)
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



#define KEY_ROW0  72
#define KEY_ROW1  130
#define KEY_ROW2  188
#define KEY_ROW3  246
#define KEY_ROW4  304
#define KEY_WIDTH 55
#define KEY_HIGTH 55
#define KEY_LINE(x)  (7+57*(x-1))

static BUTTON_Handle btn_handle_WORD[11];
static EDIT_Handle   edit_handle_en;




static const GUI_WIDGET_CREATE_INFO _aDialogCreate_INFORMATION[] = {
  { WINDOW_CreateIndirect, " ",        ID_WINDOW_INFORMATION,     15, 48, 663, 365},
//   { TEXT_CreateIndirect,   "���߱��", ID_TEXT_INFORMATION_number, 5,  0, 200,  30},
//   { EDIT_CreateIndirect,   " ",        ID_EDIT_INFORMATION_number, 5, 30, 300,  35},
//   { TEXT_CreateIndirect,   "��������", ID_TEXT_INFORMATION_name, 355,  0, 200,  30},
//   { EDIT_CreateIndirect,   " ",        ID_EDIT_INFORMATION_name, 355, 30, 300,  35},

  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_1,  (25+KEY_LINE(2)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_2,  (25+KEY_LINE(3)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_3,  (25+KEY_LINE(4)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_4,  (25+KEY_LINE(5)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_5,  (25+KEY_LINE(6)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_6,  (25+KEY_LINE(7)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_7,  (25+KEY_LINE(8)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_8,  (25+KEY_LINE(9)), KEY_ROW0, (KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_9,  (25+KEY_LINE(10)), KEY_ROW0,(KEY_WIDTH+5), KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_A,  (25+KEY_LINE(11)),  KEY_ROW0, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "      ", ID_INFORMATION_WORD_B, 5, KEY_ROW0, (KEY_WIDTH+30), KEY_HIGTH},
  { BUTTON_CreateIndirect, "1", ID_INFORMATION_KEY_1,          KEY_LINE(1) ,  KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "2", ID_INFORMATION_KEY_2,          KEY_LINE(2) ,  KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "3", ID_INFORMATION_KEY_3,          KEY_LINE(3) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "4", ID_INFORMATION_KEY_4,          KEY_LINE(4) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "5", ID_INFORMATION_KEY_5,          KEY_LINE(5) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "6", ID_INFORMATION_KEY_6,          KEY_LINE(6) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "7", ID_INFORMATION_KEY_7,          KEY_LINE(7) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "8", ID_INFORMATION_KEY_8,          KEY_LINE(8) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "9", ID_INFORMATION_KEY_9,          KEY_LINE(9) , KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "0", ID_INFORMATION_KEY_0,          KEY_LINE(10), KEY_ROW1, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "<--", ID_INFORMATION_KEY_BACKSPACE,  KEY_LINE(11), KEY_ROW1, 80,        KEY_HIGTH},
  { BUTTON_CreateIndirect, "q", ID_INFORMATION_KEY_Q,          KEY_LINE(1) , KEY_ROW2, 80,        KEY_HIGTH},
  { BUTTON_CreateIndirect, "w", ID_INFORMATION_KEY_W,          (25+KEY_LINE(2)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "e", ID_INFORMATION_KEY_E,          (25+KEY_LINE(3)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "r", ID_INFORMATION_KEY_R,          (25+KEY_LINE(4)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "t", ID_INFORMATION_KEY_T,          (25+KEY_LINE(5)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "y", ID_INFORMATION_KEY_Y,          (25+KEY_LINE(6)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "u", ID_INFORMATION_KEY_U,          (25+KEY_LINE(7)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "i", ID_INFORMATION_KEY_I,          (25+KEY_LINE(8)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "o", ID_INFORMATION_KEY_O,          (25+KEY_LINE(9)) , KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "p", ID_INFORMATION_KEY_P,          (25+KEY_LINE(10)), KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "-", ID_INFORMATION_KEY_PLUS,       (25+KEY_LINE(11)), KEY_ROW2, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "Cap", ID_INFORMATION_KEY_CAPITIAL, KEY_LINE(1) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "a", ID_INFORMATION_KEY_A,          KEY_LINE(2) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "s", ID_INFORMATION_KEY_S,          KEY_LINE(3) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "d", ID_INFORMATION_KEY_D,          KEY_LINE(4) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "f", ID_INFORMATION_KEY_F,          KEY_LINE(5) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "g", ID_INFORMATION_KEY_G,          KEY_LINE(6) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "h", ID_INFORMATION_KEY_H,          KEY_LINE(7) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "j", ID_INFORMATION_KEY_J,          KEY_LINE(8) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "k", ID_INFORMATION_KEY_K,          KEY_LINE(9) , KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "l", ID_INFORMATION_KEY_L,          KEY_LINE(10), KEY_ROW3, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "Enter", ID_INFORMATION_KEY_ENTER,  KEY_LINE(11), KEY_ROW3, 80,        KEY_HIGTH},
  { BUTTON_CreateIndirect, "Shift", ID_INFORMATION_KEY_SHIFT,  KEY_LINE(1) , KEY_ROW4, 80,        KEY_HIGTH},
  { BUTTON_CreateIndirect, "z", ID_INFORMATION_KEY_Z,          (25+KEY_LINE(2)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "x", ID_INFORMATION_KEY_X,          (25+KEY_LINE(3)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "c", ID_INFORMATION_KEY_C,          (25+KEY_LINE(4)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "v", ID_INFORMATION_KEY_V,          (25+KEY_LINE(5)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "b", ID_INFORMATION_KEY_B,          (25+KEY_LINE(6)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "n", ID_INFORMATION_KEY_N,          (25+KEY_LINE(7)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "m", ID_INFORMATION_KEY_M,          (25+KEY_LINE(8)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "/", ID_INFORMATION_KEY_SLASH,      (25+KEY_LINE(9)) , KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, ".", ID_INFORMATION_KEY_POINT,      (25+KEY_LINE(10)), KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
  { BUTTON_CreateIndirect, "Exit", ID_INFORMATION_KEY_EXIT,    (25+KEY_LINE(11)), KEY_ROW4, KEY_WIDTH, KEY_HIGTH},
};



static unsigned char  name_ch_length = 0;	
unsigned char name_chinese_putin[40] = {0};
unsigned char name_chinese_test[40] = {0};
unsigned char number_test[30] = {0};
unsigned char name_chinese[40] = {0};
unsigned char number_putin[30] = {0};
unsigned char number_en[30] = {0};
static unsigned char number_length = 0;
static unsigned char backspace_state = 0;       //ɾ���������λ�� Ĭ��Ϊƴ������ 0��ƴ������ 1��������ʾ��
int i = 11, j = 0 , k = 0;
static unsigned char KEYBOARD_inital = 0;
unsigned char tmp_char[2] = {0};
unsigned int key = 0, key_num = 0;
unsigned char name_flag = 0;
unsigned char py_string[8] = {0},py_string_length = 0;
unsigned char py_chinese[255] = {0}, py_chinese_length = 0;     //�洢ƴ����Ӧ�ĺ����ַ���  һ�������������ֽ�������� ǰ��Ϊ���ֽں���Ϊ���ֽ� ��ͬ
	//���������ɰ���7������
unsigned char py_char[2] = 0;
unsigned char chinese_choice_flag = 0;                         //0������ѡ��ť��δ����  1������ѡ��ť�Ѵ���
unsigned char py_chinese_display[20][18] = {0};                 //��ҳ��ʾʱ�����ʾ10ҳ ÿҳΪ9������ ����ʱ��ʾΪ�� ��βΪ��ҳ��
unsigned char py_chinese_page = 0 , py_chinese_page_current = 0, py_chinese_page_flag = 0;
//           ��Ҫ��ʾ�ĺ��ֵ�ҳ��    Ŀǰ��ʾ�ĺ��ֵ�ҳ��        ���ַ�ҳ��־λ 0��δ��ҳ 1����ҳ
unsigned char button_string[10] = {0},button_str_length = 0;
unsigned char putin_flag = 0;               //0�������Ϊ���  1�������Ϊ���� ͨ�������Ӧ����ʾ���л�

static void Parameter_Inital(void)
{
	if(test_putin_flag == 1)
	{
	  for(i = 0;i < NUM_MAX_LENGTH;i++)
	  {
		  name_chinese[i] = name_chinese_test[i];
		  number_en[i] = number_test[i];
	  }
  }
	else
	{
	  for(i = 0;i < NUM_MAX_LENGTH;i++)
	  {
		  name_chinese[i] = name_chinese_putin[i];
		  number_en[i] = number_putin[i];
	  }
  }
	name_ch_length = strlen(name_chinese)/2;
	number_length = strlen(number_en);
	
	putin_flag = 0;
	backspace_state = 0;
	KEYBOARD_inital = 0;
	key = 0;
	key_num = 0;
	for(j = 0; j<8; j++)
	{
		py_string[j] = 0;
	}
	py_string_length = 0;
	name_flag = 0;
	for(j = 0; j<255; j++)
	{
		py_chinese[j] = 0;
	}
	py_chinese_length = 0;
	chinese_choice_flag = 0;
	for(j = 0; j<20; j++)
	{
		for(k = 0;k<18;k++)
		{
			py_chinese_display[j][k] = 0;
		}
	}
	py_chinese_page = 0 ;
	py_chinese_page_current = 0;
	py_chinese_page_flag = 0;
	i = 11;
	j = 0;
	k = 0;
}

unsigned char test_putin_flag = 0;
WM_HWIN KEY_BOARD_hItem;
bool Parameter_Inital_flag=0;
 bool WM_NOTIFY_BACK_flag=0;
static void _cbDialog_INFORMATION(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	unsigned int i = 0;
	unsigned char tmp_char[2] = {0};
  GUI_RECT rClient;
  OS_CPU_SR  cpu_sr;
  static flag=0;

  KEY_BOARD_hItem = pMsg->hWin;

  hItem = pMsg->hWin;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
	{
		Parameter_Inital();
		
		WINDOW_SetDefaultBkColor(0xC6C4C4);
		if(test_putin_flag == 1)
		{	
			putin_flag = 0; 
			if(Chinese_display == 1)
			{
				TEXT_CreateAsChild(355,0,200,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_name,WM_CF_SHOW,"��������",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_name);
				TEXT_SetFont(hItem,&GUI_FontHZ_SimSun_24);
				
				TEXT_CreateAsChild(5,0,200,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_number,WM_CF_SHOW,"���߱��",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_number);
				TEXT_SetFont(hItem,&GUI_FontHZ_SimSun_24);
			}
			else
			{
				TEXT_CreateAsChild(355,0,200,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_name,WM_CF_SHOW,"Patient Name",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_name);
				TEXT_SetFont(hItem,&GUI_Font24_ASCII);
				
				TEXT_CreateAsChild(5,0,200,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_number,WM_CF_SHOW,"Patient Number",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_number);
				TEXT_SetFont(hItem,&GUI_Font24_ASCII);
			}
			EDIT_CreateAsChild(5,30,300,35,KEY_BOARD_hItem,ID_EDIT_INFORMATION_number,WM_CF_SHOW,300);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_number);
			EDIT_SetFont(hItem,&GUI_Font24_ASCII);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,0xE2E2E2);
			EDIT_SetMaxLen(hItem,300);
			EDIT_SetText(hItem,number_en);
			
			EDIT_CreateAsChild(355,30,300,35,KEY_BOARD_hItem,ID_EDIT_INFORMATION_name,WM_CF_SHOW,300);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
			EDIT_SetFont(hItem,&GUI_FontHZ24);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,0xE2E2E2);
			EDIT_SetMaxLen(hItem,300);
			EDIT_SetText(hItem,name_chinese);
	  }
		else if(test_putin_flag == 2)
		{
			putin_flag = 1;
			if(Chinese_display == 1)
			{
				TEXT_CreateAsChild(5,0,550,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_name,WM_CF_SHOW,"��������",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_name);
				TEXT_SetFont(hItem,&GUI_FontHZ_SimSun_24);
			}
			else
			{
				TEXT_CreateAsChild(5,0,550,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_name,WM_CF_SHOW,"Patient Name",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_name);
				TEXT_SetFont(hItem,&GUI_Font24_ASCII);
			}
			
			EDIT_CreateAsChild(5,30,650,35,KEY_BOARD_hItem,ID_EDIT_INFORMATION_name,WM_CF_SHOW,300);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
			EDIT_SetFont(hItem,&GUI_FontHZ24);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,0xE2E2E2);
			EDIT_SetMaxLen(hItem,300);
			EDIT_SetText(hItem,name_chinese);
			
			for(i = 0; i < 10; i++)
			{
				WM_DisableWindow((WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i))));
			}
		}
		else if(test_putin_flag == 3)
		{
			putin_flag = 0; 
			if(Chinese_display == 1)
			{
				TEXT_CreateAsChild(5,0,550,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_number,WM_CF_SHOW,"���߱��",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_number);
				TEXT_SetFont(hItem,&GUI_FontHZ_SimSun_24);
			}
			else
			{
				TEXT_CreateAsChild(5,0,550,30,KEY_BOARD_hItem,ID_TEXT_INFORMATION_number,WM_CF_SHOW,"Patient Number",TEXT_CF_LEFT);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFORMATION_number);
				TEXT_SetFont(hItem,&GUI_Font24_ASCII);
			}
						
			EDIT_CreateAsChild(5,30,650,35,KEY_BOARD_hItem,ID_EDIT_INFORMATION_number,WM_CF_SHOW,300);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_number);
			EDIT_SetFont(hItem,&GUI_Font24_ASCII);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,0xE2E2E2);
			EDIT_SetMaxLen(hItem,300);
			EDIT_SetText(hItem,number_en);
		}		
		
		//����������̵ĸ�������
		for(i = 0; i < 44; i++)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
 	    BUTTON_SetFont(hItem,&GUI_Font24_ASCII);                                  //����
			BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_UNPRESSED);                               //δ���µı���ɫ
			BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
		  BUTTON_SetTextColor(hItem,0,0xD62A86);
		  BUTTON_SetTextColor(hItem,1,0xD62A86);
			if(capital_lower_flag == 1)
			{
				if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_CAPITIAL)                         //��д״̬�¸ı�Cap�����ı���ɫ
				{
					BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_PRESSED);                                 //δ���µı���ɫ
			    BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
				}
				BUTTON_SetText(hItem,Capital_button_keys[i]);
			}
			else
			{
				if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_CAPITIAL)
				{
					BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_UNPRESSED);                               //δ���µı���ɫ
			    BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
				}
				BUTTON_SetText(hItem,Lower_button_keys[i]);
			}
		}
		
		hItem = pMsg->hWin;
		for(i = 0; i < 10; i++)
		{
			btn_handle_WORD[i] = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_WORD_1 + i));
			//btn_handle_WORD[i] = BUTTON_CreateAsChild((25+KEY_LINE((i+2))),72,60,55,hItem,(ID_INFORMATION_WORD_1+i),BUTTON_CF_HIDE);
			WM_HideWindow(btn_handle_WORD[i]);
			BUTTON_SetBkColor(btn_handle_WORD[i],0,TYPE_BUTTON_CLR_UNPRESSED);                                 //δ���µı���ɫ
			BUTTON_SetBkColor(btn_handle_WORD[i],1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
			BUTTON_SetFont(btn_handle_WORD[i],&GUI_FontHZ24);
		}
		
		btn_handle_WORD[10] = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_WORD_B));
		WM_HideWindow(btn_handle_WORD[10]);
		BUTTON_SetBkColor(btn_handle_WORD[10],BUTTON_CI_DISABLED,TYPE_BUTTON_CLR_UNPRESSED);		            //����ʱ����ɫ
		WM_DisableWindow(btn_handle_WORD[10]);
		BUTTON_SetFont(btn_handle_WORD[10],&GUI_Font24_ASCII);
		BUTTON_SetTextColor(btn_handle_WORD[10],BUTTON_CI_DISABLED,GUI_BLACK);

    break;
	}
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //�ڰ����ɿ�ʱ������Ӧ
			  OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		    OSIntNesting++;
				switch(Id) 
				{
					case ID_INFORMATION_KEY_EXIT:           //�˳���ť�Ĵ���     ͨ��EXIT��ť�����˳� �򲻽������ݱ���
					{
						hItem = pMsg->hWin;
						test_putin_flag = 0;
						ts_event.x1 = -1;
						ts_event.y1 = -1;
						g_ucTouch = 0;
						_StoreUnstable(-1, -1);		
						GUI_EndDialog(hItem,0);               //���������봰��  
						break;
					}
					case ID_INFORMATION_KEY_SHIFT:         //shift��ť�Ĵ���
					{		
						if(putin_flag == 0)                   //Ӣ��ģʽ�� �л���־λ���л�������ʾ�ַ�
						{
							if(capital_lower_flag == 0)         //ֻ�д�дδ����ʱ���ܽ���shift�Ĳ���
							{
						    Shift_flag = 1 - Shift_flag;
								if(Shift_flag == 1)
								{
									for(i = 0; i < 44; i++)
									{
										hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
										BUTTON_SetText(hItem,Capital_button_keys[i]);
										if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_SHIFT)
										{
											BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_PRESSED);                                 //δ���µı���ɫ
											BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
										}
									}
								}
								else 
								{
									for(i = 0; i < 44; i++)
									{
										hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
										BUTTON_SetText(hItem,Lower_button_keys[i]);
										if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_SHIFT)
										{
											BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_UNPRESSED);                                 //δ���µı���ɫ
											BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
										}
									}
								}	
							}
						}		
						break;
					}
					case ID_INFORMATION_KEY_BACKSPACE:     //��ɾ��ť�Ĵ���
					{
            if(putin_flag == 1)			
						{							
							if(strlen(py_string) == 0)
							{
								if((strlen(name_chinese)) > 0)
								{
									name_chinese[(strlen(name_chinese)-1)] = 0;
									name_chinese[(strlen(name_chinese)-1)] = 0;
									name_ch_length = name_ch_length-1;
									hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
									EDIT_SetText(hItem,name_chinese);
								}
							}
							else
							{
									py_string[(strlen(py_string)-1)] = 0;
									py_string_length = strlen(py_string);
									BUTTON_SetText(btn_handle_WORD[10],py_string);
									goto PY_START;
							}
					  }
						else
						{							
							if((strlen(number_en)) > 0)
							{
								number_en[(strlen(number_en)-1)] = 0;
								number_length = number_length-1;
							  hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_number);
								EDIT_SetText(hItem,number_en);
							}
						}
						break;
					}
					case ID_INFORMATION_KEY_PLUS:         //plus��ť�Ĵ���
					{		
						break;
					}
					case ID_INFORMATION_KEY_CAPITIAL:      //��д������ť�Ĵ��� ���д�Сд�л� ��ת�������ϵ���ĸ��ʾ
					{
            capital_lower_flag = 1 - capital_lower_flag;			
            if(capital_lower_flag == 1)
						{
							Shift_flag = 0;
					    for(i = 0; i < 44; i++)
							{
								hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
							  BUTTON_SetText(hItem,Capital_button_keys[i]);
								if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_CAPITIAL)
								{
									BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_PRESSED);                                 //δ���µı���ɫ
									BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
								}
							}
						}
						else 
            {
							for(i = 0; i < 44; i++)
							{
								hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
							  BUTTON_SetText(hItem,Lower_button_keys[i]);
								if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_CAPITIAL)
								{
									BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_UNPRESSED);                               //δ���µı���ɫ
									BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
								}
							}
						}							
						break;
					}
					case ID_EDIT_INFORMATION_number:      //������߱����ʾ��
					{	
            putin_flag = 0;						
						break;
					}
					case ID_EDIT_INFORMATION_name:       //�������������ʾ��
					{		
            putin_flag = 1;	
						break;
					}
					case ID_INFORMATION_KEY_ENTER:         //enter��ť�Ĵ���
					{
						if(test_putin_flag == 1)
						{
							for(i = 0;i < NUM_MAX_LENGTH;i++)
							{
								name_chinese_test[i] = name_chinese[i];
								number_test[i] = number_en[i];
							}
					  }
						else
						{
							for(i = 0;i < NUM_MAX_LENGTH;i++)
							{
								name_chinese_putin[i] = name_chinese[i];
								number_putin[i] = number_en[i];								
							}
						}
						test_putin_flag = 0;
            hItem = pMsg->hWin;
						ts_event.x1 = -1;
						ts_event.y1 = -1;
						g_ucTouch = 0;
						_StoreUnstable(-1, -1);	
						GUI_EndDialog(hItem,0);               //���������봰��  
						break;
					}
					case ID_INFORMATION_KEY_SLASH:         //'/'��ť�Ĵ���
					{		
						break;
					}
					case ID_INFORMATION_KEY_POINT:         //'.'��ť�Ĵ���
					{		
            
						break;
					}
					default:
					{
						if(                               //Ӣ���ַ��Ĵ���ʽ
							(Id > ID_INFORMATION_KEY_BACKSPACE)
				     &(Id < ID_INFORMATION_KEY_SLASH))
						{
							if(putin_flag == 1)
							{
								if(py_string_length < 8)
								{
									//ȡ��������������ַ������������Ӣ���ַ���ʾ��Ӣ���ַ�����
									py_string[py_string_length] = Lower_keys[Id - ID_INFORMATION_KEY_1];
									py_string_length = py_string_length + 1;
									WM_ShowWindow(btn_handle_WORD[10]);
									BUTTON_SetText(btn_handle_WORD[10],py_string);                       

	PY_START:
									sprintf(py_chinese,"%s",py_ime(py_string));                         //��ȡƴ����Ӧ�ĺ���
									py_chinese_length = strlen(py_chinese);                             //ƴ����Ӧ�ĺ��ֵĸ���*2
											
									if(py_chinese_length != 0)                                          //����������鲻Ϊ�� ����к���ѡ��
									{
										if(chinese_choice_flag == 0)                                      //�Ƿ񴴽�����ѡ��ť
										{
											chinese_choice_flag = 1;
											for(j = 0;j < 10;j++)
											{
												WM_ShowWindow(btn_handle_WORD[j]);
												WM_EnableWindow(btn_handle_WORD[j]);
											}
										}
										else
										{
											for(j = 0;j < 10;j++)      //������ѡ��ť��ղ�����
											{
												BUTTON_SetText(btn_handle_WORD[j]," ");
												WM_DisableWindow(btn_handle_WORD[j]);
											}
										}								
												
												//�����ر�־λ
										py_chinese_page_flag = 0;				
										for(k = 0; k < 20 ; k++)
										{
											for(j =0; j < 16; j++)
											{
												py_chinese_display[k][j] = 0;
											}
										}
												
										//����11������ �򲻽��з�ҳ ��ʱ��ҳ��ť��ʹ���Ҳ���ʾ��ҳͼ�� ���ִӵ�һ����ť�Ͽ�ʼ��ʾ �м���������ʹ�ܼ�����ť
										if(py_chinese_length < 20)                                        
										{
											k = 0;
											//��ʾ���ֲ�ʹ����Ӧ��ť
											for(j = 0; j < (py_chinese_length-1); j = j+2)
											{
												tmp_char[0] = py_chinese[j];
												tmp_char[1] = py_chinese[j+1];
												WM_EnableWindow(btn_handle_WORD[(j/2)]);
												BUTTON_SetFont(btn_handle_WORD[(j/2)],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[(j/2)], tmp_char);
												k++;
											}
											//����ʾ���ֵİ�ť��ʹ��  ��������޷�Ӧ
											for(;k<10;k++)
											{
												WM_DisableWindow(btn_handle_WORD[k]);
											}
											py_chinese_page_flag = 0;                     //����Ҫ��ҳ
										}
										else        //����11������ ����з�ҳѡ�� 
										{
											//��ʾ��ҳ��ť �ҵ�ǰΪ��һҳ ��ֻ���·�ҳ��ť û�����Ϸ�ҳ��ť
											WM_EnableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9], ">>");
											//�����ֵַ����������з�����ʾ ���е�һҳ��ʾ9������ ����ÿҳ��ʾ8������
											for(j = 0; j < 9; j++)
											{
												tmp_char[0] = py_chinese[j*2];
												tmp_char[1] = py_chinese[j*2+1];
												WM_EnableWindow(btn_handle_WORD[j]);
												BUTTON_SetFont(btn_handle_WORD[j],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[j], tmp_char);								
											}
											
											py_chinese_page_current = 0;
											py_chinese_page_flag = 1;        //��Ҫ��ҳ
											
											for(k = 0; k < 20 ; k++)
											{
												if((18+k*16) < py_chinese_length)      //����δ����ĺ��� ����з��䵽����
												{
													py_chinese_page = k+1;										
													for(j = 0; j < 16; j++)
													{
														if((18+j+k*16) < py_chinese_length)          //����δ����ĺ��� ����з��䵽����
														{
															py_chinese_display[py_chinese_page][j] = py_chinese[18+j+k*16];
														}
														else                                         //����δ����ĺ��� �����
														{
															j = 16;
															k = 20;
														}
													}
												}
												else                                   //����δ����ĺ��� �����
												{
													k = 20;
												}
											}
										}
									}
									else      //ƴ��û�ж�Ӧ�ĺ���
									{
										if(chinese_choice_flag == 1)
										{
											for(j = 0;j < 10;j++)      //������ѡ��ť����
											{
												BUTTON_SetText(btn_handle_WORD[j]," ");
												WM_DisableWindow(btn_handle_WORD[j]);
											}
										}
										//�����ر�־λ
										py_chinese_page_flag = 0;				
										for(k = 0; k < 20 ; k++)
										{
											for(j =0; j < 18; j++)
											{
												py_chinese_display[k][j] = 0;
											}
										}	
									}
								}
						  }
	            else
							{
								if(number_length < NUM_MAX_LENGTH)
								{
									if((capital_lower_flag == 1) || (Shift_flag == 1))
									{
										number_en[number_length] = Capital_keys[(Id - ID_INFORMATION_KEY_1)];
									}
									else
									{
								    number_en[number_length] = Lower_keys[(Id - ID_INFORMATION_KEY_1)];
									}
									if(Shift_flag == 1)
									{
										Shift_flag = 0;
										for(i = 0; i < 44; i++)
										{
											hItem = WM_GetDialogItem(pMsg->hWin, (ID_INFORMATION_KEY_1 + i));
											BUTTON_SetText(hItem,Lower_button_keys[i]);
											if((ID_INFORMATION_KEY_1 + i) == ID_INFORMATION_KEY_SHIFT)
											{
												BUTTON_SetBkColor(hItem,0,TYPE_BUTTON_CLR_UNPRESSED);                                 //δ���µı���ɫ
												BUTTON_SetBkColor(hItem,1,TYPE_BUTTON_CLR_PRESSED);			                            //���µı���ɫ
											}
										}
									}
									number_length = number_length + 1;
							    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_number);
								  EDIT_SetText(hItem,number_en);
								}
								else
								{
// 									FR1000_ERROR_FLAG = 17;
// 				          Error_popup(FR1000_ERROR_FLAG);
									;
								}
							}
							break;
						}
						else if(                            //���ְ�ť�Ĵ���ʽ
							(Id > (ID_INFORMATION_KEY_1-1))
				     &(Id < (ID_INFORMATION_KEY_B)))
						{
							if(putin_flag == 0)
							{
								if(number_length < NUM_MAX_LENGTH)
								{
								  number_en[number_length] = Lower_keys[(Id - ID_INFORMATION_KEY_1)];
									number_length = number_length + 1;
							    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_number);
								  EDIT_SetText(hItem,number_en);
								}
							}
							break;
						}
						else if(                            //����ѡ��ť
							(Id > (ID_INFORMATION_WORD_1-1))
				     &(Id < (ID_INFORMATION_WORD_B)))
						{
						  if(py_chinese_page_flag == 0)                               //δ���з�ҳ ���ʱѡ���Ӧ�ĺ��ּ���
							{
								if(name_ch_length < NAME_MAX_LENGTH)
								{
									name_chinese[name_ch_length*2] = py_chinese[(Id - ID_INFORMATION_WORD_1)*2];
									name_chinese[name_ch_length*2+1] = py_chinese[(Id - ID_INFORMATION_WORD_1)*2 + 1];
									name_ch_length = name_ch_length+1;
									hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
									EDIT_SetText(hItem,name_chinese);
									
									//����ѡ����� ˢ�º���ѡ��ťΪ����ˢ�±�־λ
									for(j = 0; j < 10; j++)
									{
										BUTTON_SetText(btn_handle_WORD[j]," ");
										WM_DisableWindow(btn_handle_WORD[j]);
									}
									for(j = 0; j < 8;j++)
									{
										py_string[j] = 0;
									}
									for(j = 0; j < 255;j++)
									{
										py_chinese[j] = 0;
									}
									for(k = 0; k < 20 ; k++)
									{
										for(j =0; j < 18; j++)
										{
											py_chinese_display[k][j] = 0;
										}
									}
									py_chinese_page = 0;
									py_chinese_page_current = 0;
									py_chinese_page_flag = 0;
									BUTTON_SetText(btn_handle_WORD[10]," ");
									py_string_length = 0;
								}
								else                                                     //��Ҫ���б����� δ����
								{
									;;;
								}
							}
							else                                                        //�з�ҳ ��Ҫ�ж�Ϊ�ĸ���ť�����в�ͬ�Ĳ���
							{
								if(py_chinese_page_current == 0)                          //��ǰҳΪ��ҳ  ��ֻ�������һ����ťʱ��ҳ ���������ѡ���ּ���
								{
									if(                                                     //��ҳ ��ǰʮһ����ťֱ�ӷ��غ��ּ���
										 (Id > (ID_INFORMATION_WORD_1 - 1))
										&(Id < (ID_INFORMATION_WORD_A))
										)
									{
										if(name_ch_length < NAME_MAX_LENGTH)
										{
											name_chinese[name_ch_length*2] = py_chinese[(Id - ID_INFORMATION_WORD_1)*2];
											name_chinese[name_ch_length*2+1] = py_chinese[(Id - ID_INFORMATION_WORD_1)*2 + 1];
											name_ch_length = name_ch_length+1;
											hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
											EDIT_SetText(hItem,name_chinese);
											
											//����ѡ����� ˢ�º���ѡ��ťΪ����ˢ�±�־λ
											for(j = 0; j < 10; j++)
											{
												BUTTON_SetText(btn_handle_WORD[j]," ");
												WM_DisableWindow(btn_handle_WORD[j]);
											}
											for(j = 0; j < 8;j++)
											{
												py_string[j] = 0;
											}
											for(j = 0; j < 255;j++)
											{
												py_chinese[j] = 0;
											}
											for(k = 0; k < 20 ; k++)
											{
												for(j =0; j < 18; j++)
												{
													py_chinese_display[k][j] = 0;
												}
											}
											py_chinese_page = 0;
											py_chinese_page_current = 0;
											py_chinese_page_flag = 0;
											BUTTON_SetText(btn_handle_WORD[10]," ");
											py_string_length = 0;
										}
										else                                                     //��Ҫ���б����� δ����
										{
											;;;
										}
									}
									else                                                       //�����Ϊ��ҳ��ť ����з�ҳ��ˢ�°�ť�ϵĺ���
									{
										//��ʾ�Ϸ�ҳ��ť �·�ҳ��ť�Ƿ���ʾ��Ҫ�����ж�
										WM_EnableWindow(btn_handle_WORD[0]);
										BUTTON_SetFont(btn_handle_WORD[0],&GUI_Font24_ASCII);
										BUTTON_SetText(btn_handle_WORD[0],  "<<");
										if(py_chinese_page > 1)      // ��ҳ�����2ҳ ��������·�ҳ��ť
										{
											WM_EnableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9], ">>");
										}
										else
										{
											WM_DisableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9], " ");
										}
										//���м�8����������ʾ����
										for(i = 0;i < 8;i++)
										{
											tmp_char[0] = py_chinese_display[py_chinese_page_current+1][i*2];
											tmp_char[1] = py_chinese_display[py_chinese_page_current+1][i*2+1];
											BUTTON_SetFont(btn_handle_WORD[i+1],&GUI_FontHZ24);
											if((tmp_char[0]+tmp_char[1]) != 0)         //���ֲ�Ϊ��
											{
												WM_EnableWindow(btn_handle_WORD[i+1]);
												BUTTON_SetText(btn_handle_WORD[i+1], tmp_char);	
											}
											else                                      //���ַ� �޺��� ����ʾΪ���ҽ��ð�ť
											{
												BUTTON_SetText(btn_handle_WORD[i+1], " ");	
												WM_DisableWindow(btn_handle_WORD[i+1]);
											}
										}
										py_chinese_page_current = py_chinese_page_current+1;
									}
								}
								else if(py_chinese_page_current != py_chinese_page)                       //��ǰҳΪ�м�ҳ 
								{
									if(                                                     //�м�ҳ�м�8��ֱ�ӷ��غ��ּ���
										 (Id > (ID_INFORMATION_WORD_1))
										&(Id < (ID_INFORMATION_WORD_A))
										)
									{
										if(name_ch_length < NAME_MAX_LENGTH)
										{
											name_chinese[name_ch_length*2] = py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2];
											name_chinese[name_ch_length*2+1] = py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2 + 1];
											name_ch_length = name_ch_length+1;
											hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
											EDIT_SetText(hItem,name_chinese);
											
											//����ѡ����� ˢ�º���ѡ��ťΪ����ˢ�±�־λ
											for(j = 0; j < 10; j++)
											{
												WM_DisableWindow(btn_handle_WORD[j]);
												BUTTON_SetText(btn_handle_WORD[j]," ");
											}
											for(j = 0; j < 8;j++)
											{
												py_string[j] = 0;
											}
											for(j = 0; j < 255;j++)
											{
												py_chinese[j] = 0;
											}
											for(k = 0; k < 20 ; k++)
											{
												for(j =0; j < 18; j++)
												{
													py_chinese_display[k][j] = 0;
												}
											}
											py_chinese_page = 0;
											py_chinese_page_current = 0;
											py_chinese_page_flag = 0;
											BUTTON_SetText(btn_handle_WORD[10]," ");
											py_string_length = 0;
										}
										else                                                     //��Ҫ���б����� δ����
										{
											;;;
										}
									}
									else if(Id == ID_INFORMATION_WORD_1)                  //�����Ϊ�Ϸ�ҳ��ť ����з�ҳ��ˢ�°�ť�ϵĺ���
									{
										//��ʾ�·�ҳ��ť �Ϸ�ҳ��ť�Ƿ���ʾ��Ҫ�����ж�
										WM_EnableWindow(btn_handle_WORD[9]);
										BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
										BUTTON_SetText(btn_handle_WORD[9],  ">>");
										if(py_chinese_page_current == 1)      //��ǰҳ���ǵڶ�ҳ ����ʾ�Ϸ�ҳ��ť ����ʾ10������
										{
											for(i = 0;i < 9;i++)
											{
												tmp_char[0] = py_chinese[i*2];
												tmp_char[1] = py_chinese[i*2+1];
												WM_EnableWindow(btn_handle_WORD[i]);
												BUTTON_SetFont(btn_handle_WORD[i],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[i], tmp_char);	
											}
										}
										else
										{
											//��ʾ�Ϸ�ҳ��ť
											WM_EnableWindow(btn_handle_WORD[0]);
											BUTTON_SetFont(btn_handle_WORD[0],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[0],  "<<");
											//���м�8����������ʾ����
											for(i = 0;i < 8;i++)
											{
												tmp_char[0] = py_chinese_display[py_chinese_page_current-1][i*2];
												tmp_char[1] = py_chinese_display[py_chinese_page_current-1][i*2+1];
												WM_EnableWindow(btn_handle_WORD[i+1]);
												BUTTON_SetFont(btn_handle_WORD[i+1],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[i+1], tmp_char);	
											}
										}							
										py_chinese_page_current = py_chinese_page_current - 1;
									}
									else                                     //�����Ϊ�·�ҳ��ť ����з�ҳ��ˢ�°�ť�ϵĺ���
									{
										//��ʾ�Ϸ�ҳ��ť �·�ҳ��ť�Ƿ���ʾ��Ҫ�����ж�
										WM_EnableWindow(btn_handle_WORD[0]);
										BUTTON_SetFont(btn_handle_WORD[0],&GUI_Font24_ASCII);
										BUTTON_SetText(btn_handle_WORD[0],  "<<");
										if(py_chinese_page_current != (py_chinese_page-1))      //��ǰҳ�治�ǵ����ڶ�ҳ ��ʾ�·�ҳ��ť 
										{
											WM_EnableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9],  ">>");
										}
										else
										{
											WM_DisableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9],  " ");
										}
										//���м�9����������ʾ����
										for(i = 0;i < 8;i++)
										{
											tmp_char[0] = py_chinese_display[py_chinese_page_current+1][i*2];
											tmp_char[1] = py_chinese_display[py_chinese_page_current+1][i*2+1];
											BUTTON_SetFont(btn_handle_WORD[i+1],&GUI_FontHZ24);
											if((tmp_char[0] + tmp_char[1]) != 0)
											{
												WM_EnableWindow(btn_handle_WORD[i+1]);
												BUTTON_SetText(btn_handle_WORD[i+1], tmp_char);	
											}
											else
											{
												WM_DisableWindow(btn_handle_WORD[i+1]);
												BUTTON_SetText(btn_handle_WORD[i+1], " ");	
											}
										}													
										py_chinese_page_current = py_chinese_page_current + 1;
									}
								}
								else                                                            //��ǰҳ��Ϊ���һҳ 
								{
									if(                                                     //���һҳ ���ж��Ƿ��к��� ���򷵻غ��� ����ղ���
										 (Id > (ID_INFORMATION_WORD_1))
										&(Id < (ID_INFORMATION_WORD_B))
										)
									{
										if(name_ch_length < NAME_MAX_LENGTH)
										{
											if(
												 (((py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2])
												+(py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2 + 1])) != 0)
												)
											{
												name_chinese[name_ch_length*2] = py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2];
												name_chinese[name_ch_length*2+1] = py_chinese_display[py_chinese_page_current][(Id - ID_INFORMATION_WORD_2)*2 + 1];
												name_ch_length = name_ch_length+1;
												hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION_name);
												EDIT_SetText(hItem,name_chinese);
												
												//����ѡ����� ˢ�º���ѡ��ťΪ����ˢ�±�־λ
												for(j = 0; j < 10; j++)
												{
													WM_DisableWindow(btn_handle_WORD[j]);
													BUTTON_SetText(btn_handle_WORD[j]," ");
												}
												for(j = 0; j < 8;j++)
												{
													py_string[j] = 0;
												}
												for(j = 0; j < 255;j++)
												{
													py_chinese[j] = 0;
												}
												for(k = 0; k < 20 ; k++)
												{
													for(j =0; j < 18; j++)
													{
														py_chinese_display[k][j] = 0;
													}
												}
												py_chinese_page = 0;
												py_chinese_page_current = 0;
												py_chinese_page_flag = 0;
												BUTTON_SetText(btn_handle_WORD[10]," ");
												py_string_length = 0;
											}
										}
										else                                                     //��Ҫ���б����� δ����
										{
											;;;
										}
									}
									else if(Id == ID_INFORMATION_WORD_1)                  //�����Ϊ�Ϸ�ҳ��ť ����з�ҳ��ˢ�°�ť�ϵĺ���
									{
										if(py_chinese_page == 1)                                   //���ֻ����ҳ ����ʾ�·���ť ˢ���ַ�
										{
											WM_EnableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9],  ">>");
											for(i = 0;i < 9;i++)
											{
												tmp_char[0] = py_chinese[i*2];
												tmp_char[1] = py_chinese[i*2+1];
												WM_EnableWindow(btn_handle_WORD[i]);
												BUTTON_SetFont(btn_handle_WORD[i],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[i], tmp_char);	
											}							
										}
										else                                                          //�����ҳ�������ҳ ����ʾ���·���ť ˢ���ַ�
										{
											WM_EnableWindow(btn_handle_WORD[0]);
											BUTTON_SetFont(btn_handle_WORD[0],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[0],  "<<");
											WM_EnableWindow(btn_handle_WORD[9]);
											BUTTON_SetFont(btn_handle_WORD[9],&GUI_Font24_ASCII);
											BUTTON_SetText(btn_handle_WORD[9],  ">>");
											for(i = 0;i < 8;i++)
											{
												tmp_char[0] = py_chinese_display[py_chinese_page_current-1][i*2];
												tmp_char[1] = py_chinese_display[py_chinese_page_current-1][i*2+1];
												WM_EnableWindow(btn_handle_WORD[i+1]);
												BUTTON_SetFont(btn_handle_WORD[i+1],&GUI_FontHZ24);
												BUTTON_SetText(btn_handle_WORD[i+1], tmp_char);	
											}	
										}							
									}
									py_chinese_page_current = py_chinese_page_current - 1;
								}
				      }
							break;
						}	
						
					}
			 }
		 
			 break;
	  }
    break;
  default:
    WM_DefaultProc(pMsg);
// 	  flag++;
// 	  if(flag==2)
// 		{
//      
//          Parameter_Inital_flag=1;
// 			   flag=0;
//     }

    break;
  }
 
			
	
}

extern unsigned char test_putin_flag;
extern unsigned char name_chinese_test[40];
extern unsigned char name_chinese[40];
extern unsigned char number_test[30];
extern unsigned char number_en[30];

WM_HWIN CreateFramewin_INFORMATION(void) 
{
  WM_HWIN hWin;
  hWin = GUI_ExecDialogBox(_aDialogCreate_INFORMATION, GUI_COUNTOF(_aDialogCreate_INFORMATION), _cbDialog_INFORMATION, WM_HBKWIN, 0, 0);

  return hWin;
}