#ifndef _MOMENTUM_WHEEL_H
#define _MOMENTUM_WHEEL_H

#include "encoder.h"
#include "zf_gtm_pwm.h"
#include "stdio.h"
#include "private_common.h"

#define Motor_CHA    ATOM0_CH5_P02_5
#define Motor_CHB    ATOM0_CH4_P02_4


#define Motor_PWM_Freq        5000   //PWM��Ƶ��
#define Motor_Max_Speed       7200   //���ֵΪ10 000, ��PWM_DUTY_MAX

/**************************���������**************************/

typedef struct
{
        PID_TypeDef PID;
        float Speed;                //��������ֵ�ǰ�ٶ�
        float Speed_Setup;          //���������ֵ��ٶ�
        float Speed_Input;          //���������ֵ��ٶ�
}Speed_Param_TypeDef;
extern Speed_Param_TypeDef Speed_Param;

void Speed_Control(float target_speed);

/**************************���������**************************/


void Motor_Init(void);
void Motor_Stop(void);
void Motor_Control(int duty);


#endif /*_MOMENTUM_WHEEL_H*/
