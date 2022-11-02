#include "AHT20.h"

//读AHT20的状态字
static uint8_t AHT20_ReadStatusCmd(void)
{
	uint8_t tmp[1];
	Soft_I2C_Read(AHT20_SLAVE_ADDRESS,AHT20_STATUS_REG,1,tmp);
	return tmp[0];
}

//读AHT20标准使能位
static uint8_t AHT20_ReadCalEnableCmd(void)
{
	uint8_t tmp;
	tmp = AHT20_ReadStatusCmd();
	return (tmp>>3)&0x01;
}

//读取AHT20 忙标志位
static uint8_t AHT20_ReadBusyCmd(void)
{
	uint8_t tmp;
	tmp = AHT20_ReadStatusCmd();
	return (tmp>>7)&0x01;
}

//AHT20芯片初始化函数
static void AHT20_IcInitCmd(void)
{
	uint8_t tmp[2];
	tmp[0] = 0x08;
	tmp[1] = 0x00;
	Soft_I2C_Write(AHT20_SLAVE_ADDRESS, AHT20_INIT_REG, 2, tmp);
}

//触发AHT20测量
static void AHT20_TrigMeasureCmd(void)
{
	uint8_t tmp[2];
	tmp[0] = 0x33;
	tmp[1] = 0x00;
	Soft_I2C_Write(AHT20_SLAVE_ADDRESS, AHT20_Measure, 2, tmp);
}

//AHT20软复位函数
static void AHT20_SoftResetCmd(void)
{
	uint8_t tmp[1];
	Soft_I2C_Write(AHT20_SLAVE_ADDRESS, AHT20_Reset, 0, tmp);
}

//AHT20 设备初始化
uint8_t AHT20_Init(void)
{
	uint8_t rcnt = 2+1;//软复位命令 重试次数，2次
	uint8_t icnt = 2+1;//初始化命令 重试次数，2次
	
	while(--rcnt)
	{
		icnt = 2+1;
		
		Delay_ms(40);//上电后要等待40ms
		// 读取温湿度之前，首先检查[校准使能位]是否为1
		while((!AHT20_ReadCalEnableCmd()) && (--icnt))// 2次重试机会
		{
			Delay_ms(10);
			// 如果不为1，要发送初始化命令
			AHT20_IcInitCmd();
			Delay_ms(200);//这个时间不确定，手册没讲
		}
		
		if(icnt)//[校准使能位]为1,校准正常
		{
			break;//退出rcnt循环
		}
		else//[校准使能位]为0,校准错误
		{
			AHT20_SoftResetCmd();//软复位AHT20器件，重试
			Delay_ms(200);//这个时间不确定，手册没讲
		}
	}
	
	if(rcnt)
	{
		Delay_ms(200);//这个时间不确定，手册没讲
		return 0;// AHT20设备初始化正常
	}
	else
	{
		return 1;// AHT20设备初始化失败
	}
}

//读取AHT20的数据

uint8_t AHT20_ReadHT(uint32_t *HT)
{
	uint8_t cnt=3+1;//忙标志 重试次数，3次
	uint8_t tmp[6];
	uint32_t RetuData = 0;
	
	// 发送触发测量命令
	AHT20_TrigMeasureCmd();
	
	do{
		Delay_ms(75);//等待75ms待测量完成，忙标志Bit7为0
	}while(AHT20_ReadBusyCmd() && (--cnt));//重试3次
	
	if(cnt)//设备闲，可以读温湿度数据
	{
		Delay_ms(5);
		// 读温湿度数据
		Soft_I2C_Read(AHT20_SLAVE_ADDRESS, AHT20_STATUS_REG, 6, tmp);
		// 计算相对湿度RH。原始值，未计算为标准单位%。
		RetuData = 0;
		RetuData = (RetuData|tmp[1]) << 8;
		RetuData = (RetuData|tmp[2]) << 8;
		RetuData = (RetuData|tmp[3]);
		RetuData = RetuData >> 4;
		HT[0] = RetuData;
		
		// 计算温度T。原始值，未计算为标准单位°C。
		RetuData = 0;
		RetuData = (RetuData|tmp[3]) << 8;
		RetuData = (RetuData|tmp[4]) << 8;
		RetuData = (RetuData|tmp[5]);
		RetuData = RetuData&0xfffff;
		HT[1] = RetuData;
		
		return 0;
	}
	else//设备忙，返回读取失败
	{
		return 1;
	}
}

//原始数据转换函数
uint8_t StandardUnitCon(struct AHT20_INF* aht)
{
	aht->rh = (double)aht->ht[0] *100 / 1048576;//2^20=1048576 //原式：(double)aht->HT[0] / 1048576 *100，为了浮点精度改为现在的
	aht->te = (double)aht->ht[1] *200 / 1048576 -50;
	
	//限幅,RH=0~100%; Temp=-40~85°C
	if((aht->rh >=0)&&(aht->rh <=100) && (aht->te >=-40)&&(aht->te <=85))
	{
		aht->flag = 0;
		return 0;//测量数据正常
	}
	else
	{
		aht->flag = 1;
		return 1;//测量数据超出范围，错误
	}
}


uint8_t Soft_I2C_Write(uint8_t addr,uint8_t reg_addr,uint8_t len,uint8_t *data_buf)
{

    uint8_t i;
	
	I2C_Start();
	I2C_Send_Byte(addr << 1 | I2C_Direction_Transmitter);//发送器件地址+写命令
	if(I2C_Wait_ACK())//等待应答
	{
		I2C_Stop();
		return 1;
	}
	
	I2C_Send_Byte(reg_addr);//写寄存器地址
    I2C_Wait_ACK();//等待应答
	
	for(i=0;i<len;i++)
	{
		I2C_Send_Byte(data_buf[i]);//发送数据
		if(I2C_Wait_ACK())//等待ACK
		{
			I2C_Stop();
			return 1;
		}
	}
    I2C_Stop();
	return 0;
}

uint8_t Soft_I2C_Read(uint8_t addr,uint8_t reg_addr,uint8_t len,uint8_t *data_buf)
{

	uint8_t result;
	
	I2C_Start();
	I2C_Send_Byte(addr << 1 | I2C_Direction_Transmitter);//发送器件地址+写命令
	if(I2C_Wait_ACK())//等待应答
	{
		I2C_Stop();
		return 1;
	}
	
	I2C_Send_Byte(reg_addr);//写寄存器地址
    I2C_Wait_ACK();//等待应答
	
	I2C_Start();
	I2C_Send_Byte(addr << 1 | I2C_Direction_Receiver);//发送器件地址+读命令
	I2C_Wait_ACK();//等待应答
	
    while(len)
	{
		if(len==1)*data_buf=I2C_Read_Byte(0);//读数据,发送nACK
		else *data_buf=I2C_Read_Byte(1);//读数据,发送ACK
		len--;
		data_buf++;
	}
    I2C_Stop();//产生一个停止条件
	return 0;
}