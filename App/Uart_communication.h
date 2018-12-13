#ifndef _USART_COMMUNICATION_H
#define _USART_COMMUNICATION_H

#include "usart.h"
#include "stdbool.h"

#define     S_START           0x7F                     //启动自举程序
#define     S_GETALL          0x00                     //获取自举程序版本号和命令
#define     S_GETVER          0x01                     //获取版本和Flash读保护状态
#define     S_GETID           0x02                     //获取芯片ID
#define     S_READ            0x11                     //读指令
#define     S_GO              0x21                     //跳转指令
#define     S_WRITE           0x31                     //写指令
#define     S_ERASE0          0x43                     //擦除指令
#define     S_ERASE1          0x44                     //扩展擦除指令
#define     S_WRITE_PRO       0x63                     //写保护指令
#define     S_WRITE_UNPRO     0x73                     //禁止写保护指令
#define     S_READ_PRO        0x82                     //读保护指令
#define     S_READ_UNPRO      0x92                     //禁止读保护指令

#define     S_ACK             0x79
#define     S_NACK            0x1F


#define     S_W_CMD           0x01
#define     S_W_ADD           0x02
#define     S_W_NUM           0x03

#define     MCU_START_FLASH   ((uint32_t)0x08000000)



void delay_1us(void);
void delay_nus(u32 n);
void delay_1ms(void);
void delay_nms(u32 n);
unsigned char Generate_XorCheckByte(unsigned char *SourceByte_pt, unsigned int CheckByteNum);
int waitACK();
void uart5_write (unsigned char * msg_string,unsigned short size);
void send_command(unsigned char cmd_byte);
void send_adress(unsigned int ADRESS_U32);
unsigned char Write_mcu_flash(unsigned int flash_address,unsigned char * write_data,unsigned int write_length);
int Read_mcu_flash(unsigned int flash_address,unsigned char * read_data,unsigned int read_length);
int stm32isp_verify(unsigned char * write_buffer,unsigned char * read_buffer,int len);
void send_erase_data(void);
unsigned char flash_erase_all(void);

int stm32isp_sync();
int get_ver_command();
int get_ID_command();

extern volatile int mcu_count;
extern unsigned char rcv_mcu_buf[260];
extern unsigned char mcu_commu_sucess;









#endif
