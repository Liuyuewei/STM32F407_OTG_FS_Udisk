#ifndef GUI_POPUP

#define GUI_POPUP

#include "DIALOG.h"


#define ERROR_DATA_F_MCU               0x00
#define ERROR_TIMEOUT_START            0x01
#define ERROR_TIMEOUT_SPOT_CODE        0x02
#define ERROR_TIMEOUT_BLOOD            0x03
#define ERROR_TIMEOUT_ABSORB           0x04
#define ERROR_TIMEOUT_PRESS            0x05
#define ERROR_TIMEOUT_SAMPLE           0x06
#define ERROR_UN_SPOT_CODE             0x07
#define ERROR_SPOT_CODE                0x08

#define DOWN_POSITION_Y                150        //ERROR提示图片的y轴位置
#define ERROR_POSITION_Y               150        //ERROR提示图片的y轴位置
#define WARNING_POSITION_Y             150        //ERROR提示图片的y轴位置
#define STR_POSITION_Y                 245        //字符串y轴位置
#define CONFIRM_SIZE_X                 170
#define CONFIRM_SIZE_Y                 44


#define TEXT_BK_POSITION_X             200
#define TEXT_BK_POSITION_Y             140


#define CODE_END_MEASUREMENT           0x00     //测量过程中点击了结束测量按钮 则显示提示框
#define CODE_POWER_DOWN                0x01     //测量过程中点击了结束测量按钮 则显示提示框

#define DIALOG_X0                      110
#define DIALOG_Y0                      110
#define DIALOG_X1                      690
#define DIALOG_Y1                      340




void Error_popup(unsigned char Error_code);
unsigned char Warning_popup(unsigned char Warning_code);

#endif