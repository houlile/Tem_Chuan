#include "Pwm_Out.h"

void PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_DeInit(TIM8);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_High;
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);
    TIM_OC4Init(TIM8, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM8,TIM_OCPreload_Enable);//使能ccr2上的预装载寄存器
    TIM_OC4PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8,ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    TIM_Cmd(TIM8,ENABLE);
} 
