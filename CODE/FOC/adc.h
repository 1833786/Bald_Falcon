#ifndef _ADC_H
#define _ADC_H

#include "zf_vadc.h"
#include "common.h"
#include "move_filter.h"
#include "SysSe/Math/Ifx_LowPassPt1F32.h"

#define ADC_NUMBER              ADC_2           //����ADC���

#define A_PHASE_PORT            ADC2_CH3_A35     //A�������ȡͨ��
#define B_PHASE_PORT            ADC2_CH4_A36     //B�������ȡͨ��
//#define C_PHASE_PORT            ADC2_CH5_A37     //C�������ȡͨ��

#define Joystick_Y              ADC2_CH5_A37
#define Joystick_SW             ADC2_CH6_A38


#define FILTER_ENABLE           0

typedef struct
{
    double voltage_bus;         //ĸ�ߵ���
    double current_a;           //A�����
    double current_b;           //B�����
    double current_c;           //C�����
}ADC_Typedef;



ADC_Typedef Adc_Read(void);
void Adc_Collection_Init(void);

#endif /*_ADC_H*/
