#ifndef _I2C_H_
#define _I2C_H_

#include "System.h"

#define     I2C_SDA         PBout(7)
#define     I2C_SCL         PBout(6)
#define     I2C_READ_SDA    PBin(7)
//#define     SDA_IN()        {GPIOB->CRH &= 0XFFFFFFF0;GPIOB->CRH |= 8<<8;}
//#define     SDA_OUT()       {GPIOB->CRH &= 0XFFFFFFF0;GPIOB->CRH |= 3<<8;}
#define 		SDA_IN()  			{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 4<<28;}
#define 		SDA_OUT() 			{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 3<<28;}


void IIC_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_ACK(void);
void I2C_NACK(void);
uint8_t I2C_Wait_ACK(void);
void I2C_Send_Byte(uint8_t data);
uint8_t I2C_Read_Byte(uint8_t ack);


#endif

