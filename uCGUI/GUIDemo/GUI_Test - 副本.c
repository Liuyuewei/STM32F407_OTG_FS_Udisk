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
#define Display_time   300          //提示信息刷新间隔时间  单位为ms

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
char input_string[8]= "de";   //最多6个字母


unsigned char test_choice = 0;
unsigned char start_flag = 0; 
//start_flag = 0:正常状态         
//start_flag = 1:FPGA发送过来的数据出错   
//start_flag = 2:等待FPGA发来试剂卡插入状态超时
//start_flag = 3:等待FPGA发来SPOT CODE扫描完成超时
//start_flag = 4:等待FPGA发来采血管插入成功状态超时
//start_flag = 5:等待FPGA发来血液吸入加热槽完成状态超时 
//start_flag = 6:等待FPGA发来血液压入测量槽完成状态超时 
//start_flag = 7:等待FPGA发来测量完成状态超时
//start_flag = 7:测量过程中点击了结束测量按钮

uint8_t FPGA_IO_data = 0;                               //接受FPGA发送过来的数据
uint8_t FPGA_IO_flag = 0;                               //置1则表示FPGA端有数据传输过来 
uint16_t measure_data_CH1[1000] = {0};
uint16_t measure_data_CH2[1000] = {0};
uint16_t measure_data_CH3[1000] = {0};
uint16_t measure_data_CH4[1000] = {0};
uint16_t SPI1_data_spi[4000] = {0};
uint16_t SPOT_CODE_Data[2] = {0};                       //存储SPOT CODE数据 
uint32_t Test_time = 0;                                 //超时时间 

/*点击结束按钮后需要用到的一些变量*/
uint8_t END_Measure = 0;                                //0：点击按钮后不结束测量 1：点击按钮后结束测量 

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
*       _aDialogCreate_warning       错误提示窗口 由一个按钮结束该窗口 
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate_warning[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 300, 210, 200, 120},
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 10, 20, 180, 20},
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 50, 50, 80, 40},
};

/*********************************************************************
*
*       _cbDialog_warning 错误提示窗口回调函数
*/
static void _cbDialog_warning(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;
	hItem = pMsg->hWin;
	
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                          //对错误提示窗口的初始化 包括文本信息等
		
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
	  case WM_NOTIFY_PARENT:                        //对按键信息进行响应
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
	  switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //在按键松开时进行响应
				switch(Id) 
				{
					case ID_BUTTON_0: 
					{
						GUI_EndDialog(hItem,0);               //结束该错误提示窗口
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
*       CreateFramewin    建立错误提示窗口 由回调函数结束该进程
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
*       _aDialogCreate_end       错误提示窗口 由一个按钮结束该窗口 
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate_end[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_END, 300, 210, 200, 120},
//   { TEXT_CreateIndirect, "Text", ID_TEXT_END, 10, 20, 180, 40},
//   { BUTTON_CreateIndirect, "Button", ID_BUTTON_END_YES, 30, 70, 80, 40},
//   { BUTTON_CreateIndirect, "Button", ID_BUTTON_END_NO, 120, 70, 80, 40},
  { TEXT_CreateIndirect, "Text", ID_TEXT_END, 10, 20, 180, 20},
  { BUTTON_CreateIndirect, "德", ID_BUTTON_END_YES, 5, 45, 90, 50},
  { BUTTON_CreateIndirect, "德", ID_BUTTON_END_NO, 105, 45, 90, 50},
};

/*********************************************************************
*
*       _cbDialog_end 错误提示窗口回调函数
*/
static void _cbDialog_end(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;
	hItem = pMsg->hWin;
	
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                          //对错误提示窗口的初始化 包括文本信息等
		
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
	  case WM_NOTIFY_PARENT:                        //对按键信息进行响应
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
	  switch(NCode) 
	  {
			case WM_NOTIFICATION_RELEASED:              //在按键松开时进行响应
				switch(Id) 
				{
					case ID_BUTTON_END_YES: 
					{
						END_Measure = 1;
						GUI_EndDialog(hItem,0);               //结束该错误提示窗口
						break;
					}
					case ID_BUTTON_END_NO: 
					{
						END_Measure = 0;
						GUI_EndDialog(hItem,0);               //结束该错误提示窗口
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
*   CreateFramewin_End 点击结束测量按钮后的处理程序  由回调函数结束该进程
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
    TEXT_SetText(hItem, "患者姓名");
//     TEXT_SetText(hItem, "text");
	
	  //EDIT_SetDefaultFont(&GUI_FontHZ24);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION);
 	  EDIT_SetFont(hItem,&GUI_FontHZ24);
    //EDIT_SetText(hItem, "郭德纲");
     //EDIT_SetText(hItem, "Edit");
	  
 	  BUTTON_SetDefaultFont(&GUI_FontHZ24);
	  //GUI_UC_SetEncodeUTF8();
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_WORD_1);
// 	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
    BUTTON_SetText(hItem, "王");
//     BUTTON_SetText(hItem, "Q");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_INFORMATION_WORD_2);
// 	  BUTTON_SetFont(hItem,&GUI_FontHZ24);
    BUTTON_SetText(hItem, "汪");
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
			case WM_NOTIFICATION_RELEASED:              //在按键松开时进行响应
				switch(Id) 
				{
					case ID_INFORMATION_WORD_1: 
					{
						hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_INFORMATION);
         	  EDIT_SetFont(hItem,&GUI_FontHZ24);
            EDIT_SetText(hItem, "郭德纲");               
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





/*测量流程设计*/
WM_HWIN Create_Test(void) {
  
	BUTTON_Handle btn_Test_hl,btn_Set_hl,btn_Control_hl,btn_History_hl;
	static unsigned char btn_Xsize = 200, btn_Ysize = 60;
	static int i = 0;
	int key_num = 0;
	
	//显示主界面
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
		
		if (TOUCH_InRect(ts_event.x1, ts_event.y1, 5, 420, 60, 200) && (g_ucTouch == 1)) //按test按键 无需响应
		{
			test_choice = 0;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 200, 420, 60, 200) && (g_ucTouch == 1))//按setting按键 
		{
			test_choice = 1;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 395, 420, 60, 200) && (g_ucTouch == 1))//按control按键
		{
			test_choice = 2;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 590, 420, 60, 200) && (g_ucTouch == 1))//按history按键 
		{
			test_choice = 3;
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))   //点击开始测试按钮
		{
			TEST_begin();                             //调用测试函数
			bmp_dispflash(0xBB810,0,0);	              //此次测试结束 重新显示测试界面 
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,50, 100, 115) && (g_ucTouch == 1))   //点击病人信息按钮
		{
			TEST_type_information();
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
		}
		
		
		if(test_choice != 0)                       //点开了其它三个按键 需要进行相应处理 如 设置 质控 历史记录查询等
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
	FONT_T tFont;			/* 定义一个字体结构体变量，用于设置字体参数 */
	static unsigned long time_out_old = 0; //数据刷新时使用
	unsigned char Prompt_flag = 0;         //0：此时操作提示处显示为空 1：此时操作提示处显示为相应操作
	unsigned int i = 0,data_receive = 0,j,k,l,m,data_in;  //接收数据时使用的一些中间变量
	unsigned char ADC_num = 0;
	char buffer[64] ={0};
	char clear[64] = "         ";
	unsigned char spot_code_flag = 0;                     //0：spot code传输未完成 1：spot code数据传输完成
  unsigned char spot_code_h = 0;                        //0：spot code高位传输未完成  1：spot code高位数据传输已完成
	unsigned char spot_code_l = 0;                        //0：spot code低位传输未完成  1：spot code低位数据传输已完成
  PROGBAR_Handle progbar_test;                          //进度条显示的句柄
  unsigned char spot_code_str[16] = {0};                //用于spot code显示的测试数组
  unsigned char progbar_data = 0;                       //进度条数据
  unsigned char close_window_time = 0;     //提示10次关闭黑盒盖子
  
	// 	 设置字体参数
	{
		tFont.usFontCode = FC_ST_16;	/* 字体代码 16点阵 */ // 汉字大小为16*16 字母大小为 宽8高16
		tFont.usTextColor = CL_BLACK;	/* 字体颜色 */
		tFont.usBackColor = CL_WHITE;	/* 文字背景颜色 */
		tFont.usSpace = 0;				/* 文字间距，单位 = 像素 */
	}
	
	/*复位FPGA状态*/
	PG6_IO_reset();
	delay_nms(30);
	printf("\r\nFPGA RESET!\r\n");
	PG6_IO_set();
  
	/*对变量进行初始化*/
	start_flag = 0;
	FPGA_IO_flag = 0;
	FPGA_IO_data = 0;
	
	/*建立进度条控件 显示测试进度*/
  progbar_test = PROGBAR_Create(40,200, 500, 20, WM_CF_SHOW);  
	PROGBAR_EnableMemdev(progbar_test);
  //PROGBAR_SetFont(progbar_test, &tFont);
  PROGBAR_SetBarColor(progbar_test, 0, GUI_GREEN);       //已刷新的进度条颜色
  PROGBAR_SetBarColor(progbar_test, 1, GUI_LIGHTBLUE);       //未刷新的进度条颜色
  PROGBAR_SetTextAlign(progbar_test, GUI_TA_CENTER);     //字体位于进度条中心
	PROGBAR_SetMinMax(progbar_test,0,100);                 //进度条的最大值和最小值
	PROGBAR_SetTextColor(progbar_test,0,GUI_BLACK);        //设置字体颜色
	PROGBAR_SetTextColor(progbar_test,1,GUI_BLACK);        //设置字体颜色
	//PROGBAR_SetValue(progbar_test, 0);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
	//GUI_Exec();
	
	//CreateFramewin_warning();
	
	//RA8875_DispStr(50, 0, "Insert Card!            ", &tFont);	
	
	//发送开始测量信号
	send_command_FPGA(INSERT_CARD_COMMAND);
	
// 	Test_time = TEST_timeout;
// 	time_out_old = TEST_timeout;
	while(FPGA_IO_flag == 0)                       //等待FPGA发来试剂卡插入状态数据
	{
// 		if(Test_time == 0)                           //等待超时 提示错误信息并返回
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
    /*闪烁插卡标志 提示用户插入试剂卡*/
		RA8875_FillRect(100,170,100,60,0xF79E);
		OSTimeDlyHMSM(0, 0, 0, Display_time);
		bmp_dispflash(0xBB810,0,0);	
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
		OSTimeDlyHMSM(0, 0, 0, Display_time);
		/*扫描是否点击了结束测量按钮 如果点击了结束按钮且再次确认结束测量 则结束该测量进程*/
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
	
	//刷屏 防止界面不对
	bmp_dispflash(0xBB810,0,0);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	if(start_flag == 0)
	{
		if(FPGA_IO_flag == 1)                           //接收到了FPGA传来的数据
		{
			if(FPGA_IO_data == CARD_FINISHED_STATUES)     //接收到试剂卡插入成功状态数据
			{
				//RA8875_DispStr(50, 0, "SACN SPOT CODE!                 ", &tFont);
				//PROGBAR_SetValue(progbar_test, 10);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
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
    while(FPGA_IO_flag == 0)                       //等待FPGA发来SPOT CODE扫描状态数据
	  {
		  if(Test_time == 0)                           //等待超时 提示错误信息并返回
		  {
			  start_flag = 3;
			  break;
		  }
	  }
		
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == SPOT_FINISHED_STATUES)     //接收到SPOT CODE扫描完成状态 开始读取SPOT CODE数据
			{
				while(spot_code_flag == 0)
				{
					delay_nus(500);
					data_receive = SPI1_Receive();                           //接收SPOT CODE数据
					if(data_receive != 0xFFFF)
					{
						if(((data_receive&0x5400) == 0x5400))                                         //接收到高位数据
						{
							spot_code_h = 1;
							SPOT_CODE_Data[1] = data_receive;
						}
						else if(((data_receive&0x5000) == 0x5000))                                    //接收到低位数据
						{
							spot_code_l = 1;
							SPOT_CODE_Data[0] = data_receive;
						}
						if ((spot_code_h == 1)&&(spot_code_l == 1))      //高低位数据均已接收
						{
							spot_code_flag = 1;
							//PROGBAR_SetValue(progbar_test, 20);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
	            //GUI_Exec();
						}
					}
				}
				if(1)                                        //对SOPT CODE数据进行判断 暂无 后续添加
				{
					;;
				}
				/*将SPOT CODE数据显示在屏幕上*/
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
				
				
        //PROGBAR_SetValue(progbar_test, 30);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
        //GUI_Exec();
				
				//RA8875_DispStr(50, 0, "INSERT THE TUBE!            ", &tFont);
				FPGA_IO_flag = 0;
				send_command_FPGA(INSERT_TUBE_COMMAND);      //发送插采血管的指令
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
    while(FPGA_IO_flag == 0)                       //等待FPGA发来采血管插入状态
	  {
// 		  if(Test_time == 0)                           //等待超时 提示错误信息并返回
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
	    /*闪烁采血管标志 提示用户插入采血管*/
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
		//刷屏 防止界面不对
		bmp_dispflash(0xBB810,0,0);	
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
		
		if(FPGA_IO_flag == 1)
		{
			if(FPGA_IO_data == TUBE_FINISHED_STATUES)     //接收到采血管插入正常指令 发送抽血到加热槽指令
			{
				//RA8875_DispStr(50, 0, "SUCK THE BLOOD!                 ", &tFont);
				FPGA_IO_flag = 0;
				//PROGBAR_SetValue(progbar_test, 40);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
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
				
				
				send_command_FPGA(SUCK_BLOOD_COMMAND);      //发送抽血到加热槽的指令
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
    PROGBAR_SetValue(progbar_test, 0);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
    GUI_Exec();		
		Test_time = TEST_timeout;
    while(FPGA_IO_flag == 0)                       //等待FPGA发来吸血到加热槽的状态
	  {
		  if(Test_time == 0)                           //等待超时 提示错误信息并返回
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
			if(FPGA_IO_data == SUCK_FINISHED_STATUES)     //吸血到加热槽完成
			{
				Heat_enable();                              //开始加热血液
				delay_nms(50);	
				Heat_disable();
				//RA8875_DispStr(50, 0, "PRESS THE BLOOD!                 ", &tFont);
				FPGA_IO_flag = 0;
				PROGBAR_SetValue(progbar_test, 10);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
        GUI_Exec();
				send_command_FPGA(HEAT_FINISHED_COMMAND);      //加热血液完成 可以将血液压入测量槽
				
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
    while(FPGA_IO_flag == 0)                       //等待FPGA发来血液进入测量槽成功状态
	  {
		  if(Test_time == 0)                           //等待超时 提示错误信息并返回
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
			if(FPGA_IO_data == PRESS_FINISHED_STATUES)     //压血到测量槽成功
			{
				//send_command_FPGA(START_MEASURE_COMMAND);      //开始测量
				//RA8875_DispStr(50, 0, "START MEASURE!                 ", &tFont);
				PROGBAR_SetValue(progbar_test, 30);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
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
    while(FPGA_IO_flag == 0)                       //等待FPGA发来读数完成指令
	  {
		  if(Test_time == 0)                           //等待超时 提示错误信息并返回
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
			if(FPGA_IO_data == MEASURE_FINISHED_STATUES)     //FPGA读数完成
			{
				//RA8875_DispStr(50, 0, "FPGA FINISHED!                 ", &tFont);
				FPGA_IO_flag = 0;
				PROGBAR_SetValue(progbar_test, 50);                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
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
				
		//接受4096个FPGA发送来的数据并分别存储在4个数组中 通过数据的高4位判断数据位置
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
				PROGBAR_SetValue(progbar_test, (50+(i/81)));                     //设置进度值 显示为百分比 由于范围为(0，100)，故显示为此百分比
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
  RA8875_FillRect(35,85,290,640,0xF79E);    //刷屏 覆盖操作提示框 显示录入信息对话框
	
// 	sprintf(chinese_string,"%s",py_ime(input_string));
// //   sprintf(chinese_string,"%s",py_ime(input_string));
// //   chinese_string[2] = '\0';
	btn_handle_0 = BUTTON_Create(100,100,50,50,0xEF,WM_CF_SHOW);
  BUTTON_SetFont(btn_handle_0,&GUI_FontHZ24);
  BUTTON_SetText(btn_handle_0, "王");
	
	edit_handle_0 = EDIT_Create(200,300,100,30,0xFF,100,WM_CF_SHOW);
	EDIT_SetFont(edit_handle_0,&GUI_FontHZ24);
  EDIT_SetText(edit_handle_0, "郭德纲");
	
	GUI_Exec();
//  	CreateFramewin_INFORMATION();
}


// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/