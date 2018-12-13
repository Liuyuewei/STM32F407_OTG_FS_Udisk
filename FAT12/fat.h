#ifndef _FAT_H
#define _FAT_H

#include <stdbool.h>

#define NULL  0
#define FALSE 0
#define TRUE  1
//#define SUCCESS 1
#define UNUSED 0




#define FS_ATTRIBUTE_READ_WRITE 0x00
#define FS_ATTRIBUTE_READ_ONLY  0x01
#define FS_ATTRIBUTE_HIDE       0x02
#define FS_ATTRIBUTE_SYS        0x04
#define FS_ATTRIBUTE_VOLUME     0x08
#define FS_ATTRIBUTE_FOLD       0x10
#define FS_ATTRIBUTE_RECORD     0x20



struct inode 
	// This Struct Used by Directory
{
	int               i_dev;
	int               i_isdir;
	unsigned short    i_fs_type;
	unsigned long     i_number;
	unsigned long     i_father;
	unsigned int      i_mode; ///is FILE1 or dir
	unsigned short	  i_links_count;	/////////// Links count 
	unsigned short    i_count;
	unsigned short    i_size;

	unsigned long	    i_blocks;	/////////// 块数Blocks count 
	unsigned long	    ext2_block[15];/////////// Pointers to EXT2 blocks 
} ;

#define NULL_INODE (struct inode *) 0


typedef struct
{
	unsigned int         f_sys_nr;
	unsigned int         f_mode; ///读还是写
	unsigned long        f_size; ///读还是写
	unsigned long        start_blk;
	unsigned long        father_blk;
	unsigned long        current_blk;
	unsigned long        current_pos;
	unsigned long        current_hasbuf;
}FILE1;

#define NULL_FCB  (FILE1 *) 0

/*
struct super_block {
	int s_dev;
	unsigned char s_count;
	unsigned char s_dev_name;
	unsigned int  s_start_sector;
	unsigned long s_blocksize;
	unsigned short s_flag;
	unsigned char s_rd_only;
	struct inode* root;
	struct fs_operations *ops;	// FILE1 operations
};
*/














typedef unsigned char   u8_t;  //字节
typedef unsigned short u16_t;  //字
typedef unsigned int  u32_t;  //双字
typedef unsigned long long int u64_t;

#define FAT12_MAX    4078 // 12 位 FAT的最大串号 
#define FREE_FAT12   0x0

#define FAT12_MAGIC	 4085
#define FAT16_MAGIC	 (( unsigned ) 65526l )
#define FAT32_MAGIC	 268435455l

#define BAD_FAT12    0xff7
#define BAD_FAT16    0xfff7
#define BAD_FAT32    0xffffff7

#define EOF_FAT12    0xff8		// standard EOF 
#define EOF_FAT16    0xfff8
#define EOF_FAT32    0xffffff8
#define FILE1_NOT_FOUND  0xffffffff
#define FAT12_ROOT	 1

#define FAT12   0x12
#define FAT16   0x0e
#define FAT32   0x0b
#define EXT2    0x83
#define UJFS    0x82

#define PACKED __attribute__((packed))



#define NULL_FCB  (FILE1 *) 0


typedef struct
{
	signed char	jmp_boot[3];	// Boot strap short or near jump 
	signed char	oem[8];	      		// Name - can be used to special casepartition manager volumes 
	u8_t			sector_size[2];	// bytes per logical sector 
	u8_t			cluster_size;	      // sectors/cluster 
	u16_t		reserved;	          // reserved sectors 
	u8_t			fats;		            // number of FATs 
	u8_t			dir_entries[2];	    // root directory entries 
	u8_t			sectors[2];	        // number of sectors 
	u8_t			media_descriptor;		// media code (unused) 
	u16_t		fat16_length;	      // sectors/FAT 
	u16_t		sec_per_track;	    // sectors per track 
	u16_t		heads;		          // number of heads 
	u32_t		hidden;		          // hidden sectors (unused) 
	u32_t		total_sectors;	    // number of sectors (if sectors == 0) 

	// The following fields are only used by FAT12/16 
	u8_t		drv_type;		// 0x00 for floppy,0x80 for harddisks  
	u8_t		reserved1;		// reserved for WindowsNT ,that format FAT always set to 0x0 
	u8_t		boot_sig;	       // extended boot sig 0x29 
	u32_t	vol_id;			// volume serial number. 
	u8_t		vol_lab[11];		// volume label. 
	u16_t	FILE1_sys_type[8];	// one of the strings:"fat12","fat16",or"fat"
}MSDOS_SUPER12;

typedef MSDOS_SUPER12 MSDOS_SUPER16;


typedef struct
{
	signed char	jmp_boot[3];	// Boot strap short or near jump 
	signed char	oem[8];	      // Name - can be used to special casepartition manager volumes 
	u8_t			sector_size[2];	    // bytes per logical sector 
	u8_t			cluster_size;	      // sectors/cluster 
	u16_t		reserved;	          // reserved sectors 
	u8_t			fats;		            // number of FATs 
	u8_t			dir_entries[2];	    // root directory entries 
	u8_t			sectors[2];	        // number of sectors 
	u8_t			media_descriptor;		// media code (unused) 
	u16_t		fat16_length;	      // sectors/FAT 
	u16_t		sec_per_track;	    // sectors per track 
	u16_t		heads;		          // number of heads 
	u32_t		hidden;		          // hidden sectors (unused) 
	u32_t		total_sectors;	    // number of sectors (if sectors == 0) 

	// The following fields are only used by FAT32 
	u32_t	fat32_length;	      // sectors/FAT 
	u16_t	flags;		          // bit 8: fat mirroring, low 4: active fat 
	u8_t	version[2];	        // major, minor FILE1system version 
	u32_t	fat32_root_cluster;	// first cluster in root directory 
	u16_t	info_sector;	      // FILE1system info sector 
	u16_t	backup_boot;	      // backup boot sector 
	u16_t	unused[6];	        // Unused 
}MSDOS_SUPER32;

typedef struct
{
	 u16_t flag;
	 u16_t sector_size;
	 u8_t  cluster_size;
	 u32_t blk_size;
	 u32_t fat_size;
	 u16_t dir_entries;
	 u16_t fat_entries;
	 u32_t fat_base;
	 u32_t fat_root;
	 u32_t fs_base;
	 u32_t blk_base ;  //////数据区开始
	 MSDOS_SUPER12 dos_sp;

}MSDOS_INFO12;


typedef struct
{
	 u16_t flag;
	 u16_t sector_size;
	 u8_t  cluster_size;
	 u32_t blk_size;
	 u32_t fat_size;
	 u16_t dir_entries;
	 u16_t fat_entries;
	 u32_t fat_base;
	 u32_t fat_root;
	 u32_t fs_base;
	 u32_t blk_base ;  //////数据区开始
	 MSDOS_SUPER16 dos_sp;

}MSDOS_INFO16;

typedef struct
{
	 u16_t flag;
	 u16_t sector_size;
	 u8_t  cluster_size;
	 u32_t blk_size;
	 u32_t fat_size;
	 u16_t dir_entries;
	 u16_t fat_entries;
	 u32_t fat_base;
	 u32_t fat_root;
	 u32_t fs_base;
	 u32_t blk_base ;  //////数据区开始
	 MSDOS_SUPER32 dos_sp;

}MSDOS_INFO32;

typedef struct {
  unsigned sec:5;
  unsigned min:6;
  unsigned hour:5;
}__attribute__((packed)) DOS_TIME;
 

typedef struct  {
  unsigned day_of_month:5;
  unsigned month:4;
  unsigned year:7; /* since 1980 */
}__attribute__((packed)) DOS_DATE;

typedef struct
{
	u8_t FILE1_name[8];
	u8_t ext_name[3];	// name and extension 
	u8_t attribute;		// attribute bits 

	u8_t    lcase;		// Case for base and extension 
	u8_t	  ctime_ms;	// Creation time, milliseconds 
	u16_t	  ctime;		// Creation time 
	u16_t	  cdate;		// Creation date 
	u16_t	  adate;		// Last access date 
	u16_t   first_high;	// High 16 bits of cluster in FAT32 

	DOS_TIME	time;
	DOS_DATE date;
	u16_t first_cluster;// first cluster 
	u32_t	FILE1_size;		// FILE1 size (in bytes) 
}__attribute__((packed)) MSDOS_DIR;

typedef struct __fat
{
	unsigned fat_item:12;
}PACKED FAT12_TAG;

//typedef unsigned short FAT12_TAG;

#define MSDOS_READONLY      1                  // 只读
#define MSDOS_HIDDEN  2                        // 隐藏文件 
#define MSDOS_SYS_FILE1     4                   // 系统文件 
#define MSDOS_VOLUME  8                        // 卷标 
#define MSDOS_FLOD     16                      // 目录 
#define MSDOS_ARCH    32                       // 存档文件 

#define MSDOS_ISVOLUME(attribute)		(((attribute) &0x20) && ((attribute) & 0x08))
#define MSDOS_ISDIR(attribute)		  (((attribute) &0x10) && !((attribute) & 0x08))
#define MSDOS_ISREG(attribute)		  (!((attribute) & 0x08) && !((attribute) &0x10))

void  fat12_init(void);
//unsigned long Fat12_LoadFILE1(char* FILE1name,void **ptr);
//void  Fat12_LoadProgram(char* FILE1name,void* buf);
FILE1* fat12_openFILE1(char* FILE1name);
int fat12_read(FILE1*fp, void *buff, unsigned long __sb,bool flag);

int fat32_init(void);
int fat32_read(FILE1*fp, void*buff, int __sb);
FILE1* fat32_openFILE1(char* FILE1name);
void fat32_printrootdir(void);

void getfat32info(void);

void update_led(unsigned int  temp);

#endif



