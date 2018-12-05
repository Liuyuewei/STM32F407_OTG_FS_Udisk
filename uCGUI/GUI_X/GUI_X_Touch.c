/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
#include "LCD_RA8875.h"
#include "touch.h"

uint8_t g_ucTouch = 0;
/*
#define ERR_RANGE 20 //误差范围 
u8 ft5x0x_read2_data() 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=ft5x0x_read_data();//第一次得到的坐标
//    if(flag==0)return(0);
		x1=ts_event.x1;
		y1=ts_event.y1;	
    flag=ft5x0x_read_data();//第二次得到的坐标    
 //   if(flag==0)return(0);   
		x2=ts_event.x1;
		y2=ts_event.y1;	
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        ts_event.x1=(x1+x2)/2;
        ts_event.y1=(y1+y2)/2;     
    }  
		else
		x2=x1;
		y2=y1;		
}
*/ 


// uint8_t TOUCH_GetKey(void)
// {
// 	uint16_t First_POSx1=0,First_POSy1=0;
// 	if(ts_event.Key_Sta==Key_Down)        //触摸屏被按下
// 	{
// 		Pen_Int_Set(0);//关闭中断
// 		delay_nms(10);                 //10
// 		do
// 		{
// 			ft5x0x_read_data();	
// //			ft5x0x_read2_data();			
// 			ts_event.Key_Sta=Key_Up;		
// 		}while(PEN()==0);//如果PEN一直有效,则一直执行	
// 		Pen_Int_Set(1);//开启中断
// 		if((0<ts_event.x1<800)&&(0<ts_event.y1<480))
//  		return 1;
//   	else
//  		return 0;			
// 	}                  
// 	else 
// 	{
// //		GPIOB->ODR^=GPIO_Pin_14;	
// 		return 0;
// 	}
// }


void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}

int  GUI_TOUCH_X_MeasureX(void) 
{
/*
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//数据组
	unsigned short temp=0,X=0;	
 	
	while(count<8)//循环读数10次
	{	   	  
		databuffer[count]=RA8875_TouchReadY();
		count++; 
		OSTimeDly(1); 
	}  
	if(count==8)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//升序排列
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 
	//	X=4000-(databuffer[3]+databuffer[4]+databuffer[5])/3;	 
		X=(databuffer[3]+databuffer[4]+databuffer[5])/3; 
	}
	return(X); 
*/
   
	//g_ucTouch = TOUCH_GetKey();
	/* 
	if(g_ucTouch == 1)
	{
		g_ucTouch = 0;
		return ts_event.x1;	
	} else
	{
		return -1;
	}
	*/
	return 0;
}

int  GUI_TOUCH_X_MeasureY(void) {
/*
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//数据组
	unsigned short temp=0,Y=0;	
 
    while(count<8)	//循环读数10次
	{	   	  
		databuffer[count]=RA8875_TouchReadX();
		count++; 
		OSTimeDly(1); 
	}  
	if(count==8)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//升序排列
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 
	//	Y=4200-(databuffer[3]+databuffer[4]+databuffer[5])/3;
		Y=(databuffer[3]+databuffer[4]+databuffer[5])/3;	  
	  
	}
	return(Y); 
*/
/*
	g_ucTouch = TOUCH_GetKey();
	if(g_ucTouch == 1)
	{
		return ts_event.y1;
	}else
	{
		return -1;
	}
*/
	return 0;
	
}

extern  int xPhys, yPhys;
int CalibrationComplete = 1;
GUI_PID_STATE State; 
void GUI_TOUCH_Measure(void)
 {  
	xPhys = -1; yPhys = -1;
	g_ucTouch = TOUCH_GetKey();
	 
	if(g_ucTouch == 1)
	{
		  xPhys = ts_event.x1;
	  	yPhys = ts_event.y1; 
		
// 		State.x = ts_event.x1;
// 	  State.y = ts_event.y1; 
// 		
// 		State.Pressed = 1;
// 		
// 		GUI_PID_StoreState(&State);
			
	}
		else
		{
			xPhys = -1;
	  	yPhys = -1;
			
			//GUI_PID_GetState(&State);
			State.x = -1;
			State.y = -1;
			State.Pressed = 0;
			GUI_PID_StoreState(&State);
 			_StoreUnstable(-1, -1);
		}
		//mydelay_nms(1000); 
}

//int  GUI_TOUCH_X_MeasureX(void) {
//  return 0;
//}
//
//int  GUI_TOUCH_X_MeasureY(void) {
//  return 0;
//}


uint8_t TOUCH_GetKey(void)
{
	if(ts_event.Key_Sta==Key_Down)        //触摸屏被按下
	{
		Pen_Int_Set(0);//关闭中断
		//Delaynms(5);
		do
		{
  			//I2C_Configuration();
			ft5x0x_read_data();
		}while(	ts_event.Key_Sta==Key_Down);//如果PEN一直有效,则一直执行	
		Pen_Int_Set(1);//开启中断
		if((0<ts_event.x1<800)&&(0<ts_event.y1<480))
		{
 		  return 1;
		}
  	else
		{
 		  return 0;			
		}
	}                  
	else 
	{
		return 0;
	}
}