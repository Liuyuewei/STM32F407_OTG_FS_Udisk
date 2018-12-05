#include "GUI_X_LCD.h"

/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_EVENT  *DispSem;
static  OS_EVENT  *EventMbox;

static  OS_EVENT  *KeySem;
static  int        KeyPressed;
static  char       KeyIsInited;





void GUI_X_Log(const char *s) 
{ 
 GUI_USE_PARA(s); 
}

void GUI_X_Warn(const char *s) 
{
 GUI_USE_PARA(s); 
}

void GUI_X_ErrorOut(const char *s)
{ 
GUI_USE_PARA(s); 
}

void GUI_X_ExecIdle (void) 
{
//    OS_X_Delay(1);

	OSTimeDly(50);
}

/*被GUI_Init()调用,用来初始化一些GUI运行之前需要用的硬件,如键盘或者鼠标之类的.如果不需要的话,可以为空*/
void GUI_X_Init (void) 
{
    KeySem = OSSemCreate(0);
}

void  GUI_X_Delay (int period) 
{
    INT32U  ticks;
    ticks = (period * 1000) / OS_TICKS_PER_SEC;
    OSTimeDly((INT16U)ticks);
//  	delay_nus(period);
}

int  GUI_X_GetTime (void) 
{
    return ((int)OSTimeGet());
}

void  GUI_X_InitOS (void)
{ 
    DispSem   = OSSemCreate(1);			   //建立一个互斥型信号量
    EventMbox = OSMboxCreate((void *)0);   //建立一个邮箱
}

void  GUI_X_Lock (void)
{ 
    INT8U  err;
    
    
    OSSemPend(DispSem, 0, &err);
}


void  GUI_X_Unlock (void)
{ 
    OSSemPost(DispSem);
}


U32  GUI_X_GetTaskId (void) 
{ 
    return ((U32)(OSTCBCur->OSTCBPrio));
}

void GUI_X_WaitEvent (void) 
{
    INT8U  err;


    (void)OSMboxPend(EventMbox, 0, &err);
}


void GUI_X_SignalEvent (void) 
{
    (void)OSMboxPost(EventMbox, (void *)1);
}

static  void  CheckInit (void) 
{
    if (KeyIsInited == 0) {
        KeyIsInited = 1;
        GUI_X_Init();
    }
}


int  GUI_X_GetKey (void) 
{
    int r;


    r          = KeyPressed;
    CheckInit();
    KeyPressed = 0;
    return (r);
}


int  GUI_X_WaitKey (void) 
{
    int    r;
    INT8U  err;


    CheckInit();
    if (KeyPressed == 0) {
        OSSemPend(KeySem, 0, &err);
    }
    r          = KeyPressed;
    KeyPressed = 0;
    return (r);
}


void  GUI_X_StoreKey (int k) 
{
    KeyPressed = k;
    OSSemPost(KeySem);
}