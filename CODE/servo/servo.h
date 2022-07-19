#ifndef _SERVO_H
#define _SERVO_H


#include "headfile.h"
#include "stdio.h"


/******����Ƕȷ�Χ*******/
#define Min_Angle 50
#define Mid_Angle 85
#define Max_Angle 120
/******����Ƕȷ�Χ*******/

typedef struct{
        float Head_Angle;               //��Ŷ����ǰ�Ƕ�
        float Head_Angle_Input;         //�������ĽǶ�
        float Head_Angle_Setup;         //���ö���ĽǶ�
}Servo_Param_TypeDef;
extern Servo_Param_TypeDef Servo_Param;

void Servo_Init(void);
uint8 Servo_Control(float angle);


#endif /*_SERVO_H*/
