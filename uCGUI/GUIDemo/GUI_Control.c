#include "DIALOG.h"
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
#include "Uart_communication.h"
#include "fsmc_nand.h"
#include "AT24C512.h"
#include "SimSun_16.h"
const unsigned char Control_type_card[20] = "质控卡";
const unsigned char Control_type_liquid[20] = "质控液";
const unsigned char Control_result_1[20] = "通过";
const unsigned char Control_result_0[20] = "未通过";

const unsigned char Control_en_type_card[20] = "EQC";
const unsigned char Control_en_type_liquid[20] = "WQC";
const unsigned char Control_en_result_1[20] = "PASS";
const unsigned char Control_en_result_0[20] = "FAIL";

unsigned char Control_inital_flag = 0;
unsigned char Control_type[20] = {0};
unsigned char Control_date[20] = {0};
unsigned char Control_result[20] = {0};

void Control_begin(void);
void Fresh_Control_dispaly(unsigned char inter);
void Control_Progbar_set(unsigned char Progbar_data);
void Control_his_deal(bool Create_or_Search);


#define  PROGBAR_CONTROL_CLR_BKG        GUI_LIGHTBLUE
#define  PROGBAR_CONTROL_CLR            GUI_GREEN
#define  PROGBAR_CONTROL_X              40
#define  PROGBAR_CONTROL_Y              160
#define  PROGBAR_CONTROL_SIZEX          500
#define  PROGBAR_CONTROL_SIZEY          20
#define  PROGBAR_CONTROL_TOTAL          100
PROGBAR_Handle PROGBAR_CONTROL;                          //进度条显示的句柄
unsigned char control_date_buf[24];
unsigned char Control_finish_flag = 0;
bool Control_back_flag=0;
extern bool power_flag;
extern bool time_flag;
extern void  time_display(bool *time_flag);
void Create_Control(void) 
{
	unsigned char i = 0;
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
	
	if(Control_finish_flag == 0)
	{
		for(i = 0;i<24;i++)
		{
			control_date_buf[i] = 0;
		}
  }
	//Control_finish_flag = 0;
	
	
	do
	{
		Prompt_column_update();
		time_display(&time_flag);
		if(Control_inital_flag == 0)
		{
			if(Chinese_display == 1)
			{
				bmp_dispflash(ADRESS_CONTROL_RIGHT1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);		
			}
			else
			{
				bmp_dispflash(ADRESS_EN_CONTROL_RIGHT1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}	
			
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			if(Chinese_display == 1)
			{
				bmp_dispflash(ADRESS_CONTROL_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
				SERIAL_ROM_Font_mode();
				Graphic_Mode();			
				GUI_SetFont(&GUI_FontHZ16);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("质控类型:",42,335);
				GUI_DispStringAt("测试结果:",242,335);
				GUI_DispStringAt("质控时间:",42,375);	
			}
			else
			{
				bmp_dispflash(ADRESS_EN_CONTROL_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	SERIAL_ROM_Font_mode();
				Graphic_Mode();			
				GUI_SetFont(&GUI_Font16B_ASCII);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("Control Type:",42,335);
				GUI_DispStringAt("Control Result:",242,335);
				GUI_DispStringAt("Control Time:",42,375);	
			}	
			
			
			if(Control_finish_flag == 1)
			{
				if(Chinese_display == 1)
				{
					memcpy(Control_type,Control_type_card,sizeof(Control_type_card));
					memcpy(Control_result,Control_result_1,sizeof(Control_result_1));
					GUI_SetFont(&GUI_FontHZ16);		   
					GUI_SetBkColor(BACK_COLOUR_24);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringAt(Control_type,120,335);     //类型
					GUI_DispStringAt(Control_result,320,335);   //结果
					
					GUI_SetFont(&GUI_Font16_ASCII);		   //时间
					GUI_SetBkColor(BACK_COLOUR_24);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringAt(control_date_buf,120,375);
				}
				else
				{
					memcpy(Control_type,Control_en_type_card,sizeof(Control_type_card));
					memcpy(Control_result,Control_en_result_1,sizeof(Control_result_1));
					GUI_SetFont(&GUI_Font16B_ASCII);		   
					GUI_SetBkColor(BACK_COLOUR_24);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringAt(Control_type,130,335);     //类型
					GUI_DispStringAt(Control_result,320,335);   //结果
					GUI_DispStringAt(control_date_buf,130,375); //时间
				}
			}
			Control_inital_flag = 1;
		}
		
    if (TOUCH_InRect(ts_event.x1, ts_event.y1, 5, 420, 60, 200) && (g_ucTouch == 1))
		{
			test_choice = 0;
			Control_inital_flag = 0;
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 200, 420, 60, 200) && (g_ucTouch == 1))
		{
			test_choice = 1;
			Control_inital_flag = 0;
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 395, 420, 60, 200) && (g_ucTouch == 1))
		{
			test_choice = 2;                        //调用测试函数
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 590, 420, 60, 200) && (g_ucTouch == 1))
		{
			test_choice = 3;
			Control_inital_flag = 0;
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))   //点击开始测试按钮
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			Control_finish_flag = 0;
			//获取系统时间
			RTC_ClearFlag(RTC_FLAG_ALRAF);	
			RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
			(void)RTC->DR;
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
			
			sprintf(control_date_buf," 20%0.2d/%0.2d/%0.2d  %0.2d:%0.2d", 
													RTC_DateStructure.RTC_Year,
																RTC_DateStructure.RTC_Month, 
																			RTC_DateStructure.RTC_Date,
																							RTC_TimeStructure.RTC_Hours, 
																										RTC_TimeStructure.RTC_Minutes);
			
			if(Chinese_display == 1)
			{
			  GUI_SetFont(&GUI_Font16_ASCII);		   //时间
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt(control_date_buf,120,375);
			}
			else
			{
				GUI_SetFont(&GUI_Font16B_ASCII);		   //时间
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt(control_date_buf,130,375);
			}
			
			if(Chinese_display == 1)
			{
				bmp_dispflash(ADRESS_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);		
			}
			else
			{
				bmp_dispflash(ADRESS_EN_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}	
			
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			Work_type = 1;
			Control_begin();
			Work_type = 2;
      Control_inital_flag = 0;			
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,50, 100, 115) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))   //点击病人信息按钮
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
	
			
			Control_his_deal(0);
			
			Control_inital_flag = 0;
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,300, 100, 115) && (g_ucTouch == 1))   //点击打印按钮
		{
			if(Control_finish_flag == 1)
			{
				Print_inital();
				Print_test();
			  delay_nms(60);
				
				if((rcv_print_count != 0) && (Print_rcv_buf[0] == PRINT_ERROR))
				{
				    FR1000_ERROR_FLAG = 15;
				    Error_popup(FR1000_ERROR_FLAG);
				    Control_inital_flag = 0;
				}
				else if((rcv_print_count != 0) && (Print_rcv_buf[0] == PRINT_PAPER_LACK))
				{
				    FR1000_ERROR_FLAG = 16;
				    Error_popup(FR1000_ERROR_FLAG);
				    Control_inital_flag = 0;
				}
				else
				{
					usart6_write_string("          质控报告单        \r\n");
					delay_nms(60);
					usart6_write_string("==============================\r\n");
					delay_nms(60);
					usart6_write_string("质控类型:  ");
					delay_nms(60);
					usart6_write_string(Control_type);
					delay_nms(60);
					usart6_write_string("\r\n");
					delay_nms(60);
					usart6_write_string("质控结果:  ");
					delay_nms(60);
					usart6_write_string(Control_result);
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
					usart6_write_string("  \r\n");
					delay_nms(60);
				}
				rcv_print_count = 0;
				memset(Print_rcv_buf,0,sizeof(Print_rcv_buf));
			}
			else
			{
				FR1000_ERROR_FLAG = 14;
				Error_popup(14);
				Control_inital_flag = 0;
			}
		}
						//点击了关机按钮
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 0, 0, 40, 115) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			power_flag=Warning_popup(CODE_POWER_DOWN);
		
			if(power_flag== 1)
			{
				RA8875_ClrScr(0x0510);
				POWER_Down_deal();
				Power_disable();
				while(1);
			}
			else
			{
				Control_inital_flag=0;	
			}
 		}
		if(test_choice != 2)
		{
			break;
		}
		//OSTimeDlyHMSM(0, 0, 0, 100);
	}
	while(1);
}
// #define S_START        0x05
// #define S_SPOT_CODE    0x06
// #define S_CHECK_BLOOD  0x07
// #define S_ABSORB       0x08
// #define S_HEAT         0x09
// #define S_PRESS        0x0A
// #define S_SAMPLE       0x0B
// #define S_STOP         0x0C
// #define 

#define CONTROL_timeout   1000
unsigned char Control_state = S_START;
unsigned char Control_twinkle_flag = 0;
unsigned char Control_error_time = 0;
unsigned char Control_end = 0;
//Control_end = 0:正常状态         
//Control_end = 1:FPGA发送过来的数据出错   
//Control_end = 2:等待FPGA发来试剂卡插入状态超时
//Control_end = 3:等待FPGA发来SPOT CODE扫描完成超时
//Control_end = 4:等待FPGA发来采血管插入成功状态超时
//Control_end = 5:等待FPGA发来血液吸入加热槽完成状态超时 
//Control_end = 6:等待FPGA发来血液压入测量槽完成状态超时 
//Control_end = 7:等待FPGA发来测量完成状态超时
//Control_end = 8:测量过程中点击了结束测量按钮
//Control_end = 9:SPOT CODE未扫描 请扫描SPOT CODE
//Control_end = 10:质控过程中拔出试剂卡
//Control_end = 11:质控过程中拔出采血管

unsigned char close_window_time = 0;
unsigned char control_spot_code_h = 0,
              control_spot_code_m = 0,
              control_spot_code_l = 0;
unsigned long int Control_time = 0;

uint32_t Control_pro_time = 0;

void Control_begin(void)
{
	Control_state = S_START;
	while(1)
	{
	  Prompt_column_update();
		time_display(&time_flag);
		switch(Control_state)                        //发送控制指令
		{
			case S_START:
			{
				Control_state = S_CHECK_BLOOD;
				send_mcu_cmd = S_START;
				mcu_send_cmd();
				break;
			}
			case S_SPOT_CODE:
			{
				Control_state = S_CHECK_BLOOD;
				send_mcu_cmd = S_SPOT_CODE;
				mcu_send_cmd();
				break;
			}
			case S_CHECK_BLOOD:
			{
				Control_state = S_ABSORB;
				send_mcu_cmd = S_CHECK_BLOOD;
				mcu_send_cmd();
				break;
			}
			case S_ABSORB:
			{
				Control_state = S_PRESS;
				send_mcu_cmd = S_ABSORB;
				mcu_send_cmd();
				break;
			}
			case S_PRESS:
			{
				Control_state = S_SAMPLE;
				send_mcu_cmd = S_PRESS;
				mcu_send_cmd();
				break;
			}
			case S_SAMPLE:
			{
				Control_state = S_START;
				send_mcu_cmd = S_SAMPLE;
				mcu_send_cmd();
				Control_finish_flag = 0;
				Control_pro_time = 0;
				break;
			}
			case S_STOP:
			{
				Control_state = S_START;
				send_mcu_cmd = S_STOP;
				mcu_send_cmd();
				Control_finish_flag = 0;
				break;
			}
		}
		
		Control_twinkle_flag = 0;
		Control_error_time = 0;                      //错误计数清零
		Control_end = 0;                             //错误标志位
		rec_mcu_flag = 4;                            //=2则表示收到正确的回复
		Control_time = CONTROL_timeout;              //超时时间置位
		while(1)
		{
			Prompt_column_update();
			time_display(&time_flag);
			mcu_uart_Process();
			if(rec_mcu_flag != 2)
			{
				switch(send_mcu_cmd)
				{
					case S_START:                                           //闪烁插卡标识
					{
						if(Control_twinkle_flag == 0)
						{
							//闪烁插卡标志 提示用户插入试剂卡
							RA8875_FillRect(100,170,110,100,BACK_COLOUR_16);
							Control_twinkle_flag = 1;
						}
						else
						{
							if(Chinese_display == 1)
							{
								bmp_dispflash(ADRESS_CAED,106,186,LENGTH_CARD_X,LENGTH_CARD_Y);	
							}
							else
							{
								bmp_dispflash(ADRESS_CAED,107,189,LENGTH_CARD_X,LENGTH_CARD_Y);	
							}
							
							SERIAL_ROM_Font_mode();
							Graphic_Mode();
							Control_twinkle_flag = 0;
						}	
            if(Control_time == 0)
						{
							Control_end = 2;                                       //等待从机发来插卡完成超时
						}							
						break;
					}
					case S_SPOT_CODE:
					{
            if(Control_time == 0)
						{
							Control_end = 3;                                       //等待从机扫描SPOT CODE超时
						}			
						break;
					}
					case S_CHECK_BLOOD:                                  //闪烁采血管标识
					{
						if(Control_twinkle_flag == 0)                       
						{
							RA8875_FillRect(300,170,110,40,BACK_COLOUR_16);
							Control_twinkle_flag = 1;
						}
						else
						{
							if(Chinese_display == 1)
							{
								bmp_dispflash(ADRESS_BLOOD,310,187,LENGTH_BLOOD_X,LENGTH_BLOOD_Y);	
							}
							else
							{
								bmp_dispflash(ADRESS_BLOOD,317,189,LENGTH_BLOOD_X,LENGTH_BLOOD_Y);	
							}
							SERIAL_ROM_Font_mode();
							Graphic_Mode();
							Control_twinkle_flag = 0;
						}
            if(Control_time == 0)
						{
							Control_end = 4;                                            //等待从机插入采血管超时
						}			
						break;
					}
					case S_ABSORB:
					{					
            if(Control_time == 0)
						{
							Control_end = 5;                                           //等待吸血完成超时
						}							
						break;
					}
					case S_PRESS:
					{	
            if(Control_time == 0)
						{
							Control_end = 6;                                           //等待压血完成超时
						}								
						break;
					}
					case S_SAMPLE:
					{		
            if(Control_time == 0)
						{
							Control_end = 7;                                           //等待采样完成超时
						}
						
						if(Control_pro_time < 400)
						{
							Control_pro_time++;
							Control_Progbar_set(20 + Control_pro_time/5);
						}	
						
						break;
					}
					case S_STOP:
					{						
						break;
					}
				}
				//扫描是否点击了结束测量按钮并进行相关处理
				if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
				{
					ts_event.x1 = -1;
					ts_event.y1 = -1;
					g_ucTouch = 0;
					_StoreUnstable(-1, -1);
					if(Warning_popup(CODE_END_MEASUREMENT) == 1)
					{
						Control_end = 8;                                        //点击了测量完成按钮
						Control_state = S_START;
				    send_mcu_cmd = S_STOP;
				    mcu_send_cmd();
						break;
					}
					else
					{
           if((send_mcu_cmd == S_START)||(send_mcu_cmd == S_CHECK_BLOOD))
					  {
					    Fresh_Control_dispaly(inter_test);
					  }
					  else if((send_mcu_cmd == S_ABSORB)||(send_mcu_cmd == S_PRESS)||(send_mcu_cmd == S_SAMPLE))
					  {
						  Fresh_Control_dispaly(inter_prog);
					  }
					}
				}
				//从机未收到数据或受到错误数据 则计算出错次数 若出错次数多于5次 则结束此次测量并报错
				if((rec_mcu_flag == 1))
				{
					Control_error_time++;
					rec_mcu_flag = 4;
				}
				if(Control_error_time == 5)
				{
					Control_end = 1;
					Control_error_time = 0;
				}
				if(rec_mcu_flag == 3)                        //从机报错
				{
					if(rec_mcu_error == MCU_ERR_CARD_OUT)
					{
						Control_end = 10;
					}
					else if(rec_mcu_error == MCU_ERR_TUBE_OUT)
					{
						Control_end = 11;
					}
					rec_mcu_flag = 4;
				}
		  }
			else                                          //通信完成 则进行下一步操作
			{
				switch(send_mcu_cmd)
				{
					case S_START:                                           //显示插卡标识  防止出错
					{
						if(Chinese_display == 1)
						{
							bmp_dispflash(ADRESS_CAED,106,186,LENGTH_CARD_X,LENGTH_CARD_Y);	
						}
						else
						{
							bmp_dispflash(ADRESS_CAED,107,189,LENGTH_CARD_X,LENGTH_CARD_Y);	
						}
						SERIAL_ROM_Font_mode();
						Graphic_Mode();

            
						//接收SPOT CODE信息并解析
            control_spot_code_h = rec_mcu_buf[5];
		        control_spot_code_m = rec_mcu_buf[6];
		        control_spot_code_l = rec_mcu_buf[7];
						if(
							 (control_spot_code_h == 0) &&                                               
						   (control_spot_code_m == 0) &&   
						   (control_spot_code_l == 1)
						  )                                                 //质控卡质控 则显示插卡标识 并将系统状态切换至下一流程 跳过采血管
						{
							Control_state = S_ABSORB;                                                    //系统状态切换至吸血至staging well状态
								//闪烁合盖标志  提示合上盖子
							for(close_window_time=0;close_window_time<10;close_window_time++)
							{
								if(Control_twinkle_flag == 0)
								{
									if(Chinese_display == 1)
									{
										RA8875_FillRect(450,165,140,200,BACK_COLOUR_16);
									}
									else
									{
										RA8875_FillRect(470,165,140,200,BACK_COLOUR_16);
									}
									
									Control_twinkle_flag = 1;
								}
								else
								{
									if(Chinese_display == 1)
									{
										bmp_dispflash(ADRESS_CLOSE_WINDOW,460,167,LENGTH_WINDOW_X,LENGTH_WINDOW_Y);	
									}
									else
									{
										bmp_dispflash(ADRESS_CLOSE_WINDOW,489,166,LENGTH_WINDOW_X,LENGTH_WINDOW_Y);	
									}
									SERIAL_ROM_Font_mode();
									Graphic_Mode();
									Control_twinkle_flag = 0;
								}
								//扫描是否点击了结束测量按钮并进行相关处理
								if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
								{
									ts_event.x1 = -1;
									ts_event.y1 = -1;
									g_ucTouch = 0;
									_StoreUnstable(-1, -1);
									if(Warning_popup(CODE_END_MEASUREMENT) == 1)
									{
										Control_end = 8;
										Control_state = S_START;
										send_mcu_cmd = S_STOP;
										mcu_send_cmd();
										break;
									}
									else
									{
									 if((send_mcu_cmd == S_START)||(send_mcu_cmd == S_CHECK_BLOOD))
										{
											Fresh_Control_dispaly(inter_test);
										}
										else if((send_mcu_cmd == S_ABSORB)||(send_mcu_cmd == S_PRESS)||(send_mcu_cmd == S_SAMPLE))
										{
											Fresh_Control_dispaly(inter_prog);
										}
									}
								}
								OSTimeDlyHMSM(0, 0, 0, 300);	
							}	
							if(Control_end != 8)
							{
								Fresh_Control_dispaly(inter_prog);
								Control_Progbar_set(0);   
							}							
						}
						else if(1)                                         //质控液质控
						{
							if(1)                                            //spot code已扫描
							{
								;
							}
							else                                             //spot code未扫描
							{
								Control_end = 9;
							}
						}				
						
						break;
					}
					case S_SPOT_CODE:
					{
						control_spot_code_h = rec_mcu_buf[5];
		        control_spot_code_l = rec_mcu_buf[6];
						if(1)                                               //质控卡质控
						{
							;							
						}
						else if(1)                                         //质控液质控
						{
							if(1)                                            //spot code已扫描
							{
								;
							}
							else                                             //spot code未扫描
							{
								Control_end = 9;
							}
						}
						
						break;
					}
					case S_CHECK_BLOOD:                                  //显示采血管标识 防止出错
					{
						if(Chinese_display == 1)
						{
							bmp_dispflash(ADRESS_BLOOD,310,187,LENGTH_BLOOD_X,LENGTH_BLOOD_Y);	
						}
						else
						{
							bmp_dispflash(ADRESS_BLOOD,317,189,LENGTH_BLOOD_X,LENGTH_BLOOD_Y);	
						}	
						SERIAL_ROM_Font_mode();
						Graphic_Mode();
						//闪烁合盖标志  提示合上盖子
						for(close_window_time=0;close_window_time<10;close_window_time++)
						{
							if(Control_twinkle_flag == 0)
							{
								RA8875_FillRect(450,165,140,200,BACK_COLOUR_16);
								Control_twinkle_flag = 1;
							}
							else
							{
								if(Chinese_display == 1)
								{
								  bmp_dispflash(ADRESS_CLOSE_WINDOW,460,167,LENGTH_WINDOW_X,LENGTH_WINDOW_Y);	
								}
								else
								{
									bmp_dispflash(ADRESS_CLOSE_WINDOW,489,166,LENGTH_WINDOW_X,LENGTH_WINDOW_Y);	
								}
								SERIAL_ROM_Font_mode();
								Graphic_Mode();
								Control_twinkle_flag = 0;
							}
							//扫描是否点击了结束测量按钮并进行相关处理
							if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680, 175, 100, 115) && (g_ucTouch == 1))
							{
								ts_event.x1 = -1;
								ts_event.y1 = -1;
								g_ucTouch = 0;
								_StoreUnstable(-1, -1);
								if(Warning_popup(CODE_END_MEASUREMENT) == 1)
								{
									Control_end = 8;
									Control_state = S_START;
									send_mcu_cmd = S_STOP;
									mcu_send_cmd();
									break;
								}
								else
								{
								 if((send_mcu_cmd == S_START)||(send_mcu_cmd == S_CHECK_BLOOD))
									{
										Fresh_Control_dispaly(inter_test);
									}
									else if((send_mcu_cmd == S_ABSORB)||(send_mcu_cmd == S_PRESS)||(send_mcu_cmd == S_SAMPLE))
									{
										Fresh_Control_dispaly(inter_prog);
									}
								}
							}
							OSTimeDlyHMSM(0, 0, 0, 300);
						}
						if(Control_end != 8)
						{
						  Fresh_Control_dispaly(inter_prog);
						
	            Control_Progbar_set(0);   
						}
						break;
					}
					case S_ABSORB:
					{
						Heat_enable();                              //开始加热血液
		        delay_nms(50);	
		        Heat_disable();
						
	          Control_Progbar_set(10);  
						break;
					}
					case S_PRESS:
					{		
	          Control_Progbar_set(20);  
						break;
					}
					case S_SAMPLE:
					{			
	          Control_Progbar_set(100);  
						Control_finish_flag = 1;
						break;
					}
					case S_STOP:
					{						
						break;
					}
				}
				break;
			}
			
			if(Control_end != 0)
			{
				break;
			}
			if(Control_time > 0)
			{
				Control_time--;
				//30s检查一次 若没有收到ack指令 则重新发送命令字节
				if(((Control_time % 100) == 0) && (rec_mcu_flag == 4))
				{
					mcu_send_cmd();
				}
			}
			OSTimeDlyHMSM(0, 0, 0, 300);
		}
		
		if(Control_end != 0)
		{
			FR1000_ERROR_FLAG = Control_end;
			if(Control_end != 8)
			{  
			  Error_popup(Control_end);
			}
			break;
		}
		
		//测量完成 或 出错 则结束该测量流程
		if( (send_mcu_cmd == S_SAMPLE) || (send_mcu_cmd == S_STOP) )
		{
			
			break;
		}
  }
}
void Fresh_Control_dispaly(unsigned char inter)
{
	
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
	
		//显示主界面
	if(Chinese_display == 1)
	{
	  bmp_dispflash(ADRESS_CONTROL_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
	}
	else
	{
	  bmp_dispflash(ADRESS_EN_CONTROL_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
	}
		
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	if(inter == inter_test)
	{
	  if(Chinese_display == 1)
		{
			bmp_dispflash(ADRESS_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);		
		}
		else
		{
			bmp_dispflash(ADRESS_EN_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}			
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
	}
	else if(inter == inter_prog)
	{
		RA8875_FillRect(40,120,165,630,BACK_COLOUR_16);
		if(Chinese_display == 1)
		{
		  GUI_SetFont(&GUI_FontHZ24);
	    GUI_SetBkColor(BACK_COLOUR_24);
	    GUI_SetColor(GUI_BLACK);
	    GUI_DispStringAt("正在质控，请稍候......",40,120);
		}
		else
		{
			GUI_SetFont(&GUI_Font24_ASCII);
	    GUI_SetBkColor(BACK_COLOUR_24);
	    GUI_SetColor(GUI_BLACK);
	    GUI_DispStringAt("Controling,Please Wait......",40,120);
		}
		
	  if(Chinese_display == 1)
		{
			bmp_dispflash(ADRESS_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);		
		}
		else
		{
			bmp_dispflash(ADRESS_EN_CONTROL_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}		
		
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
		
		switch(send_mcu_cmd)
		{
			case S_ABSORB:
			{	
				Control_Progbar_set(0);  
				break;
			}
			case S_PRESS:
			{		
				Control_Progbar_set(10);  
				break;
			}
			case S_SAMPLE:
			{			
				Control_Progbar_set(20 + Control_pro_time/5);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	if(Chinese_display == 1)
	{
		GUI_SetFont(&GUI_FontHZ16);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("质控类型:",42,335);
		GUI_DispStringAt("测试结果:",242,335);
		GUI_DispStringAt("质控时间:",42,375);	GUI_SetFont(&GUI_FontHZ16);	
		GUI_DispStringAt(Control_type,120,335);
		//GUI_DispStringAt(Control_result,320,335);
		
		GUI_SetFont(&GUI_Font16_ASCII);		   //时间
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(control_date_buf,120,375);
	}
	else
	{	
		GUI_SetFont(&GUI_Font16B_ASCII);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("Control Type:",42,335);
		GUI_DispStringAt("Control Result:",242,335);
		GUI_DispStringAt("Control Time:",42,375);
		GUI_DispStringAt(Control_type,130,335);
		GUI_DispStringAt(control_date_buf,130,375);
	}	
	
	
	
	
}

void Control_Progbar_set(unsigned char Progbar_data)
{
	/*建立进度条控件 显示质控进度*/
	PROGBAR_CONTROL = PROGBAR_Create(PROGBAR_CONTROL_X,PROGBAR_CONTROL_Y, PROGBAR_CONTROL_SIZEX, PROGBAR_CONTROL_SIZEY, WM_CF_SHOW);  
	PROGBAR_EnableMemdev(PROGBAR_CONTROL);
	PROGBAR_SetBarColor(PROGBAR_CONTROL, 0, PROGBAR_CONTROL_CLR);       //已刷新的进度条颜色
	PROGBAR_SetBarColor(PROGBAR_CONTROL, 1, PROGBAR_CONTROL_CLR_BKG);       //未刷新的进度条颜色
	PROGBAR_SetTextAlign(PROGBAR_CONTROL, GUI_TA_CENTER);     //字体位于进度条中心
	PROGBAR_SetMinMax(PROGBAR_CONTROL,0,100);                 //进度条的最大值和最小值
	PROGBAR_SetTextColor(PROGBAR_CONTROL,0,GUI_BLACK);        //设置字体颜色
	PROGBAR_SetTextColor(PROGBAR_CONTROL,1,GUI_BLACK);        //设置字体颜色
	
	PROGBAR_SetValue(PROGBAR_CONTROL, Progbar_data);   
	
	GUI_Exec();		
	
	PROGBAR_Delete(PROGBAR_CONTROL);
}

extern int search_fit_result[2048];
extern NAND_ADDRESS WriteReadAddr_control;
extern unsigned long g_result_count_control;
extern int search_len;
extern GUI_CONST_STORAGE GUI_BITMAP bmlishiquanxuanhuanyuan0;
extern GUI_CONST_STORAGE GUI_BITMAP bmlishiquanxuanhuanyuan1;
extern GUI_CONST_STORAGE GUI_BITMAP bmquanxuan2;
extern bool quanxuan_flag,danxuan_flag,dele_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bmlishiquanxuan2;
extern GUI_CONST_STORAGE GUI_BITMAP bmlishiquanxuan;
extern GUI_CONST_STORAGE GUI_BITMAP bmquxiaoquanxuan1;
extern const GUI_WIDGET_CREATE_INFO _aExcuteDELEDlg[4];
extern bool print_outofpage_flag,print_error_flag,daying_flag,dayingjiemianhuanyuan_flag,sum_result_zero_xuanzhe_flag
,sum_result_zero_daying_flag,sum_result_zero_del_flag;
extern bool is_dele_flag ;
extern bool dele_flag;

extern bool control_del_flag;

unsigned long g_result_count_control=0;
struct _Save_Date_Control_Struct Control_save_data;
bool His_CON_flag=0;
static bool danxuan_control_sameflag;
static  int page_num = 0;
static unsigned int page_flag = 0;
static bool danxuan_control[8]={0};
static unsigned char tmpbuffer[512];
int  g_current_control_page = 0;
void Control_his_deal(bool Create_or_Search)
{

	
	    short aaup99=99, bbup363=363, ccup50=50, ddup288=288,aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
			unsigned int key_num;
			int j,i,count,sum_result=0,g_current_control_page_mid=0;
			
			OS_CPU_SR  cpu_sr;
			static char sel=0,del_count=0;
			unsigned int addressstatus;
			unsigned  char quanxuandel_count=0,danxuan_control_len=0;
			unsigned char str_page[8]={0},t[6]={0};
			NAND_ADDRESS ReadFlashAddr;
			NAND_ADDRESS DelFlashAddr;
			NAND_ADDRESS CurFlashAddr;
      NAND_ADDRESS BaseFlashAddr;
	PAGE_START:
      His_CON_flag=1;
      OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
			OSIntNesting++;																    
      if(!Create_or_Search)		
			 {
						I2C_Configuration();
						AT24C512_ReadPage(ADDR_24C512,0x54,(unsigned char *)&WriteReadAddr_control, 6);

						g_result_count_control=0;
						I2C_Configuration();
						AT24C512_ReadPage(ADDR_24C512,0x5a,(unsigned char *)&g_result_count_control, 4);
				    sum_result=g_result_count_control;
						g_current_control_page=0;
						I2C_Configuration();
						AT24C512_ReadPage(ADDR_24C512,0x5e,(unsigned char *)&g_current_control_page, 4);
				 
			 }
        BaseFlashAddr=WriteReadAddr_control;
	
				//算基地址
				for(i=0;i<g_result_count_control;i++)
				{ 
							do
							{
								for(j=0;j<g_bad_block_count;j++)
								 {
										if(bad_block[j] == (BaseFlashAddr.Page + (BaseFlashAddr.Block + (BaseFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
									{
										BaseFlashAddr.Block--;
										b_invalid_block = 1;
										break;	
									}else
									{
										b_invalid_block = 0;
									}
									
								 }
								
							}while(b_invalid_block == 1);
							if((b_invalid_block == 0) && (BaseFlashAddr.Page == 0))
							{
									 FSMC_NAND_AddressDecrement(&BaseFlashAddr);
								
							}
							
				}
       
//			g_flash_addr = (WriteReadAddr_control.Page + (WriteReadAddr_control.Block + (WriteReadAddr_control.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
      if(Create_or_Search)
      {
          sum_result=search_len;
				  g_current_control_page_mid=g_current_control_page;
          g_current_control_page=search_len;
				 
      }
			else
			{
          sum_result=g_result_count_control;
          
      }
// 			bmp_dispflash(0x00,0,40,800,440);	
// 			SERIAL_ROM_Font_mode();
// 			Graphic_Mode();
				bmp_dispflash(ADRESS_CON_HIS_R1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
				SERIAL_ROM_Font_mode();
				Graphic_Mode();
				bmp_dispflash(ADRESS_CON_HIS_L,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
				SERIAL_ROM_Font_mode();
				Graphic_Mode();
			
//       if(Create_or_Search)
//       {
// 				
// 				  GUI_DrawBitmap(&bmlishi_back ,  683, 47);GUI_DrawBitmap(&bmdel_disa ,  683, 235);
// 				  
// 			}
			// 	for(i=0;i<=3;i++)
			// 	{
			// 			 
			// 			 if(danxuan_control[2*i+1]==1)//
			// 			 {
			// 						 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 87+(2*i+1)*36);
			// 					
			// 			 }
			// 			 else if(danxuan_control[2*i]==1)
			// 			 {
			// 						 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 87+(2*i)*36);
			// 						

			// 			 }

			// 	}
			PAGE0:
			memset(&Control_save_data,0,sizeof(Control_save_data));

      if(!Create_or_Search)
			{

				
					    ReadFlashAddr.Zone =BaseFlashAddr.Zone+ (g_current_control_page ) / (1024);
						  ReadFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page  - (g_current_control_page ) / (1024)*1024-1 );
						  ReadFlashAddr.Page = 0;
				    
			}
			if(g_current_control_page/8>0)		
			{														
						for(i=0;i<=3;i++)
						{
								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+(2*i)*36);
								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+(2*i+1)*36);
							//
						}
			}
			else
			{
						for(i=0;i<sum_result%8;i++)
						{
								 if(i%2)
								 {
											 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
								 }
								 else
								 {
											 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);

								 }

						}

			}
		  GUI_SetFont(&GUI_FontHZ32);
			if(g_current_control_page/8==0||sum_result==0)	
			for(i=sum_result%8;i<8;i++)
			{
						

							if(i%2==0)
							{
											
											GUI_SetBkColor(0xF1F1F1);
											if(i==0)
											{
													 
															
															GUI_DispStringAt("      ",30,84);
											}
											else if(i==2)
											{
													 
															
															GUI_DispStringAt("      ",30,155);
											}
											else if(i==4)
											{
													 
														
															GUI_DispStringAt("      ",30,229);
											}
											else if(i==6)
											{
														
														 GUI_DispStringAt("      ",30,301);
											}
											
							}
							else
							{
											GUI_SetBkColor(0xd9d9da);																									
											if(i==1)
											{
													 
														
															GUI_DispStringAt("      ",30,120);
											}
											else if(i==3)
											{
													 
													
															GUI_DispStringAt("      ",30,192);
											}
											else if(i==5)
											{
													 
															GUI_DispStringAt("      ",30,264);
											}
											else if(i==7)
											{
														
														 GUI_DispStringAt("      ",30,336);
											}
							
							}

			}
			if(quanxuan_flag==1)
			{
					    quanxuan_flag=0;
				      GUI_DrawBitmap(&bmquanxuan2 ,  695, 197+40);
							sel=9;      

			}
			if(danxuan_flag==1)
			{
						 danxuan_flag=0;
						 if(sel>=0&&sel<8)
						 {
												if(sel%2)
											 {
														 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+sel*36);
														 danxuan_control[sel]=0;
											 }
											 else
											 {
														 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+sel*36);
														 danxuan_control[sel]=0;

											 }
						 }
						 sel=9;

			}
   
			
			if(sum_result < 8&&sum_result>0)
			{
				        GUI_SetBkColor(0xF1F1F1);
								GUI_SetColor(GUI_BLACK);
								GUI_SetFont(&GUI_FontHZ32x32);
								if(sum_result%8!=0)
								{
												sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
												strcat(str_page,t);
												strcat(str_page,"/");
												sprintf(t,"%d",sum_result/8+1);//
												strcat(str_page,t);
												GUI_DispStringAt(str_page,34,381);
								}
								else
								{
												sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
												strcat(str_page,t);
												strcat(str_page,"/");
												sprintf(t,"%d",sum_result/8);//
												strcat(str_page,t);
												GUI_DispStringAt(str_page,34,381);


								}
								for(i = 0; i < g_current_control_page; i++)	 //g_flash_addr	   36	  
								{
									   
											close_lcd_cs();enable_nandflash_cs();
										  if(Create_or_Search)
  											{
													ReadFlashAddr.Zone =search_fit_result[g_current_control_page-1-i] / (1024*32);
													ReadFlashAddr.Block = (search_fit_result[g_current_control_page-1-i] - (g_current_control_page ) / (1024)*1024*32) / 32;
													ReadFlashAddr.Page = search_fit_result[g_current_control_page-1-i] % 32;
										
											}
										
											do
											{
														for(j=0;j<g_bad_block_count;j++)
														 {
																		if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																		{
																					ReadFlashAddr.Block--;
																					b_invalid_block = 1;
																					break;	
																		}
																		else
																		{
																			    b_invalid_block = 0;
																		}  	
														 }
												
											}while(b_invalid_block == 1);
											if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
											{
															Loop1:
																if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage(tmpbuffer,ReadFlashAddr,1))   //tmpbuffer   (unsigned char *)&Control_save_data
																{
																			bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																			g_bad_block_count++;
																			memset(ZeroBuf,0,sizeof(ZeroBuf));
																			tmp_addr.Page = ReadFlashAddr.Page + 512;
																			tmp_addr.Block = ReadFlashAddr.Block;
																			tmp_addr.Zone = ReadFlashAddr.Zone;

																			FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																			FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																			goto Loop1;	
																}
																memcpy((unsigned char *)&Control_save_data,tmpbuffer,sizeof(tmpbuffer));
												
											}
											 if(!Create_or_Search)
											{
											      FSMC_NAND_AddressDecrement(&ReadFlashAddr);
											}
											close_nandflash_cs();enable_lcd_cs();
											count = 0;		
											GUI_SetColor(GUI_BLACK);
											GUI_SetFont(&GUI_FontHZ16);						
											if(i%2==0)
											{
												
															GUI_SetBkColor(0xF1F1F1);	
															GUI_DispStringAt(Control_save_data.date,70,88+i*38);
															GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
															GUI_DispStringAt(Control_save_data.result,355,88+i*38);
															GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
												
															
															
											}
											else
											{

															GUI_SetBkColor(0xd9d9da);
														
															GUI_DispStringAt(Control_save_data.date,70,88+i*38);
															GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
															GUI_DispStringAt(Control_save_data.result,355,88+i*38);
															GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
												
															
															
											
											}
								}
					
								i=sum_result;	
								GUI_SetFont(&GUI_FontHZ16);
								for(;i<8;i++)
								{
												if(i%2==0)
												{
																
																GUI_SetBkColor(0xF1F1F1);
																
																	
																GUI_DispStringAt("                ",70,88+i*38);
																GUI_DispStringAt("        ",224,88+i*38);
																GUI_DispStringAt("        ",355,88+i*38);
																GUI_DispStringAt("                ",521,88+i*38);
																
																
								
																
												}
												else
												{
																GUI_SetBkColor(0xd9d9da);
														 
																GUI_DispStringAt("                ",70,88+i*38);
																GUI_DispStringAt("        ",224,88+i*38);
																GUI_DispStringAt("        ",355,88+i*38);
																GUI_DispStringAt("                ",521,88+i*38);
																
																
							
												}

								}					
							
							dele_flag= 0;
							dayingjiemianhuanyuan_flag=0;
          								
			}
			else if(sum_result >= 8)
			{
				      GUI_SetBkColor(0xF1F1F1);
							GUI_SetColor(GUI_BLACK);
							GUI_SetFont(&GUI_FontHZ32x32);
								if(sum_result%8!=0)
								{
												sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
												strcat(str_page,t);
												strcat(str_page,"/");
												sprintf(t,"%d",sum_result/8+1);//
												strcat(str_page,t);
												GUI_DispStringAt(str_page,34,381);
								}
								else
								{
												sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
												strcat(str_page,t);
												strcat(str_page,"/");
												sprintf(t,"%d",sum_result/8);//
												strcat(str_page,t);
												GUI_DispStringAt(str_page,34,381);


								}
				    
							for(i = 0; i < 8; i++)	 //g_flash_addr	   36	  
							{

										 close_lcd_cs();enable_nandflash_cs();
					           if(Create_or_Search)
										 {
													ReadFlashAddr.Zone = search_fit_result[g_current_control_page-1-i] / (1024*32);
													ReadFlashAddr.Block =(search_fit_result[g_current_control_page-1-i] - (g_current_control_page ) / (1024)*1024*32) / 32;
													ReadFlashAddr.Page = search_fit_result[g_current_control_page-1-i] % 32;
										
										 }
							
										do
										{
													 for(j=0;j<g_bad_block_count;j++)
													 {
																	if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																	{
																					ReadFlashAddr.Block--;
																					b_invalid_block = 1;
																					break;	
																	}
																	else
																	{
																		      b_invalid_block = 0;
																	}	
													 }	
										}while(b_invalid_block == 1);
										if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
										{
										Loop:
													if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,ReadFlashAddr,1))
													{
																	bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																	g_bad_block_count++;
																	memset(ZeroBuf,0,sizeof(ZeroBuf));
																	tmp_addr.Page = ReadFlashAddr.Page + 512;
																	tmp_addr.Block = ReadFlashAddr.Block;
																	tmp_addr.Zone = ReadFlashAddr.Zone;
																	FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																	FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																	goto Loop;	
													}
										}
									  if(!Create_or_Search)
										{
												FSMC_NAND_AddressDecrement(&ReadFlashAddr);
										}
										close_nandflash_cs();enable_lcd_cs();
										count = 0;
										  
							      GUI_SetFont(&GUI_FontHZ16);
										if(i%2==0)
										{
											
														GUI_SetBkColor(0xF1F1F1);
												
														GUI_DispStringAt(Control_save_data.date,70,88+i*38);
														GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
														GUI_DispStringAt(Control_save_data.result,355,88+i*38);
														GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
										
														
														
										}
										else
										{

														GUI_SetBkColor(0xd9d9da);
													
														GUI_DispStringAt(Control_save_data.date,70,88+i*38);
														GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
														GUI_DispStringAt(Control_save_data.result,355,88+i*38);
														GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
										
														
														
										
										}

							}
              dele_flag= 0;	
							dayingjiemianhuanyuan_flag=0;
									
					   	
			} 
			else if(sum_result==0)
			{
							GUI_SetBkColor(0xF1F1F1);
							GUI_SetColor(GUI_BLACK);
							GUI_SetFont(&GUI_FontHZ32x32);
							strcat(str_page,"0");
							strcat(str_page,"/");															
							strcat(str_page,"0");
							GUI_DispStringAt(str_page,34,381);


								

			}
	    close_nandflash_cs();enable_lcd_cs();
			if(g_current_control_page/8<sum_result/8&&(g_current_control_page>8)&&(sum_result%8!=0))
			{
						 aaup99=99, bbup363=363, ccup50=50, ddup288=288;
						 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
			}
			else if(g_current_control_page/8==0&&sum_result>8)//第一页
			{
						 aaup99=99, bbup363=363, ccup50=50, ddup288=288;
						 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
						
			}
			else if(g_current_control_page/8==sum_result/8&&sum_result>8)//最后一页
			{
						 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
						 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
						 
				
			}
			else if(g_current_control_page==sum_result&&sum_result<=8)
			{
						 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
						 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
			}
 	    OS_EXIT_CRITICAL();
 		  OSIntExit();        //触发任务切换软中断
      //while((ts_event.Key_Sta==Key_Up));
	    do
	    {
								//sel 9为无效值，8为全选，0--7为单选
							 if (TOUCH_InRect(ts_event.x1, ts_event.y1, 33, 86, 291, 58) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//选择
								{
									    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		                	OSIntNesting++;	
												 //先清除全选
											if(quanxuan_flag==1)
											{
														 memset(danxuan_control,0,sizeof(danxuan_control[0])*8);
														 quanxuan_flag=0;
												     
														 sel=9;
														 if(g_current_control_page/8>0)
														 {
																					 for(i=0;i<4;i++)
																					{
																							 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+(2*i)*36);
																							 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+(2*i+1)*36);
																					}
																		 
															}
															else if(g_current_control_page/8==0)
															{
																			 for(i=0;i<sum_result%8;i++)
																			 {
																						 if(i%2)
																						 {

																									 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
																						 }
																						 else
																						{
																									 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);
																						}
																			}
																		 

															}
														 
											}
											if(g_current_control_page/8>0)
											{
												    danxuan_control_len=8;
                      }
											else if(g_current_control_page/8==0)
											{
												   danxuan_control_len=sum_result%8;
											}
												
											for(i=0;i<danxuan_control_len;i++)
											{
														if (TOUCH_InRect(ts_event.x1, ts_event.y1, 20, 80+36*i, 36, 58) && (g_ucTouch == 1))
														{
																		
																		if(sel==i)
																		{
																					 if(danxuan_control_sameflag==0)
																					 {
																								 danxuan_control_sameflag=1;
                                                 
																								 if(i%2)
																								 {
																												 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+i*36);
																									       danxuan_flag=1;
																												
																								 }
																								 else
																								 {

																												 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+i*36);
																									       danxuan_flag=1;
																												
																								 }
																					 }
																					 else
																					 {
																								 danxuan_control_sameflag=0;
																						     
																								 if(i%2)
																								 {
																											 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
																									     danxuan_flag=0;
																											 
																								 }
																								 else
																								 {
																											 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);
																									     danxuan_flag=0;
																											

																								 }

																					 }
																					 
																					 
																					 

																		}
																		else  if(sel!=i&&sel!=9&&sel!=danxuan_control_len)
																		{
																			      danxuan_flag=1;
																					 if(sel%2)
																					 {
																								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+sel*36);
																								 
																					 }
																					 else
																					 {
																								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+sel*36);
																								

																					 }
																					
																					 if(i%2)
																					 {
																									 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+i*36);
																									
																					 }
																					 else
																					 {

																									 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+i*36);
																									 
																					 }
																					 

																		}
																		else  if(sel!=i&&sel==danxuan_control_len)
																		{
																			      danxuan_flag=1;
																	
																					
																					 if(i%2)
																					 {
																									 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+i*36);
																									
																					 }
																					 else
																					 {

																									 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+i*36);
																									 
																					 }
																					 

																		}
																		else  if(sel!=i&&sel==9)
																		{
																		
																					 danxuan_flag=1;
																					 if(i%2)
																					 {
																									 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+i*36);
																									 
																					 }
																					 else
																					 {

																									 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+i*36);
																									
																					 }
																					 

																		}
																	  sel=i;
																		break;
																	 
															}
												}
												g_ucTouch=0;
												 OS_EXIT_CRITICAL();
 		                     OSIntExit();        //触发任务切换软中断

												
															 
																				
																					 
												

								} 
								else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 685, 121, 73, 100) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//搜索
								{
									    
										  if(Create_or_Search)
											{
													
													 g_current_control_page=g_current_control_page_mid;
											}
											Create_Seach();
											Create_or_Search=0;
											goto PAGE_START;
										
											 
								}
								else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 685, 46, 73, 100) && (g_ucTouch == 1))//返回
								{
                    
									     break;
										 
											 
								}
								else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 685, 197, 73, 100) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//全选
								{
											 
											 if(sum_result==0)
											 {
												         sum_result_zero_xuanzhe_flag=1;
                                 GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
												         sum_result_zero_xuanzhe_flag=0;
                                 goto PAGE_START;

                       }
											 else
											 {
																	 if(danxuan_flag==1)
																	 {
																				 danxuan_flag=0;
																	 }
																	 if(g_current_control_page/8>0)
																	 {
																					 if(!quanxuan_flag)
																					 {
																								quanxuan_flag=1;
																								 GUI_DrawBitmap(&bmquxiaoquanxuan1 ,  695, 197+40);
																								sel=8;
																								 for(i=0;i<=3;i++)
																								{
																										 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+(2*i)*36);
																										 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+(2*i+1)*36);
																								}
																					 }
																					 else
																					 {
																								quanxuan_flag=0;
																								GUI_DrawBitmap(&bmquanxuan2 ,  695, 197+40);
																								sel=9;
																								 for(i=0;i<=3;i++)
																								{
																										 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+(2*i)*36);
																										 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+(2*i+1)*36);
																								}
																					 }
																		}
																		else if(g_current_control_page/8==0)
																		{
																					 if(!quanxuan_flag)
																					 {
																								quanxuan_flag=1;//bmquxiaoquanxuan1
																								GUI_DrawBitmap(&bmquxiaoquanxuan1 ,  695, 197+40);
																								sel=8;
																								 for(i=0;i<sum_result%8;i++)
																								{
																									
																										 if(i%2)
																										 {

																													 GUI_DrawBitmap(&bmlishiquanxuan2 ,  31, 86+i*36);
																										 }
																										 else
																										{
																													 GUI_DrawBitmap(&bmlishiquanxuan ,  31, 86+i*36);
																										}
																								}
																					 }
																					 else
																					 {
																								quanxuan_flag=0;
																								GUI_DrawBitmap(&bmquanxuan2 ,  695, 197+40);
																								sel=9;
																							
																									 for(i=0;i<sum_result%8;i++)
																								{
																									
																											 if(i%2)
																											 {

																														 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
																											 }
																											 else
																											{
																														 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);
																											}
																								}
																					 }

																		}
																		
											}
                      g_ucTouch=0;
											 
								}//Create_Seach(void);
								else if (TOUCH_InRect(ts_event.x1, ts_event.y1, aaup99, bbup363, ccup50, ddup288) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//上页
									
								{				
													PAGE1:
										      if((page_num>0&&!dele_flag)||(g_current_control_page%8==0))
													     page_num --;
													page_flag = 1;	
                       											
													if(!dele_flag)										
													if(g_current_control_page<=sum_result-8&&g_current_control_page>=0)
													g_current_control_page  += 8;  //进行分页显示
                          if(!Create_or_Search)	
													{
																	I2C_Configuration();
																	AT24C512_WritePage(ADDR_24C512,0x5e,(unsigned char *)&g_current_control_page, 4);
																	
                          }
													if(g_current_control_page/8>0)		
                          {														
																for(i=0;i<=3;i++)
																{
																		 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+(2*i)*36);
																		 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+(2*i+1)*36);
																	//
																}
												  }
													else
													{
                                for(i=0;i<sum_result%8;i++)
																{
																	   if(i%2)
																		 {
                                           GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
                                     }
																		 else
																		 {
                                           GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);

                                     }
																		
																		 
																	//
																}

                          }
													GUI_SetFont(&GUI_FontHZ32);
													if(g_current_control_page/8==0||sum_result==0)	
													for(i=sum_result%8;i<8;i++)
													{
																

																	if(i%2==0)
																	{
																					
																					GUI_SetBkColor(0xF1F1F1);
																					if(i==0)
																					{
																							 
																									
																									GUI_DispStringAt("      ",30,84);
																					}
																					else if(i==2)
																					{
																							 
																									
																									GUI_DispStringAt("      ",30,155);
																					}
																					else if(i==4)
																					{
																							 
																								
																									GUI_DispStringAt("      ",30,229);
																					}
																					else if(i==6)
																					{
																								
																								 GUI_DispStringAt("      ",30,301);
																					}
																					
																	}
																	else
																	{
																					GUI_SetBkColor(0xd9d9da);																									
																					if(i==1)
																					{
																							 
																								
																									GUI_DispStringAt("      ",30,120);
																					}
																					else if(i==3)
																					{
																							 
																							
																									GUI_DispStringAt("      ",30,192);
																					}
																					else if(i==5)
																					{
																							 
																									GUI_DispStringAt("      ",30,264);
																					}
																					else if(i==7)
																					{
																								
																								 GUI_DispStringAt("      ",30,336);
																					}
																	
																	}

													}
													if(quanxuan_flag==1)
													{
																quanxuan_flag=0;
														    GUI_DrawBitmap(&bmquanxuan2 ,  695, 197+40);
																sel=9;
													}					
													
													//FSMC_NAND_Init();
													//往上翻：第1种情况：处于翻页中间，前有页后也有页
													//		  第2种情况：处于最后一页，后面没有页
													//        第3种情况：处于第一页，前面没有页
													if(g_current_control_page  >= 8)
													{
														       if(!Create_or_Search)
																	 {
																					ReadFlashAddr.Zone =BaseFlashAddr.Zone+ (g_current_control_page ) / (1024);
																					ReadFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page  - (g_current_control_page ) / (1024)*1024)-1;
																					ReadFlashAddr.Page = 0;
																	 }
														      
																	
																	for(i = 0; i < 8; i++)	 //g_flash_addr	   36	  
																	{
																					
																					close_lcd_cs();enable_nandflash_cs();
																				  if(Create_or_Search)
																					{
																							ReadFlashAddr.Zone = search_fit_result[g_current_control_page-1-i] / (1024*32);
																							ReadFlashAddr.Block =(search_fit_result[g_current_control_page-1-i] - (g_current_control_page ) / (1024)*1024*32) / 32;
																							ReadFlashAddr.Page = search_fit_result[g_current_control_page-1-i] % 32;
																				
																					}
																					do
																					{
																									for(j=0;j<g_bad_block_count;j++)
																									{
																												if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																												{
																															ReadFlashAddr.Block--;
																															b_invalid_block = 1;
																															break;	
																												}else
																												{
																															 b_invalid_block = 0;
																												}					  	
																									}	
																					}while(b_invalid_block == 1);
																					if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
																					{
																									Loop3:
																									if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,ReadFlashAddr,1))
																									{
																													bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																													g_bad_block_count++;
																													memset(ZeroBuf,0,sizeof(ZeroBuf));
																													tmp_addr.Page = ReadFlashAddr.Page + 512;
																													tmp_addr.Block = ReadFlashAddr.Block;
																													tmp_addr.Zone = ReadFlashAddr.Zone;
																													FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																													FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																													goto Loop3;	
																									}	
																					}									
																						//FSMC_NAND_AddressIncrement(&ReadFlashAddr);
																					 if(!Create_or_Search)
																					{
																							FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																					}
																					close_nandflash_cs();enable_lcd_cs();
																					count = 0;
                                          GUI_SetColor(GUI_BLACK);
																	        GUI_SetFont(&GUI_FontHZ16);	
																					if(i%2==0)
																					{
																						
																									GUI_SetBkColor(0xF1F1F1);
																								
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																		
																									
																									
																					}
																					else
																					{

																									GUI_SetBkColor(0xd9d9da);
																								
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																						
																									
																									
																					
																					}
																					
																				
																				
																	}
																	dele_flag= 0;	
                                  dayingjiemianhuanyuan_flag=0;																	
										       }

														GUI_SetBkColor(0xF1F1F1);
														GUI_SetColor(GUI_BLACK);
														GUI_SetFont(&GUI_FontHZ32x32);
														if(sum_result%8!=0)
														{
																		sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
																		strcat(str_page,t);
																		strcat(str_page,"/");
																		sprintf(t,"%d",sum_result/8+1);//
																		strcat(str_page,t);
																		GUI_DispStringAt(str_page,34,381);
														}
														else
														{
																		sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
																		strcat(str_page,t);
																		strcat(str_page,"/");
																		sprintf(t,"%d",sum_result/8);//
																		strcat(str_page,t);
																		GUI_DispStringAt(str_page,34,381);


														}
														if(danxuan_flag==1)
														{
																	 danxuan_flag=0;
																	 
																	 if(sel>=0&&sel<8)
																	 {
																							
																							if(sel%2)
																						 {
																									 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+sel*36);
																									 danxuan_control[sel]=0;
																						 }
																						 else
																						 {
																									 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+sel*36);
																									 danxuan_control[sel]=0;

																						 }
																	 }
																	 sel=9;

														}
														if(g_current_control_page/8<sum_result/8&&(g_current_control_page>8)&&(sum_result%8!=0))
													{
																 aaup99=99, bbup363=363, ccup50=50, ddup288=288;
																 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
													}
													else if(g_current_control_page/8==0&&sum_result>8)//第一页
													{
														     aaup99=99, bbup363=363, ccup50=50, ddup288=288;
																 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
																
													}
													else if(g_current_control_page/8==sum_result/8&&sum_result>8)//最后一页
													{
                                 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
																 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
																 
														
													}
													else if(g_current_control_page==sum_result&&sum_result<=8)
													{
																 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
																 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
													}
								
								}
								else if (TOUCH_InRect(ts_event.x1, ts_event.y1, aadown388, bbdown363, ccdown50, dddown288) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//下页
								{
									PAGE2:
													if((page_num<sum_result/8&&!dele_flag)||(g_current_control_page%8==0))
													{   
															 page_num ++;
													}
													page_flag = 2;

											    if(!dele_flag)
											    if(g_current_control_page>=8&&g_current_control_page<=sum_result)
													 g_current_control_page -= 8;	//进行分页显示
                           if(!Create_or_Search)	
													{
																	I2C_Configuration();
																	AT24C512_WritePage(ADDR_24C512,0x5e,(unsigned char *)&g_current_control_page, 4);
																	
                          }
// 													if(dele_flag||dayingjiemianhuanyuan_flag)
// 													{
// 																bmp_dispflash(0x00,0,40,800,440);	
// 																SERIAL_ROM_Font_mode();
// 																Graphic_Mode();
// 														    if(Create_or_Search)
// 																{
// 																		GUI_DrawBitmap(&bmlishi_back ,  683, 47);GUI_DrawBitmap(&bmdel_disa ,  683, 235);
// 																}
// 																

// 													}
													if(dele_flag||dayingjiemianhuanyuan_flag)
													{
																bmp_dispflash(ADRESS_CON_HIS_R1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
																SERIAL_ROM_Font_mode();
																Graphic_Mode();
																bmp_dispflash(ADRESS_CON_HIS_L,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
																SERIAL_ROM_Font_mode();
																Graphic_Mode();
// 														    if(Create_or_Search)
// 																{
// 																		GUI_DrawBitmap(&bmlishi_back ,  683, 47);GUI_DrawBitmap(&bmdel_disa ,  683, 235);
// 																}
																

													}
												
													
                          if(g_current_control_page/8>0)		
                          {														
																for(i=0;i<=3;i++)
																{
																		 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+(2*i)*36);
																		 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+(2*i+1)*36);
																	//
																}
												  }
													else
													{
                                for(i=0;i<sum_result%8;i++)
																{
																	   if(i%2)
																		 {
                                           GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+i*36);
                                     }
																		 else
																		 {
                                           GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+i*36);

                                     }
																		
																		 
																	//
																}

                          }
													GUI_SetFont(&GUI_FontHZ32);
													if(g_current_control_page/8==0||sum_result==0)	
													for(i=sum_result%8;i<8;i++)
													{
																

																	if(i%2==0)
																	{
																					
																					GUI_SetBkColor(0xF1F1F1);
																					if(i==0)
																					{
																							 
																									
																									GUI_DispStringAt("      ",30,84);
																					}
																					else if(i==2)
																					{
																							 
																									
																									GUI_DispStringAt("      ",30,155);
																					}
																					else if(i==4)
																					{
																							 
																								
																									GUI_DispStringAt("      ",30,229);
																					}
																					else if(i==6)
																					{
																								
																								 GUI_DispStringAt("      ",30,301);
																					}
																					
																	}
																	else
																	{
																					GUI_SetBkColor(0xd9d9da);																									
																					if(i==1)
																					{
																							 
																								
																									GUI_DispStringAt("      ",30,120);
																					}
																					else if(i==3)
																					{
																							 
																							
																									GUI_DispStringAt("      ",30,192);
																					}
																					else if(i==5)
																					{
																							 
																									GUI_DispStringAt("      ",30,264);
																					}
																					else if(i==7)
																					{
																								
																								 GUI_DispStringAt("      ",30,336);
																					}
																	
																	}

													}
													if(quanxuan_flag==1)
													{
																quanxuan_flag=0;
														    GUI_DrawBitmap(&bmquanxuan2 ,  695, 197+40);
																sel=9;				
											
													}
													if(danxuan_flag==1)
													{
																 danxuan_flag=0;
																 if(sel>=0&&sel<8)
																 {
																						
																						if(sel%2)
																					 {
																								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+sel*36);
																								 danxuan_control[sel]=0;
																					 }
																					 else
																					 {
																								 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+sel*36);
																								 danxuan_control[sel]=0;

																					 }
																 }
																 sel=9;

													}
											
												
										 
													//FSMC_NAND_Init();
													//处于翻页中间，可以有完整的下一页20条记录
													if(g_current_control_page  >= 8)
													{
																	if(!Create_or_Search)
																	{
																			ReadFlashAddr.Zone =BaseFlashAddr.Zone+ (g_current_control_page ) / (1024);
																			ReadFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page  - (g_current_control_page ) / (1024)*1024)-1;
																			ReadFlashAddr.Page = 0;
																	}
																	for(i = 0; i < 8; i++)	 //g_flash_addr	   36	  
																	{
																		
																				  close_lcd_cs();enable_nandflash_cs();
																					if(Create_or_Search)
																					{
																							ReadFlashAddr.Zone = search_fit_result[g_current_control_page-1-i] / (1024*32);
																							ReadFlashAddr.Block =(search_fit_result[g_current_control_page-1-i] - (g_current_control_page ) / (1024)*1024*32) / 32;
																							ReadFlashAddr.Page = search_fit_result[g_current_control_page-1-i] % 32;
																					}
																				
																					do
																					{
																								for(j=0;j<g_bad_block_count;j++)
																								 {
																													if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																													{
																																ReadFlashAddr.Block--;
																																b_invalid_block = 1;
																																break;	
																													}
																													else
																													{
																																 b_invalid_block = 0;
																													}
																								 }
																					}while(b_invalid_block == 1);
																					if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
																					{
																						Loop5:
																									if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,ReadFlashAddr,1))
																									{
																										bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																										g_bad_block_count++;
																										memset(ZeroBuf,0,sizeof(ZeroBuf));
																										tmp_addr.Page = ReadFlashAddr.Page + 512;
																										tmp_addr.Block = ReadFlashAddr.Block;
																										tmp_addr.Zone = ReadFlashAddr.Zone;
																										FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																										FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																										goto Loop5;	
																									}	
																					}
																					if(!Create_or_Search)
																					{
																							FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																					}
																					close_nandflash_cs();enable_lcd_cs();
																					GUI_SetColor(GUI_BLACK);
																					GUI_SetFont(&GUI_FontHZ16);
																					count = 0;
																					if(i%2==0)
																					{
																						
																									GUI_SetBkColor(0xF1F1F1);
																								
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																				
																									
																									
																					}
																					else
																					{

																									GUI_SetBkColor(0xd9d9da);
																								
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																				
																									
																									
																					
																					}
																	
																
																		
																			}
																			
																			dele_flag = 0;
																			dayingjiemianhuanyuan_flag=0;
													}
													//处于最后一页的前一页，且最后一页没有满
													else if(g_current_control_page  < 8)
													{
													
													        if(!Create_or_Search)		
																	{
																			ReadFlashAddr.Zone =BaseFlashAddr.Zone+ (g_current_control_page ) / (1024);
																			ReadFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page  - (g_current_control_page ) / (1024)*1024 )-1;
																			ReadFlashAddr.Page = 0;
																	}
														      
																	for(i = 0; i < g_current_control_page ; i++)	 //g_flash_addr	   36	  
																	{
																		
																		      close_lcd_cs();enable_nandflash_cs();
															            if(Create_or_Search)
																					{
																							ReadFlashAddr.Zone = search_fit_result[g_current_control_page-1-i] / (1024*32);
																							ReadFlashAddr.Block =(search_fit_result[g_current_control_page-1-i] - (g_current_control_page ) / (1024)*1024*32) / 32;
																							ReadFlashAddr.Page = search_fit_result[g_current_control_page-1-i] % 32;
															
																					}
																			
																					do
																					{
																								for(j=0;j<g_bad_block_count;j++)
																								{
																										if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																									  {
																												ReadFlashAddr.Block--;
																												b_invalid_block = 1;
																												break;	
																									  }else
																									  {
																												b_invalid_block = 0;
																									  }
																						   }	
																					}while(b_invalid_block == 1);
																					if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
																					{
																								Loop16:
																									if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,ReadFlashAddr,1))
																									{
																												bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																												g_bad_block_count++;
																												memset(ZeroBuf,0,sizeof(ZeroBuf));
																												tmp_addr.Page = ReadFlashAddr.Page + 512;
																												tmp_addr.Block = ReadFlashAddr.Block;
																												tmp_addr.Zone = ReadFlashAddr.Zone;
																												FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																												FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																												goto Loop16;	
																									}
																					}
																          if(!Create_or_Search)
																					{
																							FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																					}
																					close_nandflash_cs();enable_lcd_cs();
																					count = 0;
																					GUI_SetColor(GUI_BLACK);
																	        GUI_SetFont(&GUI_FontHZ16);
																					if(i%2==0)
																					{
																						
																									GUI_SetBkColor(0xF1F1F1);
																									
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																						
																									
																									
																					}
																					else
																					{

																									GUI_SetBkColor(0xd9d9da);
																								
																									GUI_DispStringAt(Control_save_data.date,70,88+i*38);
																									GUI_DispStringAt(Control_save_data.test_name,224,88+i*38);
																									GUI_DispStringAt(Control_save_data.result,355,88+i*38);
																									GUI_DispStringAt(Control_save_data.remarks,521,88+i*38);
																							
																									
																									
																					
																					}
															
														
													
														        }
											
																		GUI_SetFont(&GUI_FontHZ16);
																		for(;i<8;i++)
																		{
																					

																						if(i%2==0)
																						{
																										
																										GUI_SetBkColor(0xF1F1F1);
																										
																											
																										GUI_DispStringAt("                ",70,88+i*38);
																										GUI_DispStringAt("        ",224,88+i*38);
																										GUI_DispStringAt("        ",355,88+i*38);
																										GUI_DispStringAt("                ",521,88+i*38);
																										
																										
																		
																										
																						}
																						else
																						{
																										GUI_SetBkColor(0xd9d9da);
																								 
																										GUI_DispStringAt("                ",70,88+i*38);
																										GUI_DispStringAt("        ",224,88+i*38);
																										GUI_DispStringAt("        ",355,88+i*38);
																										GUI_DispStringAt("                ",521,88+i*38);
																										
																										
																	
																						
																						}

																		}	
															
																		 dele_flag= 0;
																		 dayingjiemianhuanyuan_flag=0;
													}
													GUI_SetBkColor(0xF1F1F1);
													GUI_SetColor(GUI_BLACK);
													GUI_SetFont(&GUI_FontHZ32x32);
													if(sum_result%8!=0)
													{
																	sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
																	strcat(str_page,t);
																	strcat(str_page,"/");
																	sprintf(t,"%d",sum_result/8+1);//
																	strcat(str_page,t);
																	GUI_DispStringAt(str_page,34,381);
													}
													else
													{
																	sprintf(t,"%d",sum_result/8-g_current_control_page/8+1);memset(str_page,0,8);//
																	strcat(str_page,t);
																	strcat(str_page,"/");
																	sprintf(t,"%d",sum_result/8);//
																	strcat(str_page,t);
																	GUI_DispStringAt(str_page,34,381);


													}
													//处于最后一页
													if(g_current_control_page/8<sum_result/8&&(g_current_control_page>8)&&(sum_result%8!=0))
													{
																 aaup99=99, bbup363=363, ccup50=50, ddup288=288;
																 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
													}
													else if(g_current_control_page/8==0&&sum_result>8)//第一页
													{
														     aaup99=99, bbup363=363, ccup50=50, ddup288=288;
																 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
																
													}
													else if(g_current_control_page/8==sum_result/8&&sum_result>8)//最后一页
													{
                                 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
																 aadown388=388, bbdown363=363, ccdown50=50, dddown288=288;
																 
														
													}
													else if(g_current_control_page==sum_result&&sum_result<=8)
													{
																 aaup99=0, bbup363=0, ccup50=0, ddup288=0;
																 aadown388=0, bbdown363=0, ccdown50=0, dddown288=0;
													}
													
										
											
												
											}
											else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 685, 346,73, 100) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//打印
											{
												        		 if(sum_result==0)
																		 {
																							 sum_result_zero_daying_flag=1;
																							 GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																							 sum_result_zero_daying_flag=0;
																							 goto PAGE_START;

																		 }
																		 else
																		 {
																								if(danxuan_flag||quanxuan_flag)
																								{
																													daying_flag=1;
																													GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																													daying_flag=0;
																													if(is_dele_flag==0)
																													{
																															 dele_flag = 1;
																															 goto PAGE2;
																													}
																										 
																												 if((danxuan_flag))
																												 {
																													 
																															 dayingjiemianhuanyuan_flag=1;
																															 if(Create_or_Search)		
																																{
																																				if(g_current_control_page>=8)
																																				{
																																							ReadFlashAddr.Zone = search_fit_result[g_current_control_page-8+sel] / (1024*32);
																																							ReadFlashAddr.Block =(search_fit_result[g_current_control_page-8+sel] - (g_current_control_page ) / (1024)*1024*32) / 32;
																																							ReadFlashAddr.Page = search_fit_result[g_current_control_page-8+sel] % 32;
																																				}
																																				else
																																				{
																																							ReadFlashAddr.Zone = search_fit_result[sel] / (1024*32);
																																							ReadFlashAddr.Block =(search_fit_result[sel] - (g_current_control_page ) / (1024)*1024*32) / 32;
																																							ReadFlashAddr.Page = search_fit_result[sel] % 32;

																																				}
																																		
																																}
																																else
																																{
																																				ReadFlashAddr.Zone =BaseFlashAddr.Zone+ (g_current_control_page-sel ) / (1024);
																																				ReadFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page-sel  - (g_current_control_page ) / (1024)*1024 -1);
																																				ReadFlashAddr.Page = 0;
																																	
																																}
																																memset((unsigned char*)&Control_save_data,0,sizeof(Control_save_data));
																																close_lcd_cs();enable_nandflash_cs();
																																do
																																{
																																			for(j=0;j<g_bad_block_count;j++)
																																			{
																																					if(bad_block[j] == (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																					{
																																							ReadFlashAddr.Block--;
																																							b_invalid_block = 1;
																																							break;	
																																					}else
																																					{
																																							b_invalid_block = 0;
																																					}
																																		 }	
																																}while(b_invalid_block == 1);
																																if((b_invalid_block == 0) && (ReadFlashAddr.Page == 0))
																																{
																																			Loop26:
																																				if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,ReadFlashAddr,1))
																																				{
																																							bad_block[g_bad_block_count] = (ReadFlashAddr.Page + (ReadFlashAddr.Block + (ReadFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
																																							g_bad_block_count++;
																																							memset(ZeroBuf,0,sizeof(ZeroBuf));
																																							tmp_addr.Page = ReadFlashAddr.Page + 512;
																																							tmp_addr.Block = ReadFlashAddr.Block;
																																							tmp_addr.Zone = ReadFlashAddr.Zone;
																																							FSMC_NAND_WriteSpareArea(ZeroBuf, tmp_addr, 8);
																																							FSMC_NAND_AddressDecrement(&ReadFlashAddr);
																																							goto Loop26;	
																																				}
																																}
																																close_nandflash_cs();enable_lcd_cs();

																																//初始化打印机		
																																USART_SendData(USART6,0x1B);
																																//while(USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET)
																																delay_nms(5);
																																USART_SendData(USART6,0x40);
																																//while(USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET)
																																delay_nms(5);
																																//uart6_write(print_init_cmd);
																																			
																																//处理打印机缺纸异常		
																																rcv_print_count = 0;
																																USART_SendData(USART6,0x1D);  //0x1B
																																delay_nms(5);
																																USART_SendData(USART6,0x72);	//0x1B
																																delay_nms(5);
																																USART_SendData(USART6,0x01);	//0x1B

																																//uart6_write(print_status_cmd);
																																delay_nms(100);
																																if(rcv_print_count == 1)
																																{
																																	if((Print_rcv_buf[0] == 0x0C))
																																	{
																																			print_outofpage_flag=1;
																																			GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																																			print_outofpage_flag=0;
												// 																						if(language_choice)
												// 																							GUI_ExecDialogBox(_aPrinterWithoutPageDlg, GUI_COUNTOF(_aPrinterWithoutPageDlg), &_cbStardard_dlg, WM_HBKWIN, 0, 0);
												// 																						else
												// 																							GUI_ExecDialogBox(_aPrinterWithoutPageEnDlg, GUI_COUNTOF(_aPrinterWithoutPageEnDlg), &_cbStardard_dlg, WM_HBKWIN, 0, 0);
																															
																																		//break;
																																    dele_flag = 1;
																																		goto PAGE2;

																																	}
																																	if((Print_rcv_buf[0] == 0x08))
																																	{
																																			print_error_flag=1;
																																			GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																																			print_error_flag=0;
												// 																						if(language_choice)
												// 																							GUI_ExecDialogBox(_aPrinterErrorDlg, GUI_COUNTOF(_aPrinterErrorDlg), &_cbStardard_dlg, WM_HBKWIN, 0, 0);
												// 																						else
												// 																							GUI_ExecDialogBox(_aPrinterErrorEnDlg, GUI_COUNTOF(_aPrinterErrorEnDlg), &_cbStardard_dlg, WM_HBKWIN, 0, 0);
																																	
																																    dele_flag = 1;
																																		goto PAGE2;

																																	}
																																	rcv_print_count = 0;
																																	memset(Print_rcv_buf,0,sizeof(Print_rcv_buf));
																																}																				
																																
																																	if(1)
																																	{
																																		uart6_write("          质控报告单        \r\n");
																																		delay_nms(60);
																																		uart6_write("==============================\r\n");
																																		delay_nms(60);
																															
																																		uart6_write("质控日期:  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.date);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("质控类型:  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.test_name);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("质控结果:  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.result);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("备注: \r\n");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.remarks);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("==============================\r\n");
																																	}
																																	else
																																	{
																																		uart6_write("       inspection report    \r\n");
																																		delay_nms(60);
																																		uart6_write("==============================\r\n");
																																		delay_nms(60);
																																		uart6_write("Patient SN  :  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.date);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("Patient Name:  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.test_name);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("Test Project:  ");
																																		delay_nms(60);
																																		uart6_write(Control_save_data.result);
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		delay_nms(60);
																																		uart6_write("Test Result : \r\n");
																																		delay_nms(60);
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("\r\n");
																																		uart6_write("==============================\r\n");
																																	}	
																															
																																
																													
																												
																												 }	
																												 else if(quanxuan_flag)
																												 {
																																 dayingjiemianhuanyuan_flag=1;
																												 }
																												 dele_flag = 1;
																												 goto PAGE2;
																							 }
																							 if(danxuan_flag==0&&quanxuan_flag==0)
																							 {
																											
																														 GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																														 dele_flag = 1;
																														 goto PAGE2;
																							 }
																			
																		}



                      }
											else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 685, 232+40,73, 100) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))//删除
											{

										             if(Create_or_Search) 
																 {
                                      control_del_flag=1;
																	    GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																	    control_del_flag=0;
																	    dele_flag = 1;
																			goto PAGE2;

                                 }
																 if(sum_result==0)//无数据
																 {
																					 sum_result_zero_del_flag=1;
																					 GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																					 sum_result_zero_del_flag=0;
																	 	       dele_flag = 1;
																						goto PAGE2;
																			

																 }
																 else//有数据
																 {
																						if(sel!=9&&(danxuan_flag||quanxuan_flag))
																						{
																										GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																										if(is_dele_flag==0)
																										{
																												 dele_flag = 1;
																												 goto PAGE2;
																										}
																										OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
																										OSIntNesting++;
																										if(danxuan_flag==1)
																										{
																															
																															FSMC_NAND_AddressDecrement(&WriteReadAddr_control);
																															//FSMC_NAND_AddressDecrement(&WriteReadAddr_control);
																													
																															dele_flag= 1;
																															CurFlashAddr.Zone =BaseFlashAddr.Zone+  g_current_control_page / (1024);
																															CurFlashAddr.Block =BaseFlashAddr.Block+ g_current_control_page - (g_current_control_page+quanxuandel_count) / (1024) * 1024 -(sel+1);
																															CurFlashAddr.Page = 0;
																														
										
																															 close_lcd_cs();enable_nandflash_cs();
																															 for(i = 0; i < sum_result-(g_current_control_page-(sel+1))-1; i ++)
																															 {
																																				do
																																				{
																																							for(j=0;j<g_bad_block_count;j++)
																																							{
																																										if(bad_block[j] == (CurFlashAddr.Page + (CurFlashAddr.Block + (CurFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																										{
																																														CurFlashAddr.Block++;
																																														b_invalid_block = 1;
																																														break;	
																																										}else
																																										{
																																														b_invalid_block = 0;
																																										}
																																							}	
																																				}while(b_invalid_block == 1);
																																				if((b_invalid_block == 0) && (CurFlashAddr.Page == 0))
																																				{
																																								DelFlashAddr.Zone = CurFlashAddr.Zone;
																																								DelFlashAddr.Block =  CurFlashAddr.Block;
																																								DelFlashAddr.Page = 0;
																																								FSMC_NAND_EraseBlock(DelFlashAddr);
																																				}
																																				if(sum_result-(g_current_control_page-(sel+1))-1 - i > 0)
																																				{
																																							FSMC_NAND_AddressIncrement(&CurFlashAddr);
																																							do
																																							{
																																										for(j=0;j<g_bad_block_count;j++)
																																										{
																																													if(bad_block[j] == (CurFlashAddr.Page + (CurFlashAddr.Block + (CurFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																													{
																																																CurFlashAddr.Block ++;
																																																b_invalid_block = 1;
																																																break;	
																																													}else
																																													{
																																																b_invalid_block = 0;
																																													}
																																										}	
																																							}while(b_invalid_block == 1);
																																							if((b_invalid_block == 0) && (CurFlashAddr.Page == 0))
																																							{
																																										 FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,CurFlashAddr,1);
																																							}
																																							FSMC_NAND_WriteSmallPage((unsigned char *)&Control_save_data,DelFlashAddr,1);
																																						//	FSMC_NAND_AddressDecrement(&CurFlashAddr);
																																				}
																															}
																															if(g_current_control_page >0)g_current_control_page  --;									      
																															if(sum_result>0)
																															{
																																sum_result --;
																									
																															}
																															close_nandflash_cs();enable_lcd_cs();
																															if(g_current_control_page/8==0)
																															{
																																		if(sel>(int)(sum_result%8-1))
																																		{
																																			 if(sel%2)
																																			 {
																																						 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan1 ,  31, 86+sel*36);
																																						 danxuan_control[sel]=0;
																																			 }
																																			 else
																																			 {
																																						 GUI_DrawBitmap(&bmlishiquanxuanhuanyuan0 ,  31, 86+sel*36);
																																						 danxuan_control[sel]=0;

																																			 }
																																			 sel=9;
																																		}
																															}
																													
																															
																									}
																									else if(quanxuan_flag==1)
																									{
																															dele_flag= 1;
																															
																															if(g_current_control_page/8>0)
																															{
																																		quanxuandel_count=8;
																															}
																															else if(g_current_control_page/8==0)
																															{
																																		quanxuandel_count=sum_result%8;
																															}
																															for(i=0;i<quanxuandel_count;i++)
																															{ 
																																		FSMC_NAND_AddressDecrement(&WriteReadAddr_control);
																															}
																															if(g_current_control_page >0)g_current_control_page  -=quanxuandel_count;									      
																												
																															CurFlashAddr.Zone =BaseFlashAddr.Zone+  (g_current_control_page+quanxuandel_count) / (1024);
																															CurFlashAddr.Block =BaseFlashAddr.Block+ (g_current_control_page+quanxuandel_count) - (g_current_control_page+quanxuandel_count) / (1024) * 1024;
																															CurFlashAddr.Page = 0;
																															
																													
																															DelFlashAddr.Zone =BaseFlashAddr.Zone+  g_current_control_page / (1024);
																															DelFlashAddr.Block =BaseFlashAddr.Block + g_current_control_page - g_current_control_page / (1024)*1024;
																															DelFlashAddr.Page = 0;
																															close_lcd_cs();enable_nandflash_cs();	
																															if((g_current_control_page+quanxuandel_count)==sum_result)
																															{
                                                                      for(i=0;i<quanxuandel_count;i++)
																																      {
                                                                            
																																							do
																																							{
																																										for(j=0;j<g_bad_block_count;j++)
																																										{
																																													if(bad_block[j] == (DelFlashAddr.Page + (DelFlashAddr.Block + (DelFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																													{
																																																	DelFlashAddr.Block++;
																																																	b_invalid_block = 1;
																																																	break;	
																																													}else
																																													{
																																																	b_invalid_block = 0;
																																													}
																																										}	
																																							}while(b_invalid_block == 1);
																																							if((b_invalid_block == 0) && (DelFlashAddr.Page == 0))
																																							{
																																											
																																											FSMC_NAND_EraseBlock(DelFlashAddr);
																																											
																																							}
																																							FSMC_NAND_AddressIncrement(&DelFlashAddr);


                                                                      }




                                                              }
																															else
																															{
																																			for(i = 0; i <sum_result-(g_current_control_page+quanxuandel_count); i ++)
																																			{
																																				do
																																				{
																																							for(j=0;j<g_bad_block_count;j++)
																																							{
																																										if(bad_block[j] == (DelFlashAddr.Page + (DelFlashAddr.Block + (DelFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																										{
																																														DelFlashAddr.Block++;
																																														b_invalid_block = 1;
																																														break;	
																																										}else
																																										{
																																														b_invalid_block = 0;
																																										}
																																							}	
																																				}while(b_invalid_block == 1);
																																				if((b_invalid_block == 0) && (DelFlashAddr.Page == 0))
																																				{
																																								
																																								FSMC_NAND_EraseBlock(DelFlashAddr);
																																								
																																				}
																																				if(sum_result-(g_current_control_page+quanxuandel_count) - i >0)
																																				{
																																						
																																							 
																																							
																																							do
																																							{
																																										for(j=0;j<g_bad_block_count;j++)
																																										{
																																													if(bad_block[j] == (CurFlashAddr.Page + (CurFlashAddr.Block + (CurFlashAddr.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
																																													{
																																																CurFlashAddr.Block ++;
																																																b_invalid_block = 1;
																																																break;	
																																													}else
																																													{
																																																b_invalid_block = 0;
																																													}
																																										}	
																																							}while(b_invalid_block == 1);
																																							if((b_invalid_block == 0) && (CurFlashAddr.Page == 0))
																																							{
																																										 FSMC_NAND_ReadSmallPage((unsigned char *)&Control_save_data,CurFlashAddr,1);
																																							}
																																							FSMC_NAND_WriteSmallPage((unsigned char *)&Control_save_data,DelFlashAddr,1);
																																							FSMC_NAND_AddressIncrement(&CurFlashAddr);
																																							FSMC_NAND_AddressIncrement(&DelFlashAddr);
																																				}
																																			}	
																														  }
																															close_nandflash_cs();enable_lcd_cs();
																															if(sum_result>0)
																															{
																																	 sum_result -=quanxuandel_count;
																																	 //g_result_count_control-=quanxuandel_count;
																															}
																													 
																															
																															

																								 }
																								 if(!Create_or_Search)
																								{
																										 g_result_count_control=sum_result;
																										 
																								}
																								if(g_current_control_page==0)
																								{
                                                      g_current_control_page+=8;
                                                }
																						    OS_EXIT_CRITICAL();
		                                            OSIntExit();        //触发任务切换软中断
																								if(!Create_or_Search)		
																								{
																												OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
																												OSIntNesting++;
																												I2C_Configuration();
																												AT24C512_WritePage(ADDR_24C512,0x54,(unsigned char *)&WriteReadAddr_control,6);
																												AT24C512_WritePage(ADDR_24C512,0x5a,(unsigned char *)&g_result_count_control, 4);
																												AT24C512_WritePage(ADDR_24C512,0x5e,(unsigned char *)&g_current_control_page, 4);

																												OS_EXIT_CRITICAL();
																												OSIntExit();        //触发任务切换软中断
																								}
																								dele_flag = 1;
																									goto PAGE2;
																					}
																					else if(danxuan_flag==0&&quanxuan_flag==0)
																					{
																									
																												 GUI_ExecDialogBox(_aExcuteDELEDlg, GUI_COUNTOF(_aExcuteDELEDlg), &_cbCallback_dlg,0, 0, 0); //WM_HBKWIN
																												 dele_flag = 1;
																												 goto PAGE2;
																										
																										

																					}
													   }

											}
											Prompt_column_update();
											time_display(&time_flag);
											if(test_choice != 2)
											{
														
													  
													  if(Create_or_Search)
														{
																
															   g_current_control_page=g_current_control_page_mid;
														}
														else
														{
																 g_result_count_control=sum_result;
															   
														}
														break;
											}
		
	     }
	     while(1);

}
