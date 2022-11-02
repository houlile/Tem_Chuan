#include "UART.h"
#include "stdio.h"

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
	while(!(USART1->SR&(1<<7)));//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void My_Uart_Init(uint32_t Bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    //TX口的初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //RX的初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;      //浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);
    //初始化USART
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_InitStructure.USART_BaudRate = Bound;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_InitStructure);//初始化
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启串口接收中断
    USART_Cmd(USART1,ENABLE);
}
//发送一个字节
void Usart_SendByte(USART_TypeDef *USARTx,uint8_t ch)
{
    USART_SendData(USARTx,ch);
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
}
//发送8位数组
void Usart_SendArray(USART_TypeDef *USARTx,uint8_t *arry,uint16_t num)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        Usart_SendByte(USARTx,arry[i]);
    }
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}
void Usart_SendByts(USART_TypeDef *USARTx,char *p)
{
    while(*p != '\0')
    {
        Usart_SendByte(USARTx,*p);
        p++;
    }
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == SET);
}
void Up_Com_data(uint16_t data1,uint16_t data2,uint16_t data3,uint16_t data4)
{
    u8 i = 0;
    u8 ac = 0;
    u8 sc = 0;
    u8 data_send[100];
    u8 _cnt = 0;
    data_send[_cnt++] = 0xAA;
    data_send[_cnt++] = 0xFF;
    // data_send[_cnt++] = 0xAF;
    data_send[_cnt++] = 0xF1;
    data_send[_cnt++] = 8;
    data_send[_cnt++] = byte1(data1);
    data_send[_cnt++] = byte2(data1);

    data_send[_cnt++] = byte1(data2);
    data_send[_cnt++] = byte2(data2);
    data_send[_cnt++] = byte1(data3);
    data_send[_cnt++] = byte2(data3);

    data_send[_cnt++] = byte1(data4);
    data_send[_cnt++] = byte2(data4);
    // data_send[4] = _cnt - 5;
    for(i = 0; i<data_send[3]+4; i++)
    {
        sc += data_send[i];
        ac += sc;
    }
    data_send[_cnt++] = sc;
    data_send[_cnt++] = ac;
    Usart_SendArray(USART2,data_send,_cnt);
}

