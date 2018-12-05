/*
*********************************************************************************************************
*                                             uC/GUI V3.98
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
File        : GUI_Exec.c
Purpose     : Implementation of GUI_Exec
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI_Protected.h"
#include "OS_CPU.H"
#include "uCOS_II.H"
#include <stdbool.h>
#if GUI_WINSUPPORT
#include "WM.h"
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_Exec1
*/
int GUI_Exec1(void) {
  int r = 0;
  /* Execute background jobs */
  if (GUI_pfTimerExec) {
    if ((*GUI_pfTimerExec)()) {
      r = 1;                  /* We have done something */
    }
  }
  #if GUI_WINSUPPORT    /* If 0, WM will not generate any code */
  if (WM_Exec())
    r = 1;
  #endif
  return r;
}

/*********************************************************************
*
*       GUI_Exec
*/
extern bool Parameter_Inital_flag,WM_NOTIFY_BACK_flag;
extern unsigned char test_putin_flag ;
int GUI_Exec(void) {
  int r = 0;
	OS_CPU_SR  cpu_sr;
  while (GUI_Exec1()) {
    r = 1;                  /* We have done something */
		if(test_putin_flag==1||test_putin_flag==2||test_putin_flag==3)
		{
				OS_EXIT_CRITICAL();
				OSIntExit();        //触发任务切换软中?
		}
  }
	
  return r;
}

/*************************** End of file ****************************/
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
