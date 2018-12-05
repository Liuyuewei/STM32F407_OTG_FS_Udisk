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
#include "GPIO.h"
#include "fsmc_nand.h"
#include "AT24C512.h"



// unsigned char MCU_Para_ch1_data_h = 0, MCU_Para_ch2_data_h = 0, MCU_Para_ch3_data_h = 0, MCU_Para_ch4_data_h = 0,
//               MCU_Para_ch1_data_l = 0, MCU_Para_ch2_data_l = 0, MCU_Para_ch3_data_l = 0, MCU_Para_ch4_data_l = 0;

unsigned char MCU_PARA_DATA[MCU_PARA_LENGTH];





#define TEST_HISTORY_END     32768

const NAND_ADDRESS TEST_HISTORY_START 
                 = {0,0,0};



//91 BYTE

_Save_Test_Data_Struct Test_data;
_Save_Test_Data_Struct Read_save_data;

void Fresh_Test_dispaly(unsigned char inter);
void Test_Progbar_set(unsigned char Progbar_data);


#define TEST_timeout   1000
#define Display_time   300          //提示信息刷新间隔时间  单位为ms

//用于切换四个主界面
unsigned char test_choice = 0;
//test_choice = 0   :     测试界面
//test_choice = 1   :     设置界面
//test_choice = 2   :     质控界面
//test_choice = 3   :     历史界面



unsigned char FR1000_ERROR_FLAG = 0; 
//FR1000_ERROR_FLAG = 0:正常状态         
//FR1000_ERROR_FLAG = 1:从机发送过来的数据出错   
//FR1000_ERROR_FLAG = 2:等待从机发来插卡完成超时
//FR1000_ERROR_FLAG = 3:等待从机扫描一维码超时
//FR1000_ERROR_FLAG = 4:等待从机插入采血管超时
//FR1000_ERROR_FLAG = 5:等待吸血完成超时
//FR1000_ERROR_FLAG = 6:等待压血完成超时
//FR1000_ERROR_FLAG = 7:等待采样完成超时
//FR1000_ERROR_FLAG = 8:测量过程中点击了结束测量按钮
//FR1000_ERROR_FLAG = 9:标曲未扫描 请扫描标曲
//FR1000_ERROR_FLAG = 10:测量过程中拔出试剂卡
//FR1000_ERROR_FLAG = 11:测量过程中拔出采血管
//FR1000_ERROR_FLAG = 12:从机调节测量装置超时
//FR1000_ERROR_FLAG = 13:没有测量记录 无法打印
//FR1000_ERROR_FLAG = 14:没有质控记录 无法打印


//记录指令发出到接收到回复的时间 测试过程中若为0 则代表超时 
uint32_t Test_time = 0;                                 

//从机测试流程持续的时间 根据此时间计算和刷新进度条进度 
//以尽量在测试完成时显示100%
uint32_t Test_pro_time = 0;

const unsigned char Test_TYPE_P2Y12[16] = "P2Y12";
const unsigned char Test_TYPE_ASPIRIN[16] = "Aspirin";
const unsigned char Test_TYPE_GP2B_3A[16] = "GPIIb/IIIa";

const unsigned char Test_UNIT_P2Y12[4] = "PRU";
const unsigned char Test_UNIT_ASPIRIN[4] = "ARU";
const unsigned char Test_UNIT_GP2B_3A[4] = "PAU";

unsigned char Test_patient_num[20] = {0};                   //保存用户编号
unsigned char Test_date[20] = {0};                          //保存测试时间
unsigned char Test_patient_name[20] = {0};                  //保存患者姓名
unsigned char Test_type[16] = {0};                          //保存测试类型
unsigned char Test_Unit[4] = {0};                           //保存测试结果1的单位 PRU/ARU/PAU
//显示用字符串 前端加单位(PRU/ARU/PAU)
unsigned char Test_Dis_result0_ch[20] = {0};                    //显示用测试结果0的字符串
unsigned char Test_Dis_result1_ch[20] = {0};                    //显示用测试结果1的字符串
unsigned char Test_Dis_result2_ch[20] = {0};                    //显示用测试结果2的字符串


//保存结果用字符串
unsigned char Test_Save_Result0_ch[4] = {0};                       //保存测试结果0的字符串
unsigned char Test_Save_Result1_ch[4] = {0};                       //保存测试结果1的字符串
unsigned char Test_Save_Result2_ch[4] = {0};                       //保存测试结果2的字符串
//3个测试结果的数值 上面3个字符串由3个数值得到的
unsigned int Test_Result_data0, Test_Result_data1, Test_Result_data2;

unsigned char Test_finish_flag = 0;                         //进行了一次测试 需要刷新下方信息显示区域
unsigned char test_date_buf[24];                            //保存测试时间的中间变量
//是否点击了确认关机按钮 0：弹出关机界面 点击了取消 1：点击了确认
bool power_flag=0;

//保存从机传递过来的计算结果
unsigned char ch1_flag = 0, ch2_flag = 0, ch3_flag = 0, ch4_flag = 0;
unsigned int  max_data_value[4] = {0}, max_data_position[4] = {0}, min_data_value[4] = {0}, min_data_position[4] = {0};
unsigned char result_value[4] = {0};


extern bool time_flag;
extern void  time_display(bool *time_flag);

unsigned char Test_type_flag = 0;  
//1:Aspirin  2:P2Y12   3:GPIIb/IIIa
unsigned char Test_inital_flag = 1;
//0:需要进行测试界面的屏幕刷新 1：不需要进行刷新

void MCU_ADJUST_DAC(void);
void Save_Test_History(_Save_Test_Data_Struct Save_data,NAND_ADDRESS* Address);
void Read_Test_History(_Save_Test_Data_Struct *Read_data,NAND_ADDRESS* Address);
void Delete_Test_History(NAND_ADDRESS* Address);
NAND_ADDRESS TEST_HIS_ADDR ;
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
const unsigned char user_name[40][40]=
{
      "叶叶天",
	    "完日日",
	    "张三三",
	    "王木木",
	    "叶叶叶",
	    "完日水",
	    "张三水",
	    "王木到",
	    "张三是",
	    "是水是",
	 
	  
	    "王木木",
	    "叶叶叶",
	    "完日水",
	    "张三水",
	     "叶叶天",
	    "完日日",
	    "张三三",
	    "王木到",
	    "张三是",
	    "是水是",
			
		  "叶叶天",
	    "完日日",
	    "张三三",
	    "王木木",
			"王木到",
	    "张三是",
	    "是水是",
	    "叶叶叶",
	    "完日水",
	    "张三水",
			
			"叶叶天",
	    "完日日",
	    "张三三",
	    "王木木",
			"王木到",
	    "张三是",
	    "是水是",
	    "叶叶叶",
	    "完日水",
	    "张三水",
	    
};
const unsigned char user_id[40][30]=
{
      "122222222221122",
	    "122225522221122",
	    "12222462221122",
	    "122222222221122",
	    "122225522221122",
	    "12222462221122",
	    "122222222221122",
	    "122225522221122",
	    "12222462221122",
	    "122222227221122",
	    "122225572221122",
	    "12222462521122",
      "122222222221122",
	    "122225522221122",
	    "12222462221122",
	
	    "122222222621122",
	    "122225522261122",
	    "12222462221122",
	    "122222222621122",
	    "122225526221122",
	    "12222466221122",
	    "12222222221122",
	    "122225522221122",
	    "12222462721122",
	    "122222227221122",
	    "122225572221122",
	    "12222462521122",
      "122222222221122",
	    "122225522221122",
	    "12222462221122",
			
			"12222466221122",
	    "12222222221122",
	    "122225522221122",
	    "12222462721122",
	    "122222227221122",
	    "122225572221122",
	    "12222462521122",
      "122222222221122",
	    "122225522221122",
	    "12222462221122"
};
const unsigned char user_type[40][16]=
{
      "Asplin",
	    "Asplin",
	    "Asplin",
	    "Asplin",
	    "Asplin",
	    "Asplin",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
			
			"GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib ",
	    "Asplin",
	    "P2Y12 ",
	    "GPiib "
};

const unsigned char user_date[40][24]=
{
      "2015/12/11 23:00",
	    "2015/12/11 23:01",
	    "2015/12/11 23:02",
	    "2015/12/11 23:03",
	    "2015/12/11 23:04",
	    "2015/12/11 23:05",
	    "2015/12/11 23:06",
	    "2015/12/11 23:07",
	    "2015/12/11 23:08",
	    "2015/12/11 23:09",
	    "2015/12/11 23:10",
	    "2015/12/11 23:11",
      "2015/12/11 23:12",
	    "2015/12/11 23:13",
	    "2015/12/11 23:14",
	
	    "2015/12/11 23:15",
	    "2015/12/11 23:16",
	    "2015/12/11 23:17",
	    "2015/12/11 23:18",
	    "2015/12/11 23:19",
	    "2015/12/11 23:20",
	    "2015/12/11 23:21",
	    "2015/12/11 23:22",
	    "2015/12/11 23:23",
	    "2015/12/11 23:24",
	    "2015/12/11 23:25",
	    "2015/12/11 23:26",
      "2015/12/11 23:27",
	    "2015/12/11 23:28",
	    "2015/12/11 23:29",
			
		 "2015/12/11 23:30",
		 "2015/12/11 23:31",
		 "2015/12/11 23:32",
		 "2015/12/11 23:33",
		 "2015/12/11 23:34",
		 "2015/12/11 23:35",
		 "2015/12/11 23:36",
		 "2015/12/11 23:37",
		 "2015/12/11 23:38",
		 "2015/12/11 23:39"
			
};


const unsigned char user_Test_Unit[40][4]=
{
      "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
			

	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU",
	    "PRU"
};

const unsigned char user_Test_Save_Result0_ch[40][4]=
{
    
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
			
			"332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332"
};
 unsigned char user_Test_Save_Result1_ch[40][4]=
{
    
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
			
			"332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332"
};
 unsigned char user_Test_Save_Result2_ch[40][4]=
{
    
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
			
			"332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332",
	    "332"
};
const unsigned char name_remarks[30][20]=
{
    
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "当天质控",
	    "当天质控",
	    "当天质控",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "当天质控",
	    "当天质控",
	    "当天质控",
	
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "当天质控",
	    "当天质控",
	    "当天质控",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "注意基线",
	    "当天质控",
	    "当天质控",
	    "当天质控",
			

};
extern unsigned long g_result_count;
extern NAND_ADDRESS WriteReadAddr;
/*测量流程设计*/
void Create_Test(void) {
  
	
	static int i = 0, j = 0;
	OS_CPU_SR  cpu_sr;
	
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
	
	//复位标志位 进行测试界面刷新 包括提示信息 图片等
	Test_inital_flag = 1;
	
	//未进行测试操作 则清空保存测试时间缓存
	if(Test_finish_flag == 1)
	{
	  for(i = 0;i<24;i++)
	  {
		  test_date_buf[i] = 0;
  	}
  }
			//显示主界面
		if(Chinese_display == 1)
		{
			bmp_dispflash(ADRESS_TEST_RIGHT1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}
		else
		{
			bmp_dispflash(ADRESS_EN_TEST_RIGHT1,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}			
		
		SERIAL_ROM_Font_mode();
		Graphic_Mode();			
		if(Chinese_display == 1)
		{
			bmp_dispflash(ADRESS_TEST_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
		}
		else
		{
			bmp_dispflash(ADRESS_EN_TEST_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
		}
		SERIAL_ROM_Font_mode();
		Graphic_Mode();
	do
	{
		
		time_display(&time_flag);
	  if(Test_inital_flag == 0)
		{
			OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		  OSIntNesting++;

			if(Chinese_display == 0)
			{
				GUI_SetFont(&GUI_FontHZ16);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("患者姓名:",42,335);	
				GUI_DispStringAt("患者编号:",242,335);
				GUI_DispStringAt("测试时间:",42,375);	
				GUI_DispStringAt("测试项目:",242,375);	
				GUI_DispStringAt(name_chinese_test,120,335);
				
				GUI_SetFont(&GUI_Font16_ASCII);		   //编号
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt(number_test,320,335);
				
				GUI_SetFont(&GUI_FontHZ24);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("测试结果:",442,335);
			}
			else
			{
				GUI_SetFont(&GUI_Font16B_ASCII);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("Patient Name:",42,335);	
				GUI_DispStringAt("Patient Number:",242,335);
				GUI_DispStringAt("Test Time:",42,375);	
				GUI_DispStringAt("Test Items:",242,375);
				GUI_DispStringAt(number_test,350,335);	
				
				GUI_SetFont(&GUI_FontHZ16);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt(name_chinese_test,135,335);
				
				GUI_SetFont(&GUI_Font24_ASCII);
				GUI_SetBkColor(BACK_COLOUR_24);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("Test Result:",442,335);
			}
			
//       if(Test_finish_flag == 1)
// 			{
// 			  for(i=0;i<40;i++)
// 				{
					
					
// 						  memset( &number_test,0,sizeof(number_test));
// 					    memset( &name_chinese_test,0,sizeof(name_chinese_test));
// 					    memset( &Test_type,0,sizeof(Test_type));
// 					    memset( &test_date_buf,0,sizeof(test_date_buf));
// 					    memset( &Test_Save_Result0_ch,0,sizeof(Test_Save_Result0_ch));
// 					    memset( &Test_Save_Result1_ch,0,sizeof(Test_Save_Result1_ch));
// 					    memset( &Test_Save_Result2_ch,0,sizeof(Test_Save_Result2_ch));
// 					    memset( &Test_Unit,0,sizeof(Test_Unit));
// 							memcpy(number_test,user_id[i],strlen(user_id[i]));
// 							memcpy(name_chinese_test,user_name[i],strlen(user_name[i]));
// 					    memcpy(Test_type,user_type[i],strlen(user_type[i]));
// 							memcpy(test_date_buf,user_date[i],strlen(user_date[i]));
// 							memcpy(Test_Save_Result0_ch,user_Test_Save_Result0_ch[i],strlen(user_Test_Save_Result0_ch[i]));
// 							memcpy(Test_Save_Result1_ch,user_Test_Save_Result1_ch[i],strlen(user_Test_Save_Result1_ch[i]));
// 					    memcpy(Test_Save_Result2_ch,user_Test_Save_Result2_ch[i],strlen(user_Test_Save_Result2_ch[i]));
// 							memcpy(Test_Unit,user_Test_Unit[i],strlen(user_Test_Unit[i]));//unit
							if(Chinese_display == 1)
							{
								GUI_SetFont(&GUI_Font16_ASCII);		       //时间
							}
							else
							{
								GUI_SetFont(&GUI_Font16B_ASCII);
							}
							GUI_SetBkColor(BACK_COLOUR_24);
							GUI_SetColor(GUI_BLACK);
							GUI_DispStringAt(test_date_buf,120,375);
							
							GUI_SetFont(&GUI_Font24_ASCII);		   //结果0
							GUI_SetBkColor(BACK_COLOUR_24);
							GUI_SetColor(GUI_BLACK);
							GUI_DispStringAt(Test_Dis_result0_ch,556,335);
							
							//结果1
							switch(Test_type_flag)
							{
								case 1:
								{
									break;
								}
								case 2:
								{
									if(Chinese_display == 1)
									{
										GUI_SetFont(&GUI_FontHZ24);
										GUI_SetBkColor(BACK_COLOUR_24);
										GUI_SetColor(GUI_BLACK);
										GUI_DispStringAt("基线:",442,375);
									}
									else
									{
										GUI_SetFont(&GUI_Font24_ASCII);
										GUI_SetBkColor(BACK_COLOUR_24);
										GUI_SetColor(GUI_BLACK);
										GUI_DispStringAt("BaseLine:",442,375);
									}
									GUI_SetFont(&GUI_Font24_ASCII);
									GUI_SetBkColor(BACK_COLOUR_24);
									GUI_SetColor(GUI_BLACK);
									GUI_DispStringAt(Test_Dis_result1_ch,556,375);
									break;
								}
								case 3:
								{
									break;
								}
								default:
								{
									break;
								}
								
							}
							
			// 				GUI_SetFont(&GUI_FontHZ16);		  
			// 				GUI_SetBkColor(BACK_COLOUR_24);
			// 				GUI_SetColor(GUI_BLACK);
			// 				GUI_DispStringAt("最大值",200,40);
			// 				GUI_DispStringAt("位置",300,40);
			// 				GUI_DispStringAt("最小值",400,40);
			// 				GUI_DispStringAt("位置",500,40);
			// 				GUI_DispStringAt("比值",600,40);
			// 				
			// 				GUI_SetFont(&GUI_Font16_ASCII);		      
			// 		  	GUI_SetBkColor(BACK_COLOUR_24);
			// 			  GUI_SetColor(GUI_BLACK);
			// 				GUI_DispDecAt(max_data_value[0],200,58,4);
			// 				GUI_DispDecAt(max_data_position[0],300,58,4);
			// 				GUI_DispDecAt(min_data_value[0],400,58,4);
			// 				GUI_DispDecAt(min_data_position[0],500,58,4);
			// 				GUI_DispDecAt(result_value[0],600,58,3);
			// 				
			// 				GUI_DispDecAt(max_data_value[1],200,76,4);
			// 				GUI_DispDecAt(max_data_position[1],300,76,4);
			// 				GUI_DispDecAt(min_data_value[1],400,76,4);
			// 				GUI_DispDecAt(min_data_position[1],500,76,4);
			// 				GUI_DispDecAt(result_value[1],600,76,3);
			// 				
			// 				GUI_DispDecAt(max_data_value[2],200,94,4);
			// 				GUI_DispDecAt(max_data_position[2],300,94,4);
			// 				GUI_DispDecAt(min_data_value[2],400,94,4);
			// 				GUI_DispDecAt(min_data_position[2],500,94,4);
			// 				GUI_DispDecAt(result_value[2],600,94,3);
			// 				
			// 				GUI_DispDecAt(max_data_value[3],200,112,4);
			// 				GUI_DispDecAt(max_data_position[3],300,112,4);
			// 				GUI_DispDecAt(min_data_value[3],400,112,4);
			// 				GUI_DispDecAt(min_data_position[3],500,112,4);
			// 				GUI_DispDecAt(result_value[3],600,112,3);
							
			// 				memcpy(Test_type,Test_TYPE_P2Y12,sizeof(Test_TYPE_P2Y12));
							if(Chinese_display == 1)
							{
								GUI_SetFont(&GUI_Font16_ASCII);		   //项目
							}
							else
							{
								GUI_SetFont(&GUI_Font16B_ASCII);
							}
							GUI_SetBkColor(BACK_COLOUR_24);
							GUI_SetColor(GUI_BLACK);
							GUI_DispStringAt(Test_type,320,375);
							
							//清空结构体
							memset( &Test_data,0,sizeof(Test_data));
							memcpy( Test_data.Patient_id,     number_test,              strlen(number_test) );
							memcpy( Test_data.Patient_name,   name_chinese_test,        strlen(name_chinese_test) );
							memcpy( Test_data.Item,           Test_type,                strlen(Test_type) );
							memcpy( Test_data.Time,           test_date_buf,            strlen(test_date_buf) );
							memcpy( Test_data.Result_0,       Test_Save_Result0_ch,     strlen(Test_Save_Result0_ch) );
							memcpy( Test_data.Result_1,       Test_Save_Result1_ch,     strlen(Test_Save_Result1_ch) );
							memcpy( Test_data.Result_2,       Test_Save_Result2_ch,     strlen(Test_Save_Result2_ch) );
							memcpy( Test_data.Unit,           Test_Unit,                strlen(Test_Unit) );
							Test_data.Write_flag = 0x00;                                                           //此flash页已写入正常数据
							Test_data.Delete_flag = 0xFF;  

							//读取最新的flash写入地址 
  						I2C_Configuration();
              AT24C512_ReadPage(ADDR_24C512,0x34,(unsigned char *)&TEST_HIS_ADDR, 6);
							Save_Test_History(Test_data,&TEST_HIS_ADDR);
							//此flash页数据未删除
						  I2C_Configuration();
							AT24C512_ReadPage(ADDR_24C512,0x3a,(unsigned char *)&g_result_count, 4);
							g_result_count++;
							I2C_Configuration();
							AT24C512_WritePage(ADDR_24C512,0x34,(unsigned char *)&TEST_HIS_ADDR,6);
							AT24C512_WritePage(ADDR_24C512,0x3a,(unsigned char *)&g_result_count, 4);
							AT24C512_WritePage(ADDR_24C512,0x3e,(unsigned char *)&g_result_count, 4);
// 							memcpy((char*)&WriteReadAddr,(char*)&TEST_HIS_ADDR,sizeof(TEST_HIS_ADDR));
// 							TEST_HIS_ADDR.Zone = 0x00;
// 							TEST_HIS_ADDR.Block = 0x00;
// 							TEST_HIS_ADDR.Page = 0x00;
// 							Read_Test_History(&Read_save_data,&TEST_HIS_ADDR);
// 							Delete_Test_History(&TEST_HIS_ADDR);
// 							Read_Test_History(&Read_save_data,&TEST_HIS_ADDR);
//				}
			//}
			Test_inital_flag = 1;
			OS_EXIT_CRITICAL();
		  OSIntExit();        //触发任务切换软中断
		}
		
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
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			Test_finish_flag = 0;
			//获取系统时间
      RTC_ClearFlag(RTC_FLAG_ALRAF);	
		  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		  (void)RTC->DR;
		  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
			
			sprintf(test_date_buf," 20%0.2d/%0.2d/%0.2d  %0.2d:%0.2d", 
			                    RTC_DateStructure.RTC_Year,
													      RTC_DateStructure.RTC_Month, 
																      RTC_DateStructure.RTC_Date,
																			        RTC_TimeStructure.RTC_Hours, 
																							      RTC_TimeStructure.RTC_Minutes);
			
			OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		  OSIntNesting++;
			
			if(Chinese_display == 1)
			{
			  GUI_SetFont(&GUI_Font16_ASCII);		       //时间
			}
			else
			{
				GUI_SetFont(&GUI_Font16B_ASCII);		       //时间
			}
			GUI_SetBkColor(BACK_COLOUR_24);
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt(test_date_buf,120,375);
			
			if(Chinese_display == 1)
			{
				bmp_dispflash(ADRESS_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}
			else
			{
				bmp_dispflash(ADRESS_EN_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}
	    SERIAL_ROM_Font_mode();
	    Graphic_Mode();
			OS_EXIT_CRITICAL();
		  OSIntExit();        //触发任务切换软中断
			
			Work_type = 0;
			TEST_begin();                             //调用测试函数
		  Work_type = 2;
		
			Test_inital_flag = 0;
			break;
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,50, 100, 115) && (g_ucTouch == 1)&&(ts_event.Key_Sta==Key_Up))   //点击病人信息按钮
		{
			
		  OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		  OSIntNesting++;
	    ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			if(Chinese_display == 1)
			{
				bmp_dispflash(ADRESS_TEST_RIGHT3,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}
			else
			{
				bmp_dispflash(ADRESS_EN_TEST_RIGHT3,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
			}	
			SERIAL_ROM_Font_mode();
			Graphic_Mode();
			TEST_type_information();
			Test_inital_flag = 0;
		  OS_EXIT_CRITICAL();
		  OSIntExit();        //触发任务切换软中?
		  break;
		
		}
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 680,300, 100, 115) && (g_ucTouch == 1))   //点击打印按钮
		{
			
			
			MCU_ADJUST_DAC();
			
			if(Test_finish_flag == 1)
			{
				Print_inital();
				Print_test();
				delay_nms(60);
				
				
				if((rcv_print_count != 0) && (Print_rcv_buf[0] == PRINT_ERROR))
				{
				    FR1000_ERROR_FLAG = 15;
				    Error_popup(FR1000_ERROR_FLAG);
				    Test_inital_flag = 0;
				}
				else if((rcv_print_count != 0) && (Print_rcv_buf[0] == PRINT_PAPER_LACK))
				{
				    FR1000_ERROR_FLAG = 16;
				    Error_popup(FR1000_ERROR_FLAG);
				    Test_inital_flag = 0;
				}
				else
				{
					usart6_write_string("          检验报告单        \r\n");
					delay_nms(60);
					usart6_write_string("==============================\r\n");
					delay_nms(60);
					usart6_write_string("患者姓名:  ");
					delay_nms(60);
					usart6_write_string(name_chinese_test);
					delay_nms(60);
					usart6_write_string("\r\n");
					delay_nms(60);
					usart6_write_string("患者编号:  ");
					delay_nms(60);
					usart6_write_string(number_test);
					delay_nms(60);
					usart6_write_string("\r\n");
					delay_nms(60);
					usart6_write_string("检测项目:  ");
					delay_nms(60);
					usart6_write_string(Test_type);
					delay_nms(60);
					usart6_write_string("\r\n");
					delay_nms(60);
					usart6_write_string("检测结果:  ");
					delay_nms(60);
					usart6_write_string(Test_Dis_result0_ch);
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
				FR1000_ERROR_FLAG = 13;
				Error_popup(13);
				Test_inital_flag = 0;
			}
		}
				//点击了关机按钮
		else if (TOUCH_InRect(ts_event.x1, ts_event.y1, 0, 0, 40, 115) && (g_ucTouch == 1))//&&(ts_event.Key_Sta==Key_Up))
		{
			ts_event.x1 = -1;
			ts_event.y1 = -1;
			g_ucTouch = 0;
			_StoreUnstable(-1, -1);
			power_flag=Warning_popup(CODE_POWER_DOWN);
		
			if(power_flag == 1)
			{
				POWER_Down_deal();
				Power_disable();
				while(1);
			}
			else
			{
					 Test_inital_flag=0;	
			}
 		}
		
		if(test_choice != 0)                       //点开了其它三个按键 需要进行相应处理 如 设置 质控 历史记录查询等
		{
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
	while(1);
}

//处理输入信息对话框信息
void TEST_type_information(void)
{
	
	test_putin_flag = 1;
	
	CreateFramewin_INFORMATION();
	
	test_putin_flag = 0;
}
//刷新界面函数
void Fresh_Test_dispaly(unsigned char inter)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
	
	if(Chinese_display == 1)
	{
		bmp_dispflash(ADRESS_TEST_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
	}
	else
	{
		bmp_dispflash(ADRESS_EN_TEST_LEFT,0,40,LENGTH_LEFT_X,LENGTH_LEFT_Y);	
	}
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	//显示右边选择框
	if(inter == inter_test)
	{
	  if(Chinese_display == 1)
		{
	    bmp_dispflash(ADRESS_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}
		else
		{
			bmp_dispflash(ADRESS_EN_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
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
			GUI_DispStringAt("正在测试，请稍候......",40,120);
	    bmp_dispflash(ADRESS_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}
		else
		{
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_SetBkColor(BACK_COLOUR_24);
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("Testing,Please Wait......",40,120);
			bmp_dispflash(ADRESS_EN_TEST_RIGHT2,680,40,LENGTH_RIGHT_X,LENGTH_RIGHT_Y);	
		}
		
	  SERIAL_ROM_Font_mode();
	  Graphic_Mode();
		
		switch(send_mcu_cmd)
		{
			case S_ABSORB:
			{	
				Test_Progbar_set(0);  
				break;
			}
			case S_PRESS:
			{		
				Test_Progbar_set(10);  
				break;
			}
			case S_SAMPLE:
			{			
				Test_Progbar_set(20 + Test_pro_time/5);
				break;
			}
			default:
			{
				if(Test_pro_time > 0)
				{
				  Test_Progbar_set(20 + Test_pro_time/5);
				}
				break;
			}
		}
	}
	
	if(Chinese_display == 1)
	{
		GUI_SetFont(&GUI_FontHZ16);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("患者姓名:",42,335);	
		GUI_DispStringAt("患者编号:",242,335);
		GUI_DispStringAt("测试时间:",42,375);	
		GUI_DispStringAt("测试项目:",242,375);	
		GUI_DispStringAt(name_chinese_test,120,335);
		
		GUI_SetFont(&GUI_Font16_ASCII);		   //编号
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(number_test,320,335);
		GUI_DispStringAt(test_date_buf,120,375);
		
		GUI_SetFont(&GUI_FontHZ24);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("测试结果:",442,335);
	}
	else
	{
		GUI_SetFont(&GUI_Font16B_ASCII);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("Patient Name:",42,335);	
		GUI_DispStringAt("Patient Number:",242,335);
		GUI_DispStringAt("Test Time:",42,375);	
		GUI_DispStringAt("Test Items:",242,375);
		GUI_DispStringAt(number_test,350,335);	
		GUI_DispStringAt(test_date_buf,120,375);
		
		GUI_SetFont(&GUI_FontHZ16);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt(name_chinese_test,135,335);
		
		GUI_SetFont(&GUI_Font24_ASCII);
		GUI_SetBkColor(BACK_COLOUR_24);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringAt("Test Result:",442,335);
	}
}


#define  PROGBAR_TEST_CLR_BKG        GUI_LIGHTBLUE
#define  PROGBAR_TEST_CLR            GUI_GREEN
#define  PROGBAR_TEST_X              40
#define  PROGBAR_TEST_Y              160
#define  PROGBAR_TEST_SIZEX          500
#define  PROGBAR_TEST_SIZEY          20
#define  PROGBAR_TEST_TOTAL          100
//设置进度条 方法：创建进度条插件 刷新精度后再删除插件 以防止出错
void Test_Progbar_set(unsigned char Progbar_data)
{
	PROGBAR_Handle progbar_test;                          //进度条显示的句柄
	/*建立进度条控件 显示质控进度*/
	progbar_test = PROGBAR_Create(PROGBAR_TEST_X,PROGBAR_TEST_Y, PROGBAR_TEST_SIZEX, PROGBAR_TEST_SIZEY, WM_CF_SHOW);  
	PROGBAR_EnableMemdev(progbar_test);
	PROGBAR_SetBarColor(progbar_test, 0, PROGBAR_TEST_CLR);       //已刷新的进度条颜色
	PROGBAR_SetBarColor(progbar_test, 1, PROGBAR_TEST_CLR_BKG);       //未刷新的进度条颜色
	PROGBAR_SetTextAlign(progbar_test, GUI_TA_CENTER);     //字体位于进度条中心
	PROGBAR_SetMinMax(progbar_test,0,100);                 //进度条的最大值和最小值
	PROGBAR_SetTextColor(progbar_test,0,GUI_BLACK);        //设置字体颜色
	PROGBAR_SetTextColor(progbar_test,1,GUI_BLACK);        //设置字体颜色
	
	PROGBAR_SetValue(progbar_test, Progbar_data);   
	
	GUI_Exec();		
	
	PROGBAR_Delete(progbar_test);
}

unsigned char TEST_state = S_START;
unsigned char TEST_twinkle_flag = 0;
unsigned char Test_error_time = 0;
unsigned char Test_end = 0;
//Test_end = 0:正常状态         
//Test_end = 1:FPGA发送过来的数据出错   
//Test_end = 2:等待FPGA发来试剂卡插入状态超时
//Test_end = 3:等待FPGA发来SPOT CODE扫描完成超时
//Test_end = 4:等待FPGA发来采血管插入成功状态超时
//Test_end = 5:等待FPGA发来血液吸入加热槽完成状态超时 
//Test_end = 6:等待FPGA发来血液压入测量槽完成状态超时 
//Test_end = 7:等待FPGA发来测量完成状态超时
//Test_end = 8:测量过程中点击了结束测量按钮
//Test_end = 9:SPOT CODE未扫描 请扫描SPOT CODE
//Test_end = 10:测量过程中拔出试剂卡
//Test_end = 11:测量过程中拔出采血管
//Test_end = 12:测量过程中 LED亮度调节失败 此时会收到四个通道的DA值并显示在错误提示框中
unsigned char Test_DA_value[4] = {0};

unsigned char Test_window_time = 0;
unsigned char Test_spot_code_h = 0,
              Test_spot_code_m = 0,
              Test_spot_code_l = 0;
void TEST_begin(void)
{
  unsigned char test_result_h[4] = {0}, test_result_l[4] = {0};
  //unsigned char Test_result_char[5] = {0}, ch2_test_result_char[5] = {0}, ch3_test_result_char[5] = {0}, ch4_test_result_char[5] = {0};
  unsigned char Test_result_char[5] = 0;
  unsigned int  test_result_total[4] = {0};
  int i = 0,j = 0;
	unsigned long spot_code_total = 0;

	TEST_state = S_START;
	while(1)
	{
		time_display(&time_flag);
		switch(TEST_state)                        //发送控制指令
		{
			case S_START:
			{
				TEST_state = S_CHECK_BLOOD;
				send_mcu_cmd = S_START;
				mcu_send_cmd();
				break;
			}
			case S_SPOT_CODE:
			{
				TEST_state = S_CHECK_BLOOD;
				send_mcu_cmd = S_SPOT_CODE;
				mcu_send_cmd();
				break;
			}
			case S_CHECK_BLOOD:
			{
				TEST_state = S_ABSORB;
				send_mcu_cmd = S_CHECK_BLOOD;
				mcu_send_cmd();
				break;
			}
			case S_ABSORB:
			{
				TEST_state = S_PRESS;
				send_mcu_cmd = S_ABSORB;
				mcu_send_cmd();
				break;
			}
			case S_PRESS:
			{
				TEST_state = S_SAMPLE;
				send_mcu_cmd = S_PRESS;
				mcu_send_cmd();
				break;
			}
			case S_SAMPLE:
			{
				TEST_state = S_START;
				send_mcu_cmd = S_SAMPLE;
				mcu_send_cmd();
				Test_finish_flag = 0;
				Test_pro_time = 0;
				break;
			}
			case S_STOP:
			{
				TEST_state = S_START;
				send_mcu_cmd = S_STOP;
				mcu_send_cmd();
				Test_finish_flag = 0;
				break;
			}
		}
		
		TEST_twinkle_flag = 0;
		Test_error_time = 0;                      //错误计数清零
		Test_end = 0;                             //错误标志位
		rec_mcu_flag = 4;                            //=2则表示收到正确的回复
		Test_time = TEST_timeout;              //超时时间置位
		while(1)
		{
			time_display(&time_flag);
			mcu_uart_Process();
			if(rec_mcu_flag != 2)
			{
				switch(send_mcu_cmd)
				{
					case S_START:                                           //闪烁插卡标识
					{
						if(TEST_twinkle_flag == 0)
						{
							//闪烁插卡标志 提示用户插入试剂卡
							RA8875_FillRect(100,170,110,100,BACK_COLOUR_16);
							TEST_twinkle_flag = 1;
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
							TEST_twinkle_flag = 0;
						}	
            if(Test_time == 0)
						{
							Test_end = 2;
						}							
						break;
					}
					case S_SPOT_CODE:
					{
            if(Test_time == 0)
						{
							Test_end = 3;
						}			
						break;
					}
					case S_CHECK_BLOOD:                                  //闪烁采血管标识
					{
						if(TEST_twinkle_flag == 0)                       
						{
							RA8875_FillRect(300,170,110,40,BACK_COLOUR_16);
							TEST_twinkle_flag = 1;
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
							TEST_twinkle_flag = 0;
						}
            if(Test_time == 0)
						{
							Test_end = 4;
						}			
						break;
					}
					case S_ABSORB:
					{					
            if(Test_time == 0)
						{
							Test_end = 5;
						}							
						break;
					}
					case S_PRESS:
					{	
            if(Test_time == 0)
						{
							Test_end = 6;
						}								
						break;
					}
					case S_SAMPLE:
					{		
            if(Test_time == 0)
						{
							Test_end = 7;
						}
						
						if(Test_pro_time < 400)
						{
							Test_pro_time++;
							Test_Progbar_set(20 + Test_pro_time/5);
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
						Test_end = 8;
						TEST_state = S_START;
				    send_mcu_cmd = S_STOP;
				    mcu_send_cmd();
						break;
					}
					else
					{
           if((send_mcu_cmd == S_START)||(send_mcu_cmd == S_CHECK_BLOOD))
					  {
					    Fresh_Test_dispaly(inter_test);
					  }
					  else if((send_mcu_cmd == S_ABSORB)||(send_mcu_cmd == S_PRESS)||(send_mcu_cmd == S_SAMPLE))
					  {
						  Fresh_Test_dispaly(inter_prog);
					  }
					}
				}
				//从机未收到数据或受到错误数据 则计算出错次数 若出错次数多于5次 则结束此次测量并报错
				if((rec_mcu_flag == 1))
				{
					Test_error_time++;
					rec_mcu_flag = 4;
				}
				if(Test_error_time == 5)
				{
					Test_end = 1;
					Test_error_time = 0;
				}
				if(rec_mcu_flag == 3)                        //从机报错
				{
					if(rec_mcu_error == MCU_ERR_CARD_OUT)
					{
						Test_end = 10;
					}
					else if(rec_mcu_error == MCU_ERR_TUBE_OUT)
					{
						Test_end = 11;
					}
					else if(rec_mcu_error == MCU_ERR_BLOOD_WRONG)
					{
						Test_end = 12;
						for(i = 0;i < 4;i++)
						{
							Test_DA_value[i] = rec_mcu_buf[6 + i];
						}
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
            Test_spot_code_h = rec_mcu_buf[5];
		        Test_spot_code_m = rec_mcu_buf[6];
		        Test_spot_code_l = rec_mcu_buf[7];
						
						spot_code_total = 65535 * Test_spot_code_h + 255 * Test_spot_code_m + Test_spot_code_l;
						
						
						if(1)                                               
						{
							if(1)                                            //spot code已扫描
							{
								if(spot_code_total % 10 == 3)                  //P2Y12
								{
									Test_type_flag = 2;
									memset(&Test_type,0,sizeof(Test_type));
									memcpy(Test_type,Test_TYPE_P2Y12,strlen(Test_TYPE_P2Y12));
									memset(&Test_Unit,0,sizeof(Test_Unit));
									memcpy(Test_Unit,Test_UNIT_P2Y12,strlen(Test_UNIT_P2Y12));
								}
								else if(spot_code_total % 10 == 2)             //Aspirin
								{
									Test_type_flag = 1;
									memset(&Test_type,0,sizeof(Test_type));
									memcpy(Test_type,Test_TYPE_ASPIRIN,strlen(Test_TYPE_ASPIRIN));
									memset(&Test_Unit,0,sizeof(Test_Unit));
									memcpy(Test_Unit,Test_UNIT_ASPIRIN,strlen(Test_UNIT_ASPIRIN));
								}
								else if(spot_code_total % 10 == 4)              //GPIIb/IIIa
								{
									Test_type_flag = 3;
									memset(&Test_type,0,sizeof(Test_type));
									memcpy(Test_type,Test_TYPE_GP2B_3A,strlen(Test_TYPE_GP2B_3A));
									memset(&Test_Unit,0,sizeof(Test_Unit));
									memcpy(Test_Unit,Test_UNIT_GP2B_3A,strlen(Test_UNIT_GP2B_3A));
								}
								else                                           //Wrong
								{
									;
								}
							}
							else                                             //spot code未扫描
							{
								Test_end = 9;
							}							
						}
						else if(1)                                         
						{
							if(1)                                            //spot code已扫描
							{
								;
							}
							else                                             //spot code未扫描
							{
								Test_end = 9;
							}
						}
						else                                             //spot code未扫描
						{
							Test_end = 9;
						}
						break;
					}
					case S_SPOT_CODE:
					{
						;
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
						for(Test_window_time=0;Test_window_time<10;Test_window_time++)
						{
							if(TEST_twinkle_flag == 0)
							{
								if(Chinese_display == 1)
								{
								  RA8875_FillRect(450,165,140,200,BACK_COLOUR_16);
								}
								else
								{
									RA8875_FillRect(470,165,140,200,BACK_COLOUR_16);
								}
								TEST_twinkle_flag = 1;
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
								TEST_twinkle_flag = 0;
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
									Test_end = 8;
									TEST_state = S_START;
									send_mcu_cmd = S_STOP;
									mcu_send_cmd();
									break;
								}
								else
								{
								 if((send_mcu_cmd == S_START)||(send_mcu_cmd == S_CHECK_BLOOD))
									{
										Fresh_Test_dispaly(inter_test);
									}
									else if((send_mcu_cmd == S_ABSORB)||(send_mcu_cmd == S_PRESS)||(send_mcu_cmd == S_SAMPLE))
									{
										Fresh_Test_dispaly(inter_prog);
									}
								}
							}
							OSTimeDlyHMSM(0, 0, 0, 300);
						}
						if(Test_end != 8)
						{
						  Fresh_Test_dispaly(inter_prog);
						
	            Test_Progbar_set(0);   
						}
						break;
					}
					case S_ABSORB:
					{
						Heat_enable();                              //开始加热血液
		        delay_nms(50);	
		        Heat_disable();
						
	          Test_Progbar_set(10);  
						break;
					}
					case S_PRESS:
					{		
	          Test_Progbar_set(20);  
						break;
					}
					case S_SAMPLE:
					{	
						if(rec_mcu_buf[3] > 6)
						{
							
							for(i = 16; i < 60; i = i + 12)
							{
								if(i > rec_mcu_buf[3])
								{
									break;
								}
								test_result_h[rec_mcu_buf[i]-1]  = rec_mcu_buf[i - 11];
								test_result_l[rec_mcu_buf[i]-1]  = rec_mcu_buf[i - 10];
								max_data_value[rec_mcu_buf[i]-1] = rec_mcu_buf[i - 9]*255 + rec_mcu_buf[i - 8];
							  max_data_position[rec_mcu_buf[i]-1] = rec_mcu_buf[i - 7]*255 + rec_mcu_buf[i - 6];
								min_data_value[rec_mcu_buf[i]-1] = rec_mcu_buf[i - 5]*255 + rec_mcu_buf[i - 4];
							  min_data_position[rec_mcu_buf[i]-1] = rec_mcu_buf[i - 3]*255 + rec_mcu_buf[i - 2];
							  result_value[rec_mcu_buf[i]-1] = rec_mcu_buf[i-1]; 
							}
							
						}
						else
						{
							for(i=0; i<4; i++)
							{
								test_result_h[i]  = 0;
								test_result_l[i]  = 220;
								max_data_value[i] = 0;
							  max_data_position[i] = 0;
								min_data_value[i] = 0;
							  min_data_position[i] = 0;
							  result_value[i] = 0; 
							}
						}
						for(i = 0; i < 4; i++)
					  {
							test_result_total[i] = test_result_h[i]*255 + test_result_l[i];
						}
						
						switch(Test_type_flag)
						{
							case 1:         //Aspirin
							{
								Test_Result_data0 = test_result_total[3];
								Test_Result_data1 = 0;
								Test_Result_data2 = 0;
								
								//将结果0改为字符串以方便显示和保存
								memset(Test_result_char,0,sizeof(Test_result_char));
								for(i = 0;i < 5;i++)
								{
									Test_result_char[i] = Test_Result_data0 % 10 + '0';
									Test_Result_data0 = Test_Result_data0/10;
									if(Test_Result_data0 == 0)
									{
										break;
									}
								}
								memset(Test_Dis_result0_ch,0,sizeof(Test_Dis_result0_ch));
								memset(Test_Save_Result0_ch,0,sizeof(Test_Save_Result0_ch));
								//显示用字符串
								Test_Dis_result0_ch[0] = 'A';
								Test_Dis_result0_ch[1] = 'R';
								Test_Dis_result0_ch[2] = 'U';
								Test_Dis_result0_ch[3] = ' ';
								for(j=0;j<i+1;j++)
								{
									Test_Dis_result0_ch[j+4] = Test_result_char[i-j];
									Test_Save_Result0_ch[j] = Test_result_char[i-j];
								}	
								memset(Test_Save_Result1_ch,0,sizeof(Test_Save_Result1_ch));
								memset(Test_Save_Result2_ch,0,sizeof(Test_Save_Result2_ch));
								break;
							}
							case 2:         //P2Y12
							{
								Test_Result_data0 = test_result_total[3];
								Test_Result_data1 = test_result_total[1];
								Test_Result_data2 = Test_Result_data0 * 100 / Test_Result_data1;
								
								//将结果0改为字符串以方便显示和保存
								memset(Test_result_char,0,sizeof(Test_result_char));
								for(i = 0;i < 5;i++)
								{
									Test_result_char[i] = Test_Result_data0 % 10 + '0';
									Test_Result_data0 = Test_Result_data0/10;
									if(Test_Result_data0 == 0)
									{
										break;
									}
								}
								memset(Test_Dis_result0_ch,0,sizeof(Test_Dis_result0_ch));
								memset(Test_Save_Result0_ch,0,sizeof(Test_Save_Result0_ch));
								//显示用字符串
								Test_Dis_result0_ch[0] = 'P';
								Test_Dis_result0_ch[1] = 'R';
								Test_Dis_result0_ch[2] = 'U';
								Test_Dis_result0_ch[3] = ' ';
								for(j=0;j<i+1;j++)
								{
									Test_Dis_result0_ch[j+4] = Test_result_char[i-j];
									Test_Save_Result0_ch[j] = Test_result_char[i-j];
								}	
								
								//将结果1改为字符串以方便显示和保存
								memset(Test_result_char,0,sizeof(Test_result_char));
								for(i = 0;i < 5;i++)
								{
									Test_result_char[i] = Test_Result_data1 % 10 + '0';
									Test_Result_data1 = Test_Result_data1/10;
									if(Test_Result_data1 == 0)
									{
										break;
									}
								}
								memset(Test_Dis_result1_ch,0,sizeof(Test_Dis_result1_ch));
								memset(Test_Save_Result1_ch,0,sizeof(Test_Save_Result1_ch));
								Test_Dis_result1_ch[0] = 'P';
								Test_Dis_result1_ch[1] = 'R';
								Test_Dis_result1_ch[2] = 'U';
								Test_Dis_result1_ch[3] = ' ';
								for(j=0;j<i+1;j++)
								{
									Test_Dis_result1_ch[j+4] = Test_result_char[i-j];
									Test_Save_Result1_ch[j] = Test_result_char[i-j];
								}		
								
								//将结果2改为字符串以方便显示和保存
								memset(Test_result_char,0,sizeof(Test_result_char));
								for(i = 0;i < 5;i++)
								{
									Test_result_char[i] = Test_Result_data2 % 10 + '0';
									Test_Result_data2 = Test_Result_data2/10;
									if(Test_Result_data2 == 0)
									{
										break;
									}
								}
								memset(Test_Dis_result2_ch,0,sizeof(Test_Dis_result2_ch));
								memset(Test_Save_Result2_ch,0,sizeof(Test_Save_Result2_ch));
								for(j = 0; j < i+1; j++)
								{
									Test_Dis_result2_ch[j] = Test_result_char[i-j];
									Test_Save_Result2_ch[j] = Test_result_char[i-j];
								}		
								Test_Dis_result2_ch[j] = '%';
								Test_Save_Result2_ch[j] = '%';
								break;
							}
							case 3:         //GPIIb/IIIa
							{
								break;
							}
							default:
							{
								break;
							}
						}
						
						
						
						
						
						
	          Test_Progbar_set(100); 	
            Test_finish_flag = 1;						
						break;
					}
					case S_STOP:
					{						
						break;
					}
				}
				break;
			}
			
			if(Test_end != 0)
			{
				break;
			}
			if(Test_time > 0)
			{
				Test_time--;
				//30s检查一次 若没有收到ack指令 则重新发送命令字节
				if(((Test_time % 100) == 0) && (rec_mcu_flag == 4))
				{
					mcu_send_cmd();
				}
			}
			OSTimeDlyHMSM(0, 0, 0, 300);
		}
		//先进行出错处理
		if(Test_end != 0)
		{
			FR1000_ERROR_FLAG = Test_end;
			if(Test_end != 8)
			{  
			  Error_popup(Test_end);
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

//保存历史记录至nandflash
void Save_Test_History(_Save_Test_Data_Struct Save_data,NAND_ADDRESS* Address)
{
	int j = 0;
	unsigned char block_erase_flag = 0, block_bad_flag = 0;
	unsigned char write_success = 0;
	unsigned char ZeroBuf[12] = {0};
	
  close_lcd_cs();
  enable_nandflash_cs();
	
	
	while(block_erase_flag == 0)
	{
		//从一个完整的block开始时 检查block是否为坏块 若是坏块 则跳过 若不是 则擦除该块以便重新写入数据
		//若此时不是一个完整的block开始 则标记擦除标志为已完成即可
		if((Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) % NAND_BLOCK_SIZE == 0 )
		{
			while(block_bad_flag == 0)
			{
				for(j = 0; j < g_bad_block_count; j++)
				{
					//是坏块则跳至下一个块并判断
					if(bad_block[j] == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
					{
						FSMC_NAND_AddressIncrement(Address);
						if( TEST_HISTORY_END == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) ) 
						{
							*Address = TEST_HISTORY_START;
						}
						j = 0;
						break;	
					}
				}
				if(j == g_bad_block_count)
				{
					//找到合法的块 则置位相关标志位并擦除该块
					if(NAND_TIMEOUT_ERROR == FSMC_NAND_EraseBlock(*Address))
					{
						//擦除失败 标记该块为坏块并跳至下一个块继续查找
						bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
						g_bad_block_count++;
						memset(ZeroBuf,0,sizeof(ZeroBuf));
						FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
						FSMC_NAND_AddressIncrement(Address);
						if( TEST_HISTORY_END == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) )
						{
							*Address = TEST_HISTORY_START;
						}
					}
					else
					{
						//擦除成功 则置位标志位即可
						block_bad_flag = 1;
						block_erase_flag = 1;
					}
				}
		  }
		}
		else
		{
			block_erase_flag = 1;
		}
  }
	
	block_bad_flag = 0;
	
	
	while(write_success == 0)
	{
		if(NAND_TIMEOUT_ERROR == FSMC_NAND_WriteSmallPage((unsigned char *)&Save_data,*Address,1))
		{
			//写入失败 则标记当前块为坏块并跳至下一个块继续判断是否为坏块 
			bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
			g_bad_block_count++;
			memset(ZeroBuf,0,sizeof(ZeroBuf));
			FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
			FSMC_NAND_AddressIncrement(Address);
			if( TEST_HISTORY_END == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) )
			{
				*Address = TEST_HISTORY_START;
			}
		}
		else
		{
			//写入成功 跳至下一页 标志位置位
			Address->Page++;
			if(Address->Page==32)
			{
				    Address->Page=0;
				    FSMC_NAND_AddressIncrement(Address);
			}
			write_success = 1;
			block_bad_flag = 1;
			break;
		}
		
		while(block_bad_flag == 0)
		{
			for(j = 0; j < g_bad_block_count; j++)
			{
				//是坏块则跳至下一个块并判断
				if(bad_block[j] == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE))
				{
					FSMC_NAND_AddressIncrement(Address);
					if( TEST_HISTORY_END == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) ) 
					{
						*Address = TEST_HISTORY_START;
					}
					j = 0;
					break;
				}
			}
			if(j == g_bad_block_count)
			{
				//找到合法的块 则置位相关标志位并擦除该块
				if(NAND_TIMEOUT_ERROR == FSMC_NAND_EraseBlock(*Address))
				{
					//擦除失败 标记该块为坏块并跳至下一个块继续查找
					bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
					g_bad_block_count++;
					memset(ZeroBuf,0,sizeof(ZeroBuf));
					FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
					FSMC_NAND_AddressIncrement(Address);
					if( TEST_HISTORY_END == (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE) )
					{
						*Address = TEST_HISTORY_START;
					}
				}
				else
				{
					//擦除成功 则置位标志位即可
					block_bad_flag = 1;
				}
			}
		}
	}
	
	enable_lcd_cs();
  close_nandflash_cs();
}


//正常时返回该值 否则数组的第92个值为0x55  正常为0xFF
void Read_Test_History(_Save_Test_Data_Struct *Read_data,NAND_ADDRESS* Address)
{
	close_lcd_cs();
  enable_nandflash_cs();
	
	if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage((unsigned char *)Read_data,*Address,1))
	{
		//读取超时 标记该块为坏块
		bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
		g_bad_block_count++;
		memset(ZeroBuf,0,sizeof(ZeroBuf));
		FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
		Read_data->Delete_flag = 0x55;
	}
	
	close_lcd_cs();
  enable_nandflash_cs();
}

//向该页的第92位写入0x00
void Delete_Test_History(NAND_ADDRESS* Address)
{
	unsigned char data_fifo[512];
	
	close_lcd_cs();
  enable_nandflash_cs();
	
	if(NAND_TIMEOUT_ERROR == FSMC_NAND_ReadSmallPage(data_fifo,*Address,1))
	{
		//读取超时 标记该块为坏块
		bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
		g_bad_block_count++;
		memset(ZeroBuf,0,sizeof(ZeroBuf));
		FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
	}
	
	data_fifo[91] = 0x00;
	
	if(NAND_TIMEOUT_ERROR == FSMC_NAND_WriteSmallPage(data_fifo,*Address,1))
	{
		//写入失败 则标记当前块为坏块
		bad_block[g_bad_block_count] = (Address->Page + (Address->Block + (Address->Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE);
		g_bad_block_count++;
		memset(ZeroBuf,0,sizeof(ZeroBuf));
		FSMC_NAND_WriteSpareArea(ZeroBuf, *Address, 1);
	}
	
	close_lcd_cs();
  enable_nandflash_cs();
}


//功能：将从机的DA参数保存在EEPROM中
void MCU_PARA_SAVE(void)
{
	I2C_Configuration();
  AT24C512_WritePage(ADDR_24C512, MCU_PARA_START_ADDR, MCU_PARA_DATA, MCU_PARA_LENGTH);
}
//功能：从EEPROM中读出DA参数
void MCU_PARA_READ(void)
{
	I2C_Configuration();
  AT24C512_ReadPage(ADDR_24C512, MCU_PARA_START_ADDR, MCU_PARA_DATA, MCU_PARA_LENGTH);
}
//函数功能：发送指令给从机调节DAC参数并等待调节完成 回传参数
void MCU_ADJUST_DAC(void)
{
	int i = 0;
	OS_CPU_SR  cpu_sr;
	
	
	send_mcu_cmd = S_ADJUSTDAC;
  mcu_send_cmd();
	rec_mcu_flag = 4;                            //=2则表示收到正确的回复
	Test_error_time = 0;                         //错误计数清零
	Test_time = TEST_timeout;                    //超时时间置位
	
	
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	OSIntNesting++;
	bmp_dispflash(ADRESS_TEXT_BK,TEXT_BK_POSITION_X,TEXT_BK_POSITION_Y,LENGTH_TEXT_BK_X,LENGTH_TEXT_BK_Y);	
	SERIAL_ROM_Font_mode();
	Graphic_Mode();
	
	GUI_SetFont(&GUI_FontHZ16);
	GUI_SetBkColor(BACK_COLOUR_24);
	GUI_SetColor(GUI_BLACK);
	GUI_DispStringAt("从机调节中...",350,180);	
	
	
	OS_EXIT_CRITICAL();
	OSIntExit();        //触发任务切换软中断
	
	while(1)
	{
		time_display(&time_flag);                  //刷新时间显示
		mcu_uart_Process();                        //处理接收到的数据
		if(rec_mcu_flag != 2)                      //从机尚未发送调节完成指令
		{
			//从机未收到数据或受到错误数据 则计算出错次数 若出错次数多于5次 则结束此次测量并报错
			if(rec_mcu_flag == 1)
			{
				Test_error_time++;
				rec_mcu_flag = 4;
			}
			if(Test_error_time == 5)
			{
				break;
			}
			//从机报错 则提示错误并退出
			if(rec_mcu_flag == 3)                        
			{
				if(rec_mcu_error == MCU_ERR_CARD_OUT)
				{
					//调节过程中拔出调节卡
					;
				}
				else if(rec_mcu_error == MCU_ERR_ADJUSTDAC)
				{
					//调节出错 
					;
				}
				rec_mcu_flag = 4;
				break;
			}			
			if(Test_time > 0)
			{
				Test_time--;
				//30s检查一次 若没有收到ack指令 则重新发送命令字节
				if(((Test_time % 100) == 0) && (rec_mcu_flag == 4))
				{
					mcu_send_cmd();
				}
			}
		}
		else
		{
			
			OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		  OSIntNesting++;
			
			bmp_dispflash(ADRESS_TEXT_BK,TEXT_BK_POSITION_X,TEXT_BK_POSITION_Y,LENGTH_TEXT_BK_X,LENGTH_TEXT_BK_Y);	
	    SERIAL_ROM_Font_mode();
	    Graphic_Mode();
			
			GUI_SetFont(&GUI_FontHZ16);
			GUI_SetBkColor(BACK_COLOUR_24);
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("从机调节完成",350,180);
			
			GUI_SetFont(&GUI_Font16_ASCII);		      
			GUI_SetBkColor(BACK_COLOUR_24);
			GUI_SetColor(GUI_BLACK);
			
			
			//读取参数并保存
			for(i = 0; i < 8; i++)
			{
			  MCU_PARA_DATA[i] = rec_mcu_buf[5+i];
				GUI_DispDecAt(MCU_PARA_DATA[i],210+50*i,240,4);
			}
			MCU_PARA_SAVE();
			OS_EXIT_CRITICAL();
	    OSIntExit();        //触发任务切换软中断
			
			OSTimeDlyHMSM(0, 0, 10, 0); 
			Test_inital_flag = 0;
			break;
		}
		OSTimeDlyHMSM(0, 0, 0, 300);                                 //延时
	}
}