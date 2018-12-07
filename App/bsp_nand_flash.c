/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fsmc_nand.c
** Descriptions:            The FSMC Nand application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-12-30
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

/* Includes ------------------------------------------------------------------*/
#include "bsp_nand_flash.h"
#include "stm32f4xx_fsmc.h"
volatile unsigned char b_nandflash_failed_flag = 0;
unsigned char ptmp_bad_buf[12]={0};
unsigned int bad_block[80]={0};
volatile unsigned int g_bad_block_count = 0;
/* Private define ------------------------------------------------------------*/
//#define NAND_FLASH_START_ADDR     ((uint32_t)0x70000000) //块2 NAND起始地址
#define Bank_NAND_ADDR     ((uint32_t)0x80000000) //块3 NAND起始地址

#define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)

/* Private variables ---------------------------------------------------------*/
static uint8_t TxBuffer [NAND_PAGE_SIZE];
static uint8_t RxBuffer [NAND_PAGE_SIZE];

/*******************************************************************************
* Function Name  : FSMC_NAND_Init
* Description    : Configures the FSMC and GPIOs to interface with the NAND memory.
*                  This function must be called before any write/read operation
*                  on the NAND.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DelayUs(uint32_t us)
{  
     u32 i = 1 * us;
     while(i --); 
}

void close_lcd_cs(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	/* Configure MCO1 pin(PA8) in alternate function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_7);				   //PD7
}



void enable_nandflash_cs(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	/* Configure MCO1 pin(PA8) in alternate function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG,GPIO_Pin_9);	
}


void FSMC_NAND_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructure; 

  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG , ENABLE);
  /*-- GPIO Configuration ------------------------------------------------------*/
  /* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);	
	
  /* D0->D3,*/
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1 , GPIO_AF_FSMC);

   /* D4->D7 NAND pin configuration  */ 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

  /*-- GPIO Configuration ------------------------------------------------------*/
  /* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */
	  /* D0->D3 NAND pin configuration  */  
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* D4->D7 NAND pin configuration  */  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
 //FSMC_A16,FSMC_A17 CSF：PD11，PD12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
 
 //NWE , NCE2,FSMC_NWAIT:PD4,PD5,PD6
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5 , GPIO_AF_FSMC); 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);     

//	FSMC_NCE3:PG9
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC); 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure); 
	
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6 , GPIO_AF_FSMC);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOD, &GPIO_InitStructure);	
//   	  /* CS->PD13 */
  /*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_SetupTime = 0xf1;
  p.FSMC_WaitSetupTime = 0xf3;
  p.FSMC_HoldSetupTime = 0xf2;
  p.FSMC_HiZSetupTime = 0xf1;

//   p.FSMC_SetupTime = 0x1;
//   p.FSMC_WaitSetupTime = 0x3;
//   p.FSMC_HoldSetupTime = 0x2;
//   p.FSMC_HiZSetupTime = 0x1;

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;//  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0xa1;//0x00;//0xa1;
  FSMC_NANDInitStructure.FSMC_TARSetupTime =  0x15;//0x00;//0x15;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);
  /* FSMC NAND Bank Cmd Test */
  FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);//  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}

#if 0
void FSMC_NAND_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructure; 

  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG , ENABLE);



  /*-- GPIO Configuration ------------------------------------------------------*/
  /* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);	
	
  /* D0->D3,*/
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1 , GPIO_AF_FSMC);

   /* D4->D7 NAND pin configuration  */ 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);



  /*-- GPIO Configuration ------------------------------------------------------*/
  /* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */
	  /* D0->D3 NAND pin configuration  */  
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* D4->D7 NAND pin configuration  */  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  

 //FSMC_A16,FSMC_A17 CSF：PD11，PD12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
 
 //NWE , NOE,FSMC_NWAIT:PD4,PD5,PD6
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5 , GPIO_AF_FSMC); 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6 , GPIO_AF_FSMC);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOD, &GPIO_InitStructure);    

//	FSMC_NCE3:PG9
 /*
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC); 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure); 
*/	
	/* CS->PG9 */
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
	

  /*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_SetupTime = 0xf1;
  p.FSMC_WaitSetupTime = 0xf3;
  p.FSMC_HoldSetupTime = 0xf2;
  p.FSMC_HiZSetupTime = 0xf1;

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;//  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0xa1;
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x15;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  /* FSMC NAND Bank Cmd Test */
  FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);//  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);


}
#endif


/******************************************************************************
* Function Name  : FSMC_NAND_ReadID
* Description    : Reads NAND memory's ID.
* Input          : - NAND_ID: pointer to a NAND_IDTypeDef structure which will hold
*                    the Manufacturer and Device ID.
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
  u32 data = 0;

  /* Send Command to the command area */ 	
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = 0x90;
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;

   /* Sequence to read ID from NAND flash */	
   data = *(vu32 *)(Bank_NAND_ADDR | DATA_AREA);

   NAND_ID->Maker_ID   = ADDR_1st_CYCLE (data);
   NAND_ID->Device_ID  = ADDR_2nd_CYCLE (data);
   NAND_ID->Third_ID   = ADDR_3rd_CYCLE (data);
   NAND_ID->Fourth_ID  = ADDR_4th_CYCLE (data);  
}

/******************************************************************************
* Function Name  : FSMC_NAND_WriteSmallPage
* Description    : This routine is for writing one or several 512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer containing data to be written   
*                  - Address: First page address
*                  - NumPageToWrite: Number of page to write  
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation 
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
*******************************************************************************/
u32 FSMC_NAND_WriteSmallPage(u8 *pBuffer, NAND_ADDRESS Address, u32 NumPageToWrite)
{
  u32 index = 0x00, numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
  u32 status = NAND_READY, size = 0x00;

  while((NumPageToWrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /* Page write command and address */
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE0;
	  DelayUs(1000);
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);  

    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

    /* Write data */
    for(; index < size; index++)
    {
      *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = pBuffer[index];
    }
    
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;


    /* Check status for successful operation */
    status = FSMC_NAND_GetStatus();
    
    if(status == NAND_READY)
    {
      numpagewritten++;

      NumPageToWrite--;

      /* Calculate Next small page Address */
      FSMC_NAND_AddressIncrement(&Address);  //addressstatus = FSMC_NAND_AddressIncrement(&Address);  
    }    
  }
  
  return (status);			//return (status | addressstatus);
}


/******************************************************************************
* Function Name  : FSMC_NAND_ReadSmallPage
* Description    : This routine is for sequential read from one or several 
*                  512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer to fill  
*                  - Address: First page address
*                  - NumPageToRead: Number of page to read
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation 
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
*******************************************************************************/
u32 FSMC_NAND_ReadSmallPage(u8 *pBuffer, NAND_ADDRESS Address, u32 NumPageToRead)
{
  u32 index = 0x00, numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
  u32 status = NAND_READY, size = 0x00;

  while((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {	   
    /* Page Read command and page address */
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A; 
   
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS); 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS); 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS); 
    
    //*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;
	

		DelayUs(1000);
    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);
	  DelayUs(1000);
 
    /* Get Data into Buffer */    
    for(; index < size; index++)
    {
      pBuffer[index]= *(vu8 *)(Bank_NAND_ADDR | DATA_AREA);
    }

    numpageread++;
    NumPageToRead--;

    /* Calculate page address */           			 
    //addressstatus = FSMC_NAND_AddressIncrement(&Address);
	FSMC_NAND_AddressIncrement(&Address);
  }

  status = FSMC_NAND_GetStatus();
  
  return (status);			  //return (status | addressstatus);
}

//在一页内读
u32 FSMC_NAND_Read(u8 *pBuffer, NAND_ADDRESS Address, u32 Num)
{
  u32 index = 0x00, numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
  u32 status = NAND_READY, size = 0x00;

  if((Num > 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {	   
    /* Page Read command and page address */
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A; 
   
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS); 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS); 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS); 
    
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;
	  DelayUs(1000); 

    /* Calculate the size */
    //size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);
	  DelayUs(1000);
    
    /* Get Data into Buffer */    
    for(; index < size; index++)
    {
      pBuffer[index]= *(vu8 *)(Bank_NAND_ADDR | DATA_AREA);
    }
/*
    numpageread++;  
    NumPageToRead--;
    // Calculate page address            			 
    addressstatus = FSMC_NAND_AddressIncrement(&Address);
*/
  }

  status = FSMC_NAND_GetStatus();
  
  return (status );
}



// /******************************************************************************
// * Function Name  : FSMC_NAND_WriteSpareArea
// * Description    : This routine write the spare area information for the specified 
// *                  pages addresses.
// * Input          : - pBuffer: pointer on the Buffer containing data to be written 
// *                  - Address: First page address
// *                  - NumSpareAreaTowrite: Number of Spare Area to write
// * Output         : None
// * Return         : New status of the NAND operation. This parameter can be:
// *                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
// *                     a Timeout error
// *                   - NAND_READY: when memory is ready for the next operation 
// *                  And the new status of the increment address operation. It can be:
// *                  - NAND_VALID_ADDRESS: When the new address is valid address
// *                  - NAND_INVALID_ADDRESS: When the new address is invalid address
// *******************************************************************************/

// u32 FSMC_NAND_WriteSpareArea(u8 *pBuffer, NAND_ADDRESS Address, u32 NumSpareAreaTowrite)
// {
//   u32 index = 0x00, numsparesreawritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
//   u32 status = NAND_READY, size = 0x00; 

//   while((NumSpareAreaTowrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
//   {
//     // Page write Spare area command and address 
//     *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_C;
//     *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE0;

//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS); 
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS); 
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS); 

//     // Calculate the size 
//     size = NAND_SPARE_AREA_SIZE + (NAND_SPARE_AREA_SIZE * numsparesreawritten);

//     // Write the data 
//     for(; index < size; index++)
//     {
//       *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = pBuffer[index];
//     }
//     
//     *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;

//     // Check status for successful operation 
//     status = FSMC_NAND_GetStatus();

//     if(status == NAND_READY)
//     {
// 		numsparesreawritten++;      
// 		NumSpareAreaTowrite--;  
// 		addressstatus = FSMC_NAND_AddressIncrement(&Address);
//     }       
//   }
//   
//   return (status | addressstatus);
// }
 
// /******************************************************************************
// * Function Name  : FSMC_NAND_ReadSpareArea
// * Description    : This routine read the spare area information from the specified
// *                  pages addresses.
// * Input          : - pBuffer: pointer on the Buffer to fill  
// *                  - Address: First page address
// *                  - NumSpareAreaToRead: Number of Spare Area to read
// * Output         : None
// * Return         : New status of the NAND operation. This parameter can be:
// *                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
// *                     a Timeout error
// *                   - NAND_READY: when memory is ready for the next operation 
// *                  And the new status of the increment address operation. It can be:
// *                  - NAND_VALID_ADDRESS: When the new address is valid address
// *                  - NAND_INVALID_ADDRESS: When the new address is invalid address
// *******************************************************************************/

// u32 FSMC_NAND_ReadSpareArea(u8 *pBuffer, NAND_ADDRESS Address, u32 NumSpareAreaToRead)
// {
//   u32 numsparearearead = 0x00, index = 0x00, addressstatus = NAND_VALID_ADDRESS;
//   u32 status = NAND_READY, size = 0x00;

//   while((NumSpareAreaToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
//   {     
//     // Page Read command and page address    
//     *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_C;

//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);     
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);     
//     *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);    

//     //*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;

//     // Data Read 
//     size = NAND_SPARE_AREA_SIZE +  (NAND_SPARE_AREA_SIZE * numsparearearead);
//     // Get Data into Buffer 
//     for ( ;index < size; index++)
//     {
//       pBuffer[index] = *(vu8 *)(Bank_NAND_ADDR | DATA_AREA);
//     }
//     
//     numsparearearead++;    
//     NumSpareAreaToRead--;
//     addressstatus = FSMC_NAND_AddressIncrement(&Address);
//   }

//   status = FSMC_NAND_GetStatus();
//   return (status | addressstatus);
// }

/******************************************************************************
* Function Name  : FSMC_NAND_EraseBlock
* Description    : This routine erase complete block from NAND FLASH
* Input          : - Address: Any address into block to be erased
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation 
*******************************************************************************/
u32 FSMC_NAND_EraseBlock(NAND_ADDRESS Address)
{
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
		
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE1; 

  return (FSMC_NAND_GetStatus());
}
  	
	
/******************************************************************************
* Function Name  : FSMC_NAND_Reset
* Description    : This routine reset the NAND FLASH
* Input          : None
* Output         : None
* Return         : NAND_READY
*******************************************************************************/
u32 FSMC_NAND_Reset(void)
{
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_RESET;

  return (NAND_READY);
}

/******************************************************************************
* Function Name  : FSMC_NAND_GetStatus
* Description    : Get the NAND operation status
* Input          : None
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate 
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation    
*******************************************************************************/
u32 FSMC_NAND_GetStatus(void)
{
  u32 timeout = 0x200, status = NAND_READY;			   //0x1000000

  status = FSMC_NAND_ReadStatus(); 

  /* Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = FSMC_NAND_ReadStatus();
     timeout --;      
  }

  if(timeout == 0x00)
  {          
    status =  NAND_TIMEOUT_ERROR;      
  } 

  /* Return the operation status */
  return (status);      
}
/******************************************************************************
* Function Name  : FSMC_NAND_ReadStatus
* Description    : Reads the NAND memory status using the Read status command 
* Input          : None
* Output         : None
* Return         : The status of the NAND memory. This parameter can be:
*                   - NAND_BUSY: when memory is busy
*                   - NAND_READY: when memory is ready for the next operation    
*                   - NAND_ERROR: when the previous operation gererates error   
*******************************************************************************/
u32 FSMC_NAND_ReadStatus(void)
{
  u32 data = 0x00, status = NAND_BUSY;

  /* Read status operation ------------------------------------ */
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(vu8 *)(Bank_NAND_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  } 
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY; 
  }
  
  return (status);
}

/******************************************************************************
* Function Name  : NAND_AddressIncrement
* Description    : Increment the NAND memory address
* Input          : - Address: address to be incremented.
* Output         : None
* Return         : The new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
*******************************************************************************/
u32 FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address)
{
  u32 status = NAND_VALID_ADDRESS;
 
  	//原来是每次增加一页，现改为每次增加1块
//   Address->Page += 32;
//  if(Address->Page == NAND_BLOCK_SIZE)
//  {
//    Address->Page = 0;
//    Address->Block++;
//    
//    if(Address->Block == NAND_ZONE_SIZE)
//    {
//      Address->Block = 0;
//      Address->Zone++;
//
//      if(Address->Zone == NAND_MAX_ZONE)
//      {
//        status = NAND_INVALID_ADDRESS;
//      }
//    }
//  } 

	Address->Block++;
	if(Address->Block == NAND_ZONE_SIZE)
    {
      Address->Block = 0;
      Address->Zone++;

      if(Address->Zone == NAND_MAX_ZONE)
      {
        status = NAND_INVALID_ADDRESS;
      }
    }

  
  return (status);
}


u32 FSMC_NAND_AddressDecrement(NAND_ADDRESS* Address)
{
  u32 status = NAND_VALID_ADDRESS;
   if(Address->Block > 0)
   {
   		Address->Block--;	
   }else if(Address->Block == 0)
   {
   		Address->Zone--;
		Address->Block = 1023;
		if(Address->Zone < 0)
		{
			status = NAND_INVALID_ADDRESS;
		}
   		
   } 
  
  return (status);
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
/******************************************************************************
* Function Name  : FSMC_NAND_Test
* Description    : NAND test
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
extern  NAND_ADDRESS WriteReadAddr;
void FSMC_NAND_Test(void)
{
  uint16_t index;
  uint16_t j,i;
  NAND_IDTypeDef NAND_ID;
 
	
	uint32_t flash_status = 0;

  FSMC_NANDDeInit(FSMC_Bank3_NAND); // FSMC_NANDDeInit(FSMC_Bank2_NAND); //CSF
  FSMC_NAND_Init();

	flash_status = FSMC_NAND_ReadStatus();
  //检测NAND Flash 
  FSMC_NAND_ReadID(&NAND_ID);
 // printf("Nand Flash ID = %02X,%02X,%02X,%02X  ",NAND_ID.Maker_ID, NAND_ID.Device_ID,
		 //                                        NAND_ID.Third_ID, NAND_ID.Fourth_ID );
  if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x15))
  {
	 //printf("Type = K9F1G08U0A\r\n");
  }
  else if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x00) && (NAND_ID.Fourth_ID == 0x95))
  {
	 //printf("Type = K9F1G08U0B\r\n");		
  }
  else if ((NAND_ID.Maker_ID == 0xAD) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x1D))				  
  {
	 //printf("Type = HY27UF081G2A\r\n");		
  }
	 else if ((NAND_ID.Maker_ID == 0x20) && (NAND_ID.Device_ID == 0x76))
//		&& (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x1D))
  {
	 //printf("Type = NAND512W3A2CN6E\r\n");		
  }
  else
  {
	 //printf("Type = Unknow\r\n");
  }
}  

/*
*********************************************************************************************************
*	函 数 名: NAND_Format
*	功能说明: NAND Flash格式化，擦除所有的数据，重建LUT
*	形    参:  无
*	返 回 值: NAND_OK : 成功； NAND_Fail ：失败（一般是坏块数量过多导致）
*********************************************************************************************************
*/
uint8_t NAND_Format(void)
{
	uint16_t i, n;
	uint16_t usGoodBlockCount;

	WriteReadAddr.Zone = 0;
	WriteReadAddr.Block = 0;
	WriteReadAddr.Page = 0;
	for (i = 0; i < 4096; i++)
	{

		FSMC_NAND_EraseBlock(WriteReadAddr);
		FSMC_NAND_AddressIncrement(&WriteReadAddr);		
	}
}
