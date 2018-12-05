#ifndef  GUI_X_LCD_H
#define  GUI_X_LCD_H

#include "delay.h"
#include "GUI_ConfDefaults.h"
#include <ucos_ii.h>
#include "GUI_Private.H"
#include "stdio.h"


void GUI_X_Log(const char *s);
void GUI_X_Warn(const char *s);
void GUI_X_ErrorOut(const char *s);
void GUI_X_ExecIdle (void);
void GUI_X_Init (void);
void  GUI_X_Delay (int period);
int  GUI_X_GetTime (void);
void  GUI_X_InitOS (void);
void  GUI_X_Lock (void);
void  GUI_X_Unlock (void);
U32  GUI_X_GetTaskId (void);
void GUI_X_WaitEvent (void);
void GUI_X_SignalEvent (void);
static  void  CheckInit (void);
int  GUI_X_GetKey (void);
int  GUI_X_WaitKey (void);
void  GUI_X_StoreKey (int k);

#endif