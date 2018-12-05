/**
* @file          udisk.h
* @brief         disk module
* @author        fe.m
* @version       1.0
* @date          2012/08/20
*                
**/

#ifndef __U_DISK__
#define __U_DISK__
#include "stm32f4xx.h"

/*----------------------------------------------------------------------------*/
/*  Headers                                                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*  Constant Definition                                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*  Type Definition                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*  Global Function Prototypes                                                */
/*----------------------------------------------------------------------------*/
/* format disk */
extern void disk_format(void);
extern void DiskInit(void);
#define MAL_OK   0
#define MAL_FAIL 1
#define MAX_LUN  1
/* read data from disk */
//extern uint8_t disk_read_logical_block(uint8_t lun, uint32_t lba, const uint8_t *addr);

/* write data to disk */
//extern uint8_t disk_write_logical_block(uint8_t lun, uint32_t lba, const uint8_t *addr);

/* get logical block information */
//extern uint8_t disk_get_logical_block_info(uint8_t lun, USBF_MSC_ST_LUN_DATA *lun_data_ptr);

/* process disk event */
extern uint8_t disk_event(uint8_t lun, uint16_t event_type, void* event_info);

#endif
