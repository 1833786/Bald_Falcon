#ifndef _BUSHLESS_MOTOR_H
#define _BUSHLESS_MOTOR_H

#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "IfxCcu6.h"
#include "ccu6_pwm.h"
#include "IfxCcu6.h"
#include "clark.h"
#include "park.h"
#include "hall.h"
#include "move_filter.h"
#include "private_common.h"


#define Q_DEAD_PERIOD               1.0         //q������
#define PWM_PIT_NUM                 60*FPWM
#define BUS_VOLTAGE                 12          //ĸ�ߵ�ѹ
#define POLEPAIRS                   7           //���������
#define CURRENT_CLOSE_LOOP_ENABLE   0           //���������ƿ���
#define FOC_CTRL_MODE               1           //SVPWM����ģʽ���� 0:whileѭ��   1:�жϿ���

typedef enum
{
    FORWARD,    //��ת
    REVERSE,    //��ת
}MOTOR_DIR_enum;

typedef struct
{
    int32 speed;
    int32 last_speed;
    int32 set_speed;    //���õ��ٶ�
    int32 max_speed;    //�ٶ����ֵ
    int32 min_speed;    //�ٶ���Сֵ
    MOTOR_DIR_enum actual_dir;
    MOTOR_DIR_enum dir;
}BLmotor_Typedef;

typedef struct
{
    double x;
    double y;
    double z;
}Instrument_Typedef;

typedef struct
{
    double ta;
    double tb;
}VectorTime_Typedef;

typedef struct
{
    uint16 AH;
    uint16 AL;
    uint16 BH;
    uint16 BL;
    uint16 CH;
    uint16 CL;
}Period_Typedef;

typedef struct
{
    PID_TypeDef d;
    PID_TypeDef q;
}Current_CL_Typedef;

typedef struct
{
    BLmotor_Typedef BLmotor;        //�������
    ADC_Typedef Adc;                //adc�ɼ�
    CLARK_Typedef V_Clark;          //Alpha��Beta����
    CLARK_Typedef I_Clrak;          //Alpha��Beta����
    PARK_Typedef Ref_Park;          //d��qĿ��ֵ
    PARK_Typedef I_Park;            //d��q����ֵ
    PARK_Typedef Park_In;           //d��q����ֵ
    Instrument_Typedef tool;        //SVPWM�㷨�м���
    VectorTime_Typedef Vector;      //ʸ������ʱ��
    HALL_Typedef hall;              //��������������
    uint8  N;                       //��Ƕ�����
    double theta;                   //��Ƕ�
    Period_Typedef Period;          //����ռ�ձ�
    Current_CL_Typedef Current_CL;  //������PID����
}FOC_Parm_Typedef;
extern FOC_Parm_Typedef FOC;

void Mos_All_High_Open(uint16 periodAH, uint16 periodBH, uint16 periodCH);
void Mos_All_Low_Open(void);
void SVPWM_Algorithm(uint8 Udc, uint16 T);
void Momemtum_Motor_Control(float duty);
void FOC_TEST_PRINTF(void);
void FOC_Init(void);

#endif/*_BUSHLESS_MOTOR_H*/
