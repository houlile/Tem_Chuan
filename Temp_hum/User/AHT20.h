#ifndef _AHT20_H_
#define _AHT20_H_

#include "System.h"
#include "Delay.h"
#include "I2C.h"

#define   AHT20_SLAVE_ADDRESS    0x38//I2C从机地址

#define   AHT20_STATUS_REG       0x00//状态字节
#define   AHT20_INIT_REG         0XBE//初始化
#define   AHT20_Reset            0xBA//软复位信号
#define   AHT20_Measure          0xAC//测量信号

//定义一个存放温湿度传感器信息结构
struct  AHT20_INF{
    uint8_t alive;
    uint8_t flag;
    uint32_t ht[2];
    float rh;//湿度
    float te;//温度
};

#include "System.h"

uint8_t AHT20_Init(void);
uint8_t AHT20_ReadHT(uint32_t *HT);
uint8_t StandardUnitCon(struct AHT20_INF* aht);
uint8_t Soft_I2C_Write(uint8_t addr,uint8_t reg_addr,uint8_t len,uint8_t *data_buf);
uint8_t Soft_I2C_Read(uint8_t addr,uint8_t reg_addr,uint8_t len,uint8_t *data_buf);

#endif

