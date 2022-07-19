#include "adc.h"


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC�ɼ��������������
//  @param      void
//  @return     �������ֵ
//  @since      V = 1.98 + 0.15I  (-10A< I <10A)
//-------------------------------------------------------------------------------------------------------------------
ADC_Typedef Adc_Read(void)
{
    ADC_Typedef adc;

    adc.current_a = (double)adc_convert(ADC_NUMBER, A_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����
    adc.current_b = (double)adc_convert(ADC_NUMBER, B_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����
//    adc.current_c = (double)adc_convert(ADC_NUMBER, C_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����

    adc.current_a = adc.current_a / 4095 * 22.0 - 12.75;                                     //����A�����
    adc.current_b = adc.current_b / 4095 * 22.0 - 12.75;                                     //����B�����
//    adc.current_c = adc.current_c / 4095 * 22.0 - 12.75;
    //����C�����
#if 1
    move_filter_double_calc(&current_a_filter, adc.current_a);
    move_filter_double_calc(&current_b_filter, adc.current_b);
#endif
    adc.current_a = current_a_filter.data_average;
    adc.current_b = current_b_filter.data_average;
    adc.current_c = 0 - adc.current_a - adc.current_b;                                      //����C�����

    return adc;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC�ɼ����ų�ʼ��
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Adc_Collection_Init(void)
{
    adc_init(ADC_NUMBER, A_PHASE_PORT);             //A��
    adc_init(ADC_NUMBER, B_PHASE_PORT);             //B��
//    adc_init(ADC_NUMBER, C_PHASE_PORT);             //C��
}


