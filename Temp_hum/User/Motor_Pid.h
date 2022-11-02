#ifndef _MOTOR_PID_H_
#define _MOTOR_PID_H_

#include "System.h"

typedef struct _Motor{
    float motor_Kp;
    float motor_Ki;
    int16_t motor_goal_speed;
    int16_t motor_curr_speed;
}Motor_Pid;

void Motor_init(void);
void Motor_PWM_Init(u16 arr,u16 psc);
void Motor_Timer_Init(u16 arr,u16 psc);
void L_Encoder_Speed_Measure_Init(void);
void R_Encoder_Speed_Measure_Init(void);
void GPIO_PIN_Init(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin,GPIOMode_TypeDef GPIO_Mode,uint32_t RCC_APB2Periph_GPIOx);

#endif
