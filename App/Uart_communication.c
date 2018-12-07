#include "Uart_communication.h"
#include "string.h"

//串口接收到的数据
volatile int mcu_count = 0;
//串口接收数据缓冲区
unsigned char rcv_mcu_buf[260];
//串口通信成功
unsigned char mcu_commu_sucess = 0;
//存放从机相关信息
typedef struct {
    unsigned char bootloaderversion;    //1字节，bootloader版本
    unsigned char cmd_count;    //支持的所有指令个数
    unsigned char cmd[16];      //支持的所有指令
    unsigned int PID;       //product ID
} stm32info_t;
//定义结构体变量
stm32info_t stm32info;

/***************************************************************************
*函数名：delay_1us()
*参数：void
*返回值:void
*功能：使用系统节拍时钟精准延时1个微秒函数
*****************************************************************************/
void delay_1us(void)
{
	int i;
	for(i = 0; i < 5; i++)
	{
		__NOP();                      //12MHz,1us
	}
}
/***************************************************************************
*函数名：delay_nus()
*参数：u32 n  时钟延时的微秒数
*返回值:void
*功能：使用系统节拍时钟精准延时n个微秒函数
*****************************************************************************/
void delay_nus(u32 n)
{
// 	TimingDelay=n;
// 	while(TimingDelay!=0);
	while(n!=0)
	{
		delay_1us();
		n--;
	}
}
 /***************************************************************************
*函数名：delay_1ms()
*参数：void
*返回值:void
*功能：使用系统节拍时钟精准延时1个毫秒函数
*****************************************************************************/
void delay_1ms(void)
{
//     TimingDelay=1000;
// 	while(TimingDelay!=0);
  delay_nus(1000);
}
/***************************************************************************
*函数名：delay_nms()
*参数：u32 n  时钟延时的毫秒数
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void delay_nms(u32 n)
{
//   while(n--)delay_1ms();
  while(n!=0)
	{
		delay_1ms();
		n--;
	}
}

//等待从机应答
int waitACK()    //等待接收数据，第一个字节如果是0x79则认为ok返回1，如果是0x1f则认为失败返回0
{
    while(1)
	{
		delay_nms(1000);
        if(0 != mcu_count) 
		{
            if(0x79 == rcv_mcu_buf[0])
                return 1;
            else
                return 0;
        }
    }
}
/*******************************************************************************
* Function Name  : 产生异或结果
* Description    : 
* Input          : SourceByte_pt：需要进行异或的数组
                   CheckByteNum:  数组长度
* Output         : None
* Return         : 异或结果
*******************************************************************************/
unsigned char Generate_XorCheckByte(unsigned char *SourceByte_pt, unsigned int CheckByteNum)
{
	unsigned char	XorByte = 0x00;
	unsigned int	temp_counter	= 0x00;
	while(temp_counter < CheckByteNum)
		{
			XorByte	= XorByte ^ (* SourceByte_pt);
			SourceByte_pt ++;
			temp_counter ++;		
		}
	return(XorByte);
}


/*******************************************************************************
* Function Name  : 从UART5输出数据
* Description    : 
* Input          : msg_string：需要进行传输的数组
                   CheckByteNum:  数组长度
* Output         : None
* Return         : None
*******************************************************************************/
void uart5_write (unsigned char * msg_string,unsigned short size)
{
	unsigned short i;
	/* This loop reads in the text string and puts it in the UART 0 transmit buffer */
	for (i=0; i<size; i++)
	{	 
 		while(USART_GetFlagStatus(UART5,USART_FLAG_TC) == RESET) ;
		USART_SendData(UART5,msg_string[i]);
	}
}
void send_command(unsigned char cmd_byte)
{
	unsigned char XOR_BYTE = 0x00;
	unsigned char send_cmd[2] = {0};
	XOR_BYTE = (cmd_byte) ^ (0xFF);
	send_cmd[0] = cmd_byte;
	send_cmd[1] = XOR_BYTE;
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	uart5_write(send_cmd,2);
}
//从机进入自举模式
int stm32isp_sync()
{
    unsigned char send_command[1] = {S_START};
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	uart5_write(send_command,1);
    waitACK();
}
//获取从机版本号及命令
int get_ver_command()
{
    unsigned char get[32];
    int i = 1, j;
	send_command(S_GETALL);
    waitACK();
    //下面开始接收数据
    while(1) 
	{
		if(i == 16)
			break;
        get[i] = rcv_mcu_buf[i];     //读取数据，如果没有数据的话则会等待
        if(get[i] == 0x79)      //结束
            break;
        else if(get[i] == 0x1f)
            return -1;
        i++;
    }
    //数据处理
	//命令个数
	stm32info.cmd_count = get[1];
	//版本号
    stm32info.bootloaderversion = get[2];
    //存放各个命令
    for(j=0;j < stm32info.cmd_count;j++)
        stm32info.cmd[j] = get[j+3];
    return 1;
}
//获取从机ID
int get_ID_command()
{
    unsigned char get[16];
    int i = 1;
	send_command(S_GETID);
    waitACK();
	
    //接收数据
    while(1) 
	{
		if(i == 16)
			break;
        get[i] = rcv_mcu_buf[i];     //读取数据，如果没有数据的话则会等待
        if(get[i] == 0x79)           //结束
            break;
        else if(get[i] == 0x1f)
            return -1;
        i++;
    }
    stm32info.PID = (get[2] << 8);
    stm32info.PID += get[3];
 
    return 1;    
}

void send_adress(unsigned int ADRESS_U32)
{
	unsigned char send_cmd[5] = {0};

	send_cmd[0] = (ADRESS_U32 >> 24) & (0xFF);
	send_cmd[1] = (ADRESS_U32 >> 16) & (0xFF);
	send_cmd[2] = (ADRESS_U32 >>  8) & (0xFF);
	send_cmd[3] = (ADRESS_U32 & 0xFF);

	send_cmd[4] = Generate_XorCheckByte(send_cmd,4);

	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	uart5_write(send_cmd,5);
}


void send_data(unsigned char * data,unsigned int data_length)
{
	unsigned char send_cmd[260] = {0};
	unsigned int i = 0;

	send_cmd[0] = data_length - 1;

	for(i = 0;i < data_length;i++)
	{
		send_cmd[i+1] = data[i];
	}
    
	send_cmd[i+1] = Generate_XorCheckByte(send_cmd,(data_length + 1));
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	uart5_write(send_cmd,(data_length + 2));
}



unsigned char Write_mcu_flash(unsigned int flash_address,unsigned char * write_data,unsigned int write_length)
{
	unsigned char S_W_STATE = S_W_CMD,write_err_num = 0;
	
	do
	{
		switch (S_W_STATE)
		{
			case (S_W_CMD):
			{
				send_command(S_WRITE);
				break;
			}
			case (S_W_ADD):
			{
				send_adress(flash_address);
				break;
			}
			case (S_W_NUM):
			{
				send_data(write_data,write_length);
				break;
			}
			default :
			{
				break;
			}
		}
		
		write_err_num = 0;
		while(mcu_count == 0)              //未接收到从机发送的ACK或NACK命令 则重新发送命令字节一定次数 若仍失败 则报错
		{
			delay_nms(1000);
			write_err_num++;
			if(write_err_num == 10)
			{
				break;
			}
		}
		
		if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //收到ACK则继续进行 否则不执行操作 返回
		{
			switch (S_W_STATE)
			{
				case (S_W_CMD):
				{
					S_W_STATE = S_W_ADD;
					break;
				}
				case (S_W_ADD):
				{
					S_W_STATE = S_W_NUM;
					break;
				}
				case (S_W_NUM):
				{
					delay_nms(1000);
					return 1;
				}
				default :
				{
					break;
				}
			}
		}
		else
		{
			return 0;
		}
		
  }
	while(1);
}
int Read_mcu_flash(unsigned int flash_address,unsigned char * read_data,unsigned int read_length)
{
    unsigned char temp[2]; 
	temp[0] = read_length - 1;
	temp[1] = ~temp[0];

	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	//发送读取命令
	send_command(S_READ);
	waitACK();
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	send_adress(flash_address);
	waitACK();
	
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	//发送读取的字节数
	uart5_write(&temp[0],1);
	//发送读取的字节数
	uart5_write(&temp[1],1);
	while(1)
	{
        if(0 != mcu_count) 
		{
            if(0x79 == rcv_mcu_buf[0])
                return 1;
            else
                return 0;
        }
    }
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
    //下面接收数据
    for(int i=0;i<read_length;i++)
        read_data[i] = rcv_mcu_buf[i];
    return 1;
}

void send_erase_data(void)
{
	unsigned char send_cmd[3] = {0};
  send_cmd[0] = 0xff;
  send_cmd[1] = 0xff;
  send_cmd[2] = 0x00;
  mcu_count = 0;
  memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
  uart5_write(send_cmd,2);
  delay_nms(1000);
  uart5_write(&send_cmd[2],1);
}

unsigned char flash_erase_all(void)
{
	unsigned char write_err_num = 0;
	unsigned char send_cmd[3] = {0};
	send_cmd[0] = 0xff;
	send_cmd[1] = 0xff;
	send_cmd[2] = 0x00;

	send_command(S_ERASE1);

	write_err_num = 0;

	while(mcu_count == 0)              //未接收到从机发送的ACK或NACK命令 则重新发送命令字节一定次数 若仍失败 则报错
	{
		delay_nms(1000);
		write_err_num++;
		if(write_err_num == 10)
		{
			//未收到应答则返回0
			return 0;
		}
	}
	if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //收到ACK则继续进行 否则不执行操作 返回
	{
		mcu_count = 0;
		memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
		uart5_write(send_cmd,2);
		delay_nms(1000);
		uart5_write(&send_cmd[2],1);
		write_err_num = 0;
		while(mcu_count == 0)              //未接收到从机发送的ACK或NACK命令 则重新发送命令字节一定次数 若仍失败 则报错
		{
			delay_nms(5000);
			write_err_num++;
			if(write_err_num == 20)
			{
				//未收到应答则返回0
				return 0;
			}
		}
		if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //收到ACK则继续进行 否则不执行操作 返回
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
int stm32isp_verify(unsigned char * write_buffer,unsigned char * read_buffer,int len)
{
	for(int i=0;i<len;i++)
	{
		if(write_buffer[i] != read_buffer[i])
		return 0;
		else
		return 1;
	}
}   