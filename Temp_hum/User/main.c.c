#include "System.h"
#include "Delay.h"
#include "UART.h"
#include "AHT20.h"

struct AHT20_INF AHT20;

int main(void)
{
	//-------------------
	//oled相关初始化
		delay_init();	    	      
		NVIC_Configuration(); 	   
		OLED_Init();			         
		OLED_Clear(0);
  //-------------------	
		IIC_Init();
    Delay_Init();
		My_Uart_Init(115200);
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);

		AHT20.alive = !AHT20_Init();
    while(1)
    {
		if(AHT20.alive)
		{
			AHT20.flag = AHT20_ReadHT(AHT20.ht);
			StandardUnitCon(&AHT20);
		}
		
		TEST_MainPage();
		printf("温度:%.2f°c  湿度:%.2f%%\n",AHT20.te,AHT20.rh);
    Delay_ms(1000);

    }
}
