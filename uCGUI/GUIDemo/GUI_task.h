#ifndef GUI_TADK_H

#define GUI_TADK_H


#include "stdlib.h"
#include "GUI.h"
#include "GUI_Test.h"
#include "GUI_Set.h"
#include "GUI_Control.h"
#include "GUI_History.h"
#include "GUI_Popup.h"
#include "GUI_X.h"
#include "touch.h"
#include "Print_init.h"

/*定义一些ID变量*/

/*FramewinDLG.c中用到的ID变量*/
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_BUTTON_6     (GUI_ID_USER + 0x02)
#define ID_TEXT_0       (GUI_ID_USER + 0x03)

/*WindowDLG.c中用到的ID变量*/
#define ID_WINDOW_0     (GUI_ID_USER + 0x0A)
#define ID_LISTVIEW_0   (GUI_ID_USER + 0x0D)
#define ID_BUTTON_1     (GUI_ID_USER + 0x0E)

/*Test.c中用到的ID变量*/
#define ID_WINDOW_1    (GUI_ID_USER + 0x10)
#define ID_BUTTON_2    (GUI_ID_USER + 0x1B)
#define ID_BUTTON_3    (GUI_ID_USER + 0x1C)
#define ID_BUTTON_4    (GUI_ID_USER + 0x1D)
#define ID_BUTTON_5    (GUI_ID_USER + 0x1E)
#define ID_IMAGE_0     (GUI_ID_USER + 0x1F)



#define ID_BUTTON_TEST        (GUI_ID_USER + 0x20)
#define ID_BUTTON_SET         (GUI_ID_USER + 0x21)
#define ID_BUTTON_CONTROL     (GUI_ID_USER + 0x22)
#define ID_BUTTON_HISTORY     (GUI_ID_USER + 0x23)


#define LENGTH_BUTTON_X        110
#define LENGTH_BUTTON_Y        30
#define LENGTH_BLOOD_X         23
#define LENGTH_BLOOD_Y         91
#define LENGTH_LEFT_X          680
#define LENGTH_LEFT_Y          440
#define LENGTH_RIGHT_X         120
#define LENGTH_RIGHT_Y         440
#define LENGTH_SERIAL_X        30
#define LENGTH_SERIAL_Y        18
#define LENGTH_COMPUTER_X      20
#define LENGTH_COMPUTER_Y      20
#define LENGTH_WINDOW_X        177
#define LENGTH_WINDOW_Y        112
#define LENGTH_LOGIN_X         800
#define LENGTH_LOGIN_Y         480
#define LENGTH_CARD_X          54
#define LENGTH_CARD_Y          78
#define LENGTH_ICON_WARING_X   80
#define LENGTH_ICON_WARING_Y   69
#define LENGTH_ICON_ERROR_X    80
#define LENGTH_ICON_ERROR_Y    69
#define LENGTH_ICON_DOWN_X     56
#define LENGTH_ICON_DOWN_Y     49
#define LENGTH_ICON_CHOICE_X   350
#define LENGTH_ICON_CHOICE_Y   44
#define LENGTH_ICON_CONFIRM_X  350
#define LENGTH_ICON_CONFIRM_Y  44
#define LENGTH_TEXT_BK_X       400
#define LENGTH_TEXT_BK_Y       200


#define ADRESS_LIS                       0x400018
#define ADRESS_UNLIS                     0x4019F0
#define ADRESS_BLOOD                     0x4033C8
#define ADRESS_TEST_LEFT                 0x404432
#define ADRESS_TEST_RIGHT1               0x4965C2
#define ADRESS_TEST_RIGHT2               0x4B0252
#define ADRESS_TEST_RIGHT3               0x4C9EE2
#define ADRESS_SERIAL                    0x4E3B72
#define ADRESS_UNSERIAL                  0x4E3FBA
#define ADRESS_COMPUTER                  0x4E4402
#define ADRESS_UNCOMPUTER                0x4E4732
#define ADRESS_CLOSE_WINDOW              0x4E4A62
#define ADRESS_LOGIN                     0x4EE552
#define ADRESS_CAED                      0x5A9D62
#define ADRESS_CONTROL_LEFT              0x5ABE5A
#define ADRESS_CONTROL_RIGHT1            0x63DFEA
#define ADRESS_CONTROL_RIGHT2            0x657C7A
#define ADRESS_ENGLISH                   0x67190A
#define ADRESS_POWER_DOWN                0x6732E2  
#define ADRESS_HISTORY_R                 0x674CBA
#define ADRESS_TEXT_BK                   0x68E94A
#define ADRESS_ICON_CONFIRM              0x6B5A5A
#define ADRESS_ICON_WARING               0x6BD2BA
#define ADRESS_ICON_CHOICE               0x6BFDEA
#define ADRESS_ICON_DOWN                 0x6C764A
#define ADRESS_CON_HIS_L                 0x6C8BCA
#define ADRESS_CON_HIS_R1                0x75AD5A
#define ADRESS_CON_HIS_R2                0x7749EA



#define ADRESS_EN_TEST_LEFT              0x78E67A
#define ADRESS_EN_TEST_RIGHT1            0x82080A
#define ADRESS_EN_TEST_RIGHT2            0x83A49A
#define ADRESS_EN_TEST_RIGHT3            0x85412A
#define ADRESS_EN_CONTROL_LEFT           0x86DDBA
#define ADRESS_EN_CONTROL_RIGHT1         0x8FFF4A
#define ADRESS_EN_CONTROL_RIGHT2         0x919BDA
#define ADRESS_EN_ENGLISH                0x93386A 
#define ADRESS_EN_POWER_DOWN             0x935242  
#define ADRESS_EN_HISTORY_R              0x936C1A
#define ADRESS_EN_ICON_CONFIRM           0x9508AA
#define ADRESS_EN_ICON_CHOICE            0x95810A
#define ADRESS_EN_CON_HIS_L              0x95F96A
#define ADRESS_EN_CON_HIS_R1             0x9F1AFA
#define ADRESS_EN_CON_HIS_R2             0xA0B78A

extern WM_HWIN KEY_BOARD_hItem;
extern unsigned char test_putin_flag;

#endif