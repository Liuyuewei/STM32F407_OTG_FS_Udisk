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

/*��GUI_Init()����,������ʼ��һЩGUI����֮ǰ��Ҫ�õ�Ӳ��,����̻������֮���.�������Ҫ�Ļ�,����Ϊ��*/
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
    DispSem   = OSSemCreate(1);			   //����һ���������ź���
    EventMbox = OSMboxCreate((void *)0);   //����һ������
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