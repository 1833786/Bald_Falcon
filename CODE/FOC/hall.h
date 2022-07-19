#ifndef _HALL_H
#define _HALL_H

#include <stdio.h>
#include "IFXGTM_TIM_IN.h"
#include "ifxGtm_Tim.h"
#include "zf_stm_systick.h"
#include "private_common.h"
#include "zf_gpio.h"
#include "move_filter.h"

#define COMMUTATION_TIMEOUT 5000
#define CMU_CLK_ALL_DISABLE 0x55555555
#define CMU_CLK_ENABLE  2


#define REV_CNT_VAL         (10u)   //����̶����������ж�
#define FILTER_CON_NUM      (10u)   //�����źŵ��˲��г�����Ҫ������ȥ��ë��


//��������Ϊʵ�������������
#define HALL_A          IfxGtm_TIM0_2_TIN33_P33_11_IN   //�����ź�A������
#define HALL_B          IfxGtm_TIM0_0_TIN34_P33_12_IN   //�����ź�B������
#define HALL_C          IfxGtm_TIM0_1_TIN35_P33_13_IN   //�����ź�C������


typedef struct
{
        Uint8_Status_Typedef value;
        uint8 value_save[2];
        uint16 commutation_time_save[6];
        uint32 commutation_time_sum;
        uint16 commutation_time;
}HALL_Typedef;



void Hall_Init(void);
HALL_Typedef Get_Hall_Value(void);


#endif/*_HALL_H*/

