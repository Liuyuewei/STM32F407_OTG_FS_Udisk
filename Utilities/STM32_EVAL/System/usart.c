#include "usart.h"	  
  
 
//////////////////////////////////////////////////////////////////
// //加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); 
    USART_SendData(USART3,(uint8_t)ch);   
	return ch;
}
#endif 

/*使用microLib的方法*/
  
// int fputc(int ch, FILE *f)
// {
// 	USART_SendData(USART1, (uint8_t) ch);

// 	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
//    
//     return ch;
// }
// int GetKey (void)  { 

//     while (!(USART1->SR & USART_FLAG_RXNE));

//     return ((int)(USART1->DR & 0x1FF));
// }

 
  

//初始化IO 串口1 
//bound:波特率
void USART3_Init(uint32_t bound){
    //GPIO端口设置
    GPIO_InitTypeDef  GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef  NVIC_InitStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART1，
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//GPIOD时钟
		USART_DeInit(USART3);  //复位串口1
	 
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //USART3_TX   PD.8
  	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure); //初始化PD8
		
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//USART3_RX	  PD.9
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PD9


   //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART3, &USART_InitStructure); //初始化串口
#if EN_USART3_RX		  //如果使能了接收  
   //Usart3 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		 
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(USART3, ENABLE);                    //使能串口 

}





#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
				Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
				USART_SendData(USART3,(uint8_t) Res);
			
		
// 		if((USART_RX_STA&0x8000)==0)//接收未完成
// 			{
// 			if(USART_RX_STA&0x4000)//接收到了0x0d
// 				{
// 				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
// 				else USART_RX_STA|=0x8000;	//接收完成了 
// 				}
// 			else //还没收到0X0D
// 				{	
// 				if(Res==0x0d)USART_RX_STA|=0x4000;
// 				else
// 					{
// 					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
// 					}		 
// 				}
// 			}   		 
     } 
} 
#endif	

