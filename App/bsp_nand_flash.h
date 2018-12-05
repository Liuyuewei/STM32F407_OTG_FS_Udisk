/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fsmc_nand.h
** Descriptions:            The FSMC NAND application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-16
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __FSMC_NAND_H
#define __FSMC_NAND_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>

extern unsigned int bad_block[80];
extern unsigned char b_invalid_block;
extern volatile unsigned int g_bad_block_count;
extern unsigned char ZeroBuf[8];

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t Maker_ID;
  uint8_t Device_ID;
  uint8_t Third_ID;
  uint8_t Fourth_ID;
}
NAND_IDTypeDef;

typedef struct 
{
  int16_t Zone;
  int16_t Block;
  int16_t Page;
} 
NAND_ADDRESS;

struct _Save_Data_Struct
{
	unsigned char patient_id[32];
	char patient_name[32];
	unsigned char test_name[16];	
	unsigned char lot_id[16];
	unsigned char result1[48];
	unsigned char date[12];
	unsigned char time[8];
	unsigned char sample_id[4];
	unsigned char run_no[8];
};
struct _Save_Date_Control_Struct
{
	unsigned char date[20];
	char test_name[20];
	unsigned char result[20];	
	unsigned char remarks[20];

};

/* Private define ------------------------------------------------------------*/

/* NAND Area definition  for STM3210E-EVAL Board RevD */
#define CMD_AREA                   (u32)(1<<17)  /* A16 = CLE  high */
#define ADDR_AREA                  (u32)(1<<16)  /* A17 = ALE high */

#define DATA_AREA                  ((u32)0x00000000) 

/* FSMC NAND memory command */
#define	NAND_CMD_AREA_A            ((u8)0x00)
#define	NAND_CMD_AREA_B            ((u8)0x01)
#define NAND_CMD_AREA_C            ((u8)0x50)
#define NAND_CMD_AREA_TRUE1        ((u8)0x30)

#define NAND_CMD_WRITE0            ((u8)0x80)
#define NAND_CMD_WRITE_TRUE1       ((u8)0x10)
	
#define NAND_CMD_ERASE0            ((u8)0x60)
#define NAND_CMD_ERASE1            ((u8)0xD0)  

#define NAND_CMD_READID            ((u8)0x90)	
#define NAND_CMD_STATUS            ((u8)0x70)
#define NAND_CMD_LOCK_STATUS       ((u8)0x7A)
#define NAND_CMD_RESET             ((u8)0xFF)

/* NAND memory status */
#define NAND_VALID_ADDRESS         ((u32)0x00000100)
#define NAND_INVALID_ADDRESS       ((u32)0x00000200)
#define NAND_TIMEOUT_ERROR         ((u32)0x00000400)
#define NAND_BUSY                  ((u32)0x00000000)
#define NAND_ERROR                 ((u32)0x00000001)
#define NAND_READY                 ((u32)0x00000040)

/* FSMC NAND memory parameters */
#define NAND_PAGE_SIZE             (((u16)0x0200)) /* 512 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((u16)0x0020) /* 32x512 bytes pages per block */
#define NAND_ZONE_SIZE             ((u16)0x0400) /* 1024 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((u16)0x0010) /* last 16 bytes as spare area */
#define NAND_MAX_ZONE              ((u16)0x0004) /* 4 zones of 1024 block */

/* FSMC NAND memory address computation */
#define ADDR_1st_CYCLE(ADDR)       (u8)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (u8)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern NAND_ADDRESS tmp_addr;
unsigned int my_memcpy(unsigned char *strDest,unsigned char *strSrc);
int getlineResult1(char *dest,char *rcv_buf);
int getlineResult2(char *dest,char *rcv_buf);
int getlineResult3(char *dest,char *rcv_buf);

void close_lcd_cs(void);
void enable_lcd_cs(void);
void close_nandflash_cs(void);
void enable_nandflash_cs(void);
void init_save_data( struct _Save_Data_Struct *pSave_data);
void FSMC_NAND_Init(void);
void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID);
uint32_t FSMC_NAND_WriteSmallPage(u8 *pBuffer, NAND_ADDRESS Address, u32 NumPageToWrite);
uint32_t FSMC_NAND_ReadSmallPage (u8 *pBuffer, NAND_ADDRESS Address, u32 NumPageToRead);
uint32_t FSMC_NAND_WriteSpareArea(u8 *pBuffer, NAND_ADDRESS Address, u32 NumSpareAreaTowrite);
uint32_t FSMC_NAND_ReadSpareArea(u8 *pBuffer, NAND_ADDRESS Address, u32 NumSpareAreaToRead);
uint32_t FSMC_NAND_EraseBlock(NAND_ADDRESS Address);
uint32_t FSMC_NAND_Reset(void);
uint32_t FSMC_NAND_GetStatus(void);
uint32_t FSMC_NAND_ReadStatus(void);
uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address);
void FSMC_NAND_Test(void);



#endif /* __FSMC_NAND_H */

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
