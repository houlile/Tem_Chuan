#ifndef _PWM_OUT_H_
#define _PWM_OUT_H_

#include "System.h"

#define     Servo_up_pointer      300
#define     Servo_up_Maxdown      160
#define     Servo_up_Maxup        1230
#define     Servo_down_pointer    795
#define     Servo_down_Maxleft    160
#define     Servo_down_Maxright   1300




void PWM_Init(u16 arr,u16 psc);

#endif
