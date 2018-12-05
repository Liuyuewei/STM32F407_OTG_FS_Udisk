/**
  ******************************************************************************
  * @file    usbd_storage_template.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_mem.h"
#include "flash_if.h"
#include "udisk.h"
#include "stm32f4xx_flash.h"
#include "bsp_nand_flash.h"
#include <stdbool.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define FLASH_START_ADDR	0x08003000	// Flash start address
#define FLASH_SIZE			0xD000
#define FLASH_PAGE_SIZE		0x400		// 1K per page
#define FLASH_WAIT_TIMEOUT	100000
#define STORAGE_LUN_NBR     1			//支持一个设备        

extern unsigned char TxBuffer [NAND_PAGE_SIZE];
extern unsigned char RxBuffer [NAND_PAGE_SIZE];

int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, 
                           uint32_t *block_num, 
                           uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);

/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', 'F', '4', '0', '7', ' ', /* Manufacturer : 8 bytes */
  'M', 'Y', 'W', 'L', ' ', '0', '_', '0', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 

USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
  
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;
/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
extern  NAND_ADDRESS WriteReadAddr;
int8_t STORAGE_Init (uint8_t lun)
{
    
  return (0);
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#define BLOCK_SIZE              512
#define BLOCK_NUM               2095
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
  *block_size = BLOCK_SIZE;  // 
  *block_num =  BLOCK_NUM;  
  return (0); 
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsReady (uint8_t lun)
{
  return (0);
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  return  0;
}
extern NAND_ADDRESS Bad_block[80];
/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{
static bool flag=0;
  WriteReadAddr.Zone = (2000+blk_addr)/1024;
	WriteReadAddr.Block = (2000+blk_addr)%1024;
	WriteReadAddr.Page = 0;
	if(2000+blk_addr>=4096)
	return MAL_OK;		
  FSMC_NAND_ReadSmallPage(buf, WriteReadAddr, blk_len);
  return MAL_OK;
}
/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
  WriteReadAddr.Zone = (2000+blk_addr)/1024;
	WriteReadAddr.Block = (2000+blk_addr)%1024;
	WriteReadAddr.Page = 0;
	if(2000+blk_addr>=4096)
	return MAL_OK;	
	while(FSMC_NAND_EraseBlock(WriteReadAddr)!=NAND_READY);
	FSMC_NAND_WriteSmallPage(buf, WriteReadAddr, blk_len);
  return MAL_OK;

}
/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

