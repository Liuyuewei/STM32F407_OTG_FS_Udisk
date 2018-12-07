
#include "fat.h"
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include <stdbool.h>
#include <stdint.h>
#include "Uart_communication.h"
#include "flash_if.h"
#define NR_FILE1 32
extern uint32_t flashdestination, ramsource;
extern int cout;
static MSDOS_INFO12    Fat12_Info;
unsigned char*       Fat12_Temp;
static MSDOS_DIR    Fat12_RootDir[2];
static FILE1 OpenFILE1[NR_FILE1];
extern char STORAGE_Read (unsigned char lun, unsigned char *buf,  unsigned int blk_addr,unsigned short blk_len);
bool fpend_flag=0;
//---------------------------------------------------------------------------------------------------------------------
//
//  从MS FAT12表中查找某一簇的下一簇(有待研究）
//
//---------------------------------------------------------------------------------------------------------------------
unsigned short Msdos_12Next( unsigned long cluster)
{
	unsigned int blk, next, offset;unsigned char hb,lb;
  unsigned char  buf[512];
	if(cluster > Fat12_Info.fat_entries)
	;
	blk = ((cluster*3)) / 1024;
	offset = ((cluster%1024)*3/2)%512;
 	STORAGE_Read(0,  buf,  Fat12_Info.fat_base+blk,1);
	hb=buf[offset];
	if((offset ) == 511)
	{

		 	STORAGE_Read (0, buf, Fat12_Info.fat_base+blk+1,1);
 		  lb = buf[0];

	}
	else
	{
     lb = buf[offset + 1];

  }


  if(cluster%2==0)
	{
				next = ( ((lb&0x0F)<<8) + (hb) );
	}else
	{
				next = ( (lb<<4) + (hb>>4) );
	}
	return next;
}
//---------------------------------------------------------------------------------------------------------------------
//
//  簇号转扇区号
//
//---------------------------------------------------------------------------------------------------------------------
int Fat12_ToSec(int cluster)
{
	if(cluster > Fat12_Info.fat_entries)
	;

	if ( cluster > 1 )
		return Fat12_Info.blk_base+(cluster-2)*Fat12_Info.cluster_size;

	return Fat12_Info.fat_root;  //root sector
}

//---------------------------------------------------------------------------------------------------------------------
//
//  读取软盘根目录
//
//---------------------------------------------------------------------------------------------------------------------
void LoadRootDir(void)
{
	int i;
	unsigned char* p=NULL;
	p=(unsigned char*)Fat12_RootDir;
	for(i=0;i<2;i++)
	{

		  	STORAGE_Read (0, p, Fat12_Info.fat_root+i,1);
		    p+=512;
	}

}

//---------------------------------------------------------------------------------------------------------------------
//
//  Fat12 文件名比较
//
//---------------------------------------------------------------------------------------------------------------------
int Fat12_Strcmp(const char* des_FILE1name,const char* src_FILE1name)
{
	int i=0;

	while(i<8) 
	{
			if(des_FILE1name[i]!=src_FILE1name[i]) return 0;
			i++;
	}
	return 1;
}


void PrintsFat12info(void);
//---------------------------------------------------------------------------------------------------------------------
//
//  Fat12 文件系统初始化
//
//---------------------------------------------------------------------------------------------------------------------
void fat12_init(void)
{
	unsigned char buffer[512];
	MSDOS_SUPER12* sp;
	memset(buffer,0,512);
	STORAGE_Read (0,  buffer,  0, 1);
	sp = (MSDOS_SUPER12*)buffer;
	Fat12_Info.dos_sp.fats = sp->fats;
	Fat12_Info.cluster_size = sp->cluster_size;      //cluster size
	Fat12_Info.blk_size = sp->cluster_size*512;      //every block size.
	Fat12_Info.dos_sp.hidden = sp->hidden;           //unused
	Fat12_Info.dos_sp.reserved =  sp->reserved;

	Fat12_Info.dos_sp.fat16_length = sp->fat16_length;  //fat size fat 表所占扇区数
	strcpy((char*)Fat12_Info.dos_sp.oem, (char*)sp->oem);

	Fat12_Info.sector_size = sp->sector_size[0] + (sp->sector_size[1]<<8);
	Fat12_Info.dir_entries = sp->dir_entries[0] + (sp->dir_entries[1]<<8);

	Fat12_Info.fat_size = sp->fat16_length;
	Fat12_Info.flag = FAT12;
	Fat12_Info.fat_entries = sp->fat16_length*512*2/3;
	Fat12_Info.fat_root =  sp->fat16_length*sp->fats + sp->reserved + sp->hidden;
	Fat12_Info.fat_base =  sp->reserved+sp->hidden+Fat12_Info.fs_base;              //Value is sector
	Fat12_Info.blk_base =  Fat12_Info.fat_root + 32;  //14 origin: (fat12_info.dir_entries*32)/512;
	LoadRootDir();

}

void BlockToHTS(unsigned long block, int *head, int *track, int *sector )
{
	*head = ( block % ( 18 * 2 ) ) /18;
	*track =  block / ( 18 * 2 );
	*sector = block % 18 + 1;
}
//更新程序时闪灯
void update_led(unsigned int  temp)
{
	temp ++;
	if(temp % 2)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_4); //关闭LED指示灯	LYW 20181203
	}
	else
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_4); //开启LED指示灯	LYW 20181203
	}
}
//---------------------------------------------------------------------------------------------------------------------
//
//  读取 Fat12 文件
//
//---------------------------------------------------------------------------------------------------------------------

int fat12_read(FILE1*fp, void *buff, unsigned long __sb,bool flag)
{

	unsigned long copied = 0;
	unsigned long SECTOR, offset, read_size;
	unsigned long size;
	unsigned int  nr_blk=0;
	unsigned int  mcu_page_num = 0;                   //从机已经写了多少个256 byte个数据
	unsigned int  send_length = 0, i = 0;
	unsigned char  result = 0;							//返回值判断
	unsigned int  temp = 0,temp_led = 0;
	unsigned char read_temp[256] = {0};
	
	if(fp->current_pos >= fp->f_size)
	{
		return 0;
	}

	if((size = __sb) == 0)
	{
		return 0;
	}
	
	while (size>0)
	{	
		if(flag == 0)
		{			
			temp ++;
			if(temp >= 50)
			{
				temp = 0;
				update_led(temp_led ++);
			}
		}
		else
		{
			update_led(temp_led ++);
		}
		offset = fp->current_pos % 512;
		SECTOR = Fat12_ToSec(fp->current_blk);
		if(fp->f_size <= fp->current_pos) 
		{
			break;
		}
		if( (fp->current_pos + size) > fp->f_size)
		{
			size = fp->f_size - fp->current_pos;
		}
		read_size = (size>=512) ? 512 : size;
		if(read_size == 512)
		{ 
			memset(buff,0,read_size);
			STORAGE_Read (0, (void *)buff, SECTOR,1);

			//主机升级程序
			if(flag==0)
			{	
				FLASH_If_Write(&flashdestination, (uint32_t*) buff, (uint16_t) (read_size/4)); 
			}
			
			//从机升级程序
			else if(flag ==1)
			{
				if(mcu_page_num == 0)                    //首次写时进行一次全页擦除已写数据
				{
					result = flash_erase_all();
				}
				Write_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),(uint8_t*) buff,256);
				Read_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),(uint8_t*) read_temp,256);
				stm32isp_verify((unsigned char*)buff,(unsigned char*)read_temp,256);
				mcu_page_num++;
				Write_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),((uint8_t*) buff + 256),256);
				mcu_page_num++;
			}			
		}  	   
		else
		{  

			memset(buff,0,read_size);
			STORAGE_Read (0, (void *)buff, SECTOR,1);
		

			if(flag==0)
			{	
				FLASH_If_Write(&flashdestination, (uint32_t*) buff, (uint16_t) (read_size/4)); 
			}
			else if(flag ==1)
			{
				send_length = (read_size / 4) * 4;           //对从机flash进行写操作时每次必须是4的倍数
				for(i = read_size; i < send_length; i++)     //填充为0
				{
					*((uint8_t*) buff + i) = 0x00;
				}
				if(send_length < 257)
				{
					Write_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),(uint8_t*) buff,send_length);
				}
				else
				{
					Write_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),(uint8_t*) buff,256);
					mcu_page_num++;
					Write_mcu_flash((MCU_START_FLASH + mcu_page_num * 256),(uint8_t*) buff,(send_length - 256));
				}
			}		 
				offset = 0; 
		}
		copied += read_size;
		fp->current_pos += read_size;
		size -= read_size;
		if(!(fp->current_pos%512))
		{
			fp->current_blk = Msdos_12Next(fp->current_blk);  //read next cluster
		}
		nr_blk++;
		if(fp->current_blk >= EOF_FAT12 || fp->f_size <= fp->current_pos)
		{	
			break;
		}
	}
	return copied;
}


//---------------------------------------------------------------------------------------------------------------------
//
//  获得文件控制块
//
//---------------------------------------------------------------------------------------------------------------------
FILE1* GetFCB(void)
{
  FILE1* temp;

	for (temp = &OpenFILE1[0]; temp<&OpenFILE1[NR_FILE1]; temp++)
	{
		if (temp->f_mode == UNUSED)
		return temp;
	}

	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
//
// 
//读文件
//---------------------------------------------------------------------------------------------------------------------
FILE1* fat12_openFILE1(char* FILE1name)
{
	FILE1* fp;

	MSDOS_DIR* p=NULL;
	fp=GetFCB();
	p=Fat12_RootDir;

	while(p->FILE1_name[0]!=0x00)
	{
		if(Fat12_Strcmp(FILE1name,(char*)p->FILE1_name))		
		{  
			fp->current_blk=p->first_cluster;
			fp->current_pos=0;
			fp->f_size=p->FILE1_size;
			return fp;
			
		}
		p++;
		
	}

	return NULL;

}


