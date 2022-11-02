#ifndef _UART_H_
#define _UART_H_

#define  byte1(data)      (*((char*)(&data)))
#define  byte2(data)      (*((char*)(&data)+1))
#define  byte3(data)      (*((char*)(&data)+2))
#define  byte4(data)      (*((char*)(&data)+3))     

#include "System.h"

void My_Uart_Init(uint32_t Bound);
void Usart_SendByts(USART_TypeDef *USARTx,char *p);
void Usart_SendByte(USART_TypeDef *USARTx,uint8_t ch);
void Usart_SendArray(USART_TypeDef *USARTx,uint8_t *arry,uint16_t num);
void Up_Com_data(uint16_t data1,uint16_t data2,uint16_t data3,uint16_t data4);

#endif
