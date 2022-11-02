#include "Delay.h"

static u8 fac_us = 0;
static u16 fac_ms = 0;
void Delay_Init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock / 8000000;
    fac_ms = (u16)fac_us * 1000;
}
void Delay_us(u32 us)
{
    u32 delay;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        delay = SysTick->CTRL;
    }while(delay&0x01&&!(delay&(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}
void Delay_ms(u32 ms)//ms级延时最长时间为1864ms
{
    u32 delay;
    SysTick->LOAD = (u32)ms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        delay = SysTick->CTRL;
    }while(delay&0x01&&!(delay&(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;

}
