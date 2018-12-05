#ifndef GUI_TEST_H

#define GUI_TEST_H

#include "DIALOG.h"

#define test_timeout    500               //超时时间 单位为ms
typedef struct 
{
	unsigned char Patient_id[20];
	unsigned char Patient_name[14];
	unsigned char Item[16];                       //P2Y12        Aspirin    GPIIb/IIIa
	unsigned char Time[24];
	unsigned char Result_0[4];                    //PRU          ARU        PAU_0
	unsigned char Result_1[4];                    //BASELINE     0          PAU_1
	unsigned char Result_2[4];                    //%            0          %
	unsigned char Unit[4];                        //PRU          ARU        PAU
	unsigned char Write_flag;                     //0x00:该页已写入数据     0xFF：该页未写入数据
	unsigned char Delete_flag;                    //0x00:该页数据已删除     0xFF：该页数据未删除
}
_Save_Test_Data_Struct;
void Create_Test(void);
void TEST_type_information(void);
void TEST_begin(void);
void MCU_PARA_READ(void);


extern unsigned char test_choice;
extern unsigned char FPGA_IO_data;                               //接受FPGA发送过来的数据
extern unsigned char FPGA_IO_flag;                               //置1则表示FPGA端有数据传输过来 
extern unsigned char start_flag;
extern unsigned char Test_DA_value[4];

//MCU 发送给 FPGA 的控制指令
#define RESET_COMMAND                 0x00        //复位状态指令
#define INSERT_CARD_COMMAND           0x01        //开始测量 插入试剂卡
#define SCAN_SPOT_CODE_COMMAND        0x02        //试剂卡插入成功 扫描SPOT CODE
#define INSERT_TUBE_COMMAND           0x03        //SPOT CODE正常 插入采血管
#define SUCK_BLOOD_COMMAND            0x04        //开始抽血到加热槽
#define HEAT_FINISHED_COMMAND         0x05        //加热完成 将血压入测量槽
#define START_MEASURE_COMMAND         0x06        //开始采集数据
#define MEASURE_FINISHED_COMMAND      0x07        //数据采集完成 

//FPGA 发送给 MCU 的状态指令
#define RESET_STATUES                 0x00        //复位状态
#define CARD_FINISHED_STATUES         0x01        //试剂卡插入成功
#define SPOT_FINISHED_STATUES         0x02        //SPOT CODE扫描完成 将要开始在SPI上传输数据
#define TUBE_FINISHED_STATUES         0x03        //采血管插入成功 
#define SUCK_FINISHED_STATUES         0x04        //血液吸入加热槽完成 
#define PRESS_FINISHED_STATUES        0x05        //血液压入测量槽完成
#define MEASURE_FINISHED_STATUES      0x06        //测量完成

#define inter_test                    0x01
#define inter_prog                    0x02

#define BACK_COLOUR_24                0xF1F1F1    //0xEFEFEF
#define BACK_COLOUR_16                0xF79E      //0xEF7D


#define  MCU_PARA_START_ADDR  0x100
// #define  MCU_PARA_CH2_ADDR  0x102
// #define  MCU_PARA_CH3_ADDR  0x104
// #define  MCU_PARA_CH4_ADDR  0x106
#define  MCU_PARA_LENGTH    8

#ifndef GUI_FLASH 
	#define GUI_FLASH 
#endif 
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ16;
extern unsigned char FR1000_ERROR_FLAG;
extern unsigned char MCU_PARA_DATA[MCU_PARA_LENGTH];
#endif