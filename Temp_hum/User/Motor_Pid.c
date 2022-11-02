#include "Motor_Pid.h"

Motor_Pid motor_l_pid;
Motor_Pid motor_r_pid;
uint16_t motor_r_A = 0;
uint16_t motor_r_B = 0;
uint16_t motor_l_A = 0;
uint16_t motor_l_B = 0;

void Motor_init()
{
    motor_l_pid.motor_Kp = 0.0f;
    motor_l_pid.motor_Ki = 0.0f;
    motor_l_pid.motor_goal_speed = 0;
    motor_l_pid.motor_curr_speed = 0;
    motor_r_pid.motor_Kp = 0.0f;
    motor_r_pid.motor_Ki = 0.0f;
    motor_r_pid.motor_goal_speed = 0;
    motor_r_pid.motor_curr_speed = 0;
	GPIO_PIN_Init(GPIOA,GPIO_Pin_4,GPIO_Mode_Out_PP,RCC_APB2Periph_GPIOA);
    GPIO_PIN_Init(GPIOC,GPIO_Pin_0,GPIO_Mode_Out_PP,RCC_APB2Periph_GPIOC);
    GPIO_PIN_Init(GPIOC,GPIO_Pin_1,GPIO_Mode_Out_PP,RCC_APB2Periph_GPIOC);
    GPIO_PIN_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_Out_PP,RCC_APB2Periph_GPIOC);
    GPIO_PIN_Init(GPIOC,GPIO_Pin_3,GPIO_Mode_Out_PP,RCC_APB2Periph_GPIOC);
    PAout(4) = 1;
    PCout(0) = 0;
    PCout(1) = 1;
    PCout(2) = 1;
    PCout(3) = 0;
}
void GPIO_PIN_Init(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin,GPIOMode_TypeDef GPIO_Mode,uint32_t RCC_APB2Periph_GPIOx)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOx, GPIO_Pin);
}
void Motor_Timer_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_Cmd(TIM2,ENABLE);//使能TIM2外设
}
void Motor_PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_DeInit(TIM1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Reset;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_High;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);//使能ccr2上的预装载寄存器
    TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1,ENABLE);
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    TIM_Cmd(TIM1,ENABLE);
}
void L_Encoder_Speed_Measure_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535;//
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 9;
    TIM_ICInit(TIM3,&TIM_ICInitStructure);

    TIM_ClearFlag(TIM3,TIM_FLAG_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_SetCounter(TIM3,0);
    TIM3->CNT = 0;
    TIM_Cmd(TIM3,ENABLE);
}
void R_Encoder_Speed_Measure_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535;//
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 9;
    TIM_ICInit(TIM4,&TIM_ICInitStructure);

    TIM_ClearFlag(TIM4,TIM_FLAG_Update);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    TIM_SetCounter(TIM4,0);
    TIM4->CNT = 0;
    TIM_Cmd(TIM4,ENABLE);
}
