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

unsigned char Print_rcv_buf[10] = {0};
unsigned char rcv_print_count = 0;

void Print_inital(void)
{
	//初始化打印机
	usart6_write_data(0x1B);
	delay_nms(5);	
	usart6_write_data(0x40);
	delay_nms(5);
}
void Print_test(void)
{
	rcv_print_count = 0;
	
	usart6_write_data(0x1D);
	delay_nms(5);
	usart6_write_data(0x72);	
	delay_nms(5);
	usart6_write_data(0x01);
	delay_nms(100);
}