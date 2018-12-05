#include "Uart_communication.h"
#include "string.h"


volatile int mcu_count = 0;
unsigned char rcv_mcu_buf[260];
unsigned char mcu_commu_sucess = 0;
/***************************************************************************
*��������delay_1us()
*������void
*����ֵ:void
*���ܣ�ʹ��ϵͳ����ʱ�Ӿ�׼��ʱ1��΢�뺯��
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
*��������delay_nus()
*������u32 n  ʱ����ʱ��΢����
*����ֵ:void
*���ܣ�ʹ��ϵͳ����ʱ�Ӿ�׼��ʱn��΢�뺯��
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
*��������delay_1ms()
*������void
*����ֵ:void
*���ܣ�ʹ��ϵͳ����ʱ�Ӿ�׼��ʱ1�����뺯��
*****************************************************************************/
void delay_1ms(void)
{
//     TimingDelay=1000;
// 	while(TimingDelay!=0);
  delay_nus(1000);
}
/***************************************************************************
*��������delay_nms()
*������u32 n  ʱ����ʱ�ĺ�����
*����ֵ:void
*���ܣ�ϵͳ����ʱ�ӳ�ʼ������
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

/*******************************************************************************
* Function Name  : ���������
* Description    : 
* Input          : SourceByte_pt����Ҫ������������
                   CheckByteNum:  ���鳤��
* Output         : None
* Return         : �����
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
* Function Name  : ��UART5�������
* Description    : 
* Input          : msg_string����Ҫ���д��������
                   CheckByteNum:  ���鳤��
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

//��ȡ��������
unsigned int uart5_read()
{
	delay_nms(1000);
	//�Ӵ��ڽ��յ�����
	if(mcu_count > 0)
	{
		return mcu_count;
	}
	else
	{
		return 0;
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

void send_test_command(void)
{
	unsigned char send_cmd[1] = {0};
	send_cmd[0] = S_START;
	mcu_count = 0;
	memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
	uart5_write(send_cmd,1);
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
	unsigned char S_W_STATE = S_W_CMD, write_err_num = 0;
	
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
		while(mcu_count == 0)              //δ���յ��ӻ����͵�ACK��NACK���� �����·��������ֽ�һ������ ����ʧ�� �򱨴�
		{
			delay_nms(1000);
			write_err_num++;
			if(write_err_num == 10)
			{
				break;
			}
		}
		
		if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //�յ�ACK��������� ����ִ�в��� ����
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

	while(mcu_count == 0)              //δ���յ��ӻ����͵�ACK��NACK���� �����·��������ֽ�һ������ ����ʧ�� �򱨴�
	{
		delay_nms(1000);
		write_err_num++;
		if(write_err_num == 10)
		{
			//δ�յ�Ӧ���򷵻�0
			return 0;
		}
	}
	if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //�յ�ACK��������� ����ִ�в��� ����
	{
		mcu_count = 0;
		memset(rcv_mcu_buf,0,sizeof(rcv_mcu_buf));
		uart5_write(send_cmd,2);
		delay_nms(1000);
		uart5_write(&send_cmd[2],1);
		write_err_num = 0;
		while(mcu_count == 0)              //δ���յ��ӻ����͵�ACK��NACK���� �����·��������ֽ�һ������ ����ʧ�� �򱨴�
		{
			delay_nms(5000);
			write_err_num++;
			if(write_err_num == 20)
			{
				//δ�յ�Ӧ���򷵻�0
				return 0;
			}
		}
		if( (mcu_count != 0) && (rcv_mcu_buf[0] == S_ACK) )     //�յ�ACK��������� ����ִ�в��� ����
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