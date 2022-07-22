#ifndef _MOTOR_PID_H
#define _MOTOR_PID_H

#include "hall.h"
#include "servo.h"
#include "private_common.h"

extern uint8 Momentum_Motor_Flag;  //��������ͣ��־

/**************************���������**************************/

typedef struct{
        float left;
        float mid;
        float right;
}Direction_TypeDef;

typedef struct{
        PID_TypeDef Angle_PID;
        PID_TypeDef Velocity_PID;
        Direction_TypeDef Balance_Angle;
        Direction_TypeDef Speed_Correction;
}Balance_Param_TypeDef;
extern Balance_Param_TypeDef Balance_Param;

void Balance_Control(void);
/**************************���������**************************/
void BANLANCE_TEST_PRINTF(void);
void Banlance_Algorithm_Init(void);

#endif /*_MOTOR_PID_H*/



