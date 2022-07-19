#ifndef __CLARKE_H
#define __CLARKE_H

#include "adc.h"
#include "private_common.h"

typedef struct
{
    double Alpha;                   // �������ֹ����ϵAlpha�ᶨ�ӵ���
    double Beta;                    // �������ֹ����ϵBeta�ᶨ�ӵ���

}CLARK_Typedef;
CLARK_Typedef Clark_Calc(ADC_Typedef adc);

#endif /*__CLARKE_H*/
