#include "I2C.h"
#include "Delay.h"


void IIC_Init(void)
{
    // RCC->APB2ENR |= 1<<4;//使能时钟
    // GPIOB->CRH &= 0XFFFFFF00;//设置IO为推挽输出
    // GPIOB->CRH |= 0x00000033;
    // GPIOB->ODR |= 3<<8;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
}
void I2C_Start(void)
{
    SDA_OUT();//SDA设置为输出
    I2C_SDA = 1;
    I2C_SCL = 1;
    Delay_us(5);
    I2C_SDA = 0;//当SCL为高时，SDA由高变低
    Delay_us(5);
    I2C_SCL = 0;//开始发送或者接收数据

}
void I2C_Stop(void)
{
    SDA_OUT();
    I2C_SCL = 0;
    I2C_SDA = 0;
    Delay_us(5);
    I2C_SCL = 1;//当CLK为高时，SDA由低变高
    Delay_us(5);
    I2C_SDA = 1;//发送i2c总线结束信号
}
//产生ACK应答
void I2C_ACK(void)
{
    I2C_SCL = 0;
    SDA_OUT();
    I2C_SDA = 0;
    Delay_us(2);
    I2C_SCL = 1;
    Delay_us(2);
    I2C_SCL = 0;
}
void I2C_NACK(void)
{
    I2C_SCL = 0;
    SDA_OUT();
    I2C_SDA = 1;
    Delay_us(2);
    I2C_SCL = 1;
    Delay_us(2);
    I2C_SCL = 0;
}
uint8_t I2C_Wait_ACK(void)
{
    uint8_t UcErrTime = 0;
    SDA_IN();
    I2C_SDA = 1;
    Delay_us(1);
    I2C_SCL = 1;
    Delay_us(1);
    while(I2C_READ_SDA)
    {
        UcErrTime++;
        if(UcErrTime>250)
        {
            I2C_Stop();
            return 1;
        }
    }
    I2C_SCL = 0;
    return 0;
}

//I2C发送一个字节
void I2C_Send_Byte(uint8_t data)
{
    uint8_t t;
    SDA_OUT();
    I2C_SCL = 0;//
    for(t = 0;t < 8; t++)
    {
        I2C_SDA = (data&0x80)>>7;
        data <<= 1;
        Delay_us(2);
        I2C_SCL = 1;
        Delay_us(2);
        I2C_SCL = 0;
        Delay_us(2);
    }
}

//I2C读取一个字节
uint8_t I2C_Read_Byte(uint8_t ack)
{
    uint8_t i,Receive = 0;
    SDA_IN();
    for(i = 0; i < 8; i++)
    {
        I2C_SCL = 0;
        Delay_us(2);
        I2C_SCL = 1;
        Receive <<= 1;
        if(I2C_READ_SDA) Receive++;
        Delay_us(1);
    }
    if(!ack) I2C_NACK();
    else I2C_ACK();
    return Receive;
}

