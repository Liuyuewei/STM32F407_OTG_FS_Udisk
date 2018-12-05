#ifndef GUI_TEST_H

#define GUI_TEST_H

#include "DIALOG.h"

#define test_timeout    500               //��ʱʱ�� ��λΪms
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
	unsigned char Write_flag;                     //0x00:��ҳ��д������     0xFF����ҳδд������
	unsigned char Delete_flag;                    //0x00:��ҳ������ɾ��     0xFF����ҳ����δɾ��
}
_Save_Test_Data_Struct;
void Create_Test(void);
void TEST_type_information(void);
void TEST_begin(void);
void MCU_PARA_READ(void);


extern unsigned char test_choice;
extern unsigned char FPGA_IO_data;                               //����FPGA���͹���������
extern unsigned char FPGA_IO_flag;                               //��1���ʾFPGA�������ݴ������ 
extern unsigned char start_flag;
extern unsigned char Test_DA_value[4];

//MCU ���͸� FPGA �Ŀ���ָ��
#define RESET_COMMAND                 0x00        //��λ״ָ̬��
#define INSERT_CARD_COMMAND           0x01        //��ʼ���� �����Լ���
#define SCAN_SPOT_CODE_COMMAND        0x02        //�Լ�������ɹ� ɨ��SPOT CODE
#define INSERT_TUBE_COMMAND           0x03        //SPOT CODE���� �����Ѫ��
#define SUCK_BLOOD_COMMAND            0x04        //��ʼ��Ѫ�����Ȳ�
#define HEAT_FINISHED_COMMAND         0x05        //������� ��Ѫѹ�������
#define START_MEASURE_COMMAND         0x06        //��ʼ�ɼ�����
#define MEASURE_FINISHED_COMMAND      0x07        //���ݲɼ���� 

//FPGA ���͸� MCU ��״ָ̬��
#define RESET_STATUES                 0x00        //��λ״̬
#define CARD_FINISHED_STATUES         0x01        //�Լ�������ɹ�
#define SPOT_FINISHED_STATUES         0x02        //SPOT CODEɨ����� ��Ҫ��ʼ��SPI�ϴ�������
#define TUBE_FINISHED_STATUES         0x03        //��Ѫ�ܲ���ɹ� 
#define SUCK_FINISHED_STATUES         0x04        //ѪҺ������Ȳ���� 
#define PRESS_FINISHED_STATUES        0x05        //ѪҺѹ����������
#define MEASURE_FINISHED_STATUES      0x06        //�������

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