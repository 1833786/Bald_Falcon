#include "adc.h"


#if FILTER_ENABLE
Ifx_LowPassPt1F32 current_filter;
void LowPassFilter_Init(void)//Ӣ����һ�׵�ͨ�˲�����ʼ��
{
    Ifx_LowPassPt1F32_Config current_filter_config;

    current_filter_config.cutOffFrequency   = 700;          //��ֹƵ��
    current_filter_config.samplingTime      = 0.00005;      //����ʱ��
    current_filter_config.gain              = 1;            //����

    Ifx_LowPassPt1F32_init(&current_filter, &current_filter_config);
}
#endif
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC�ɼ��������������
//  @param      void
//  @return     �������ֵ
//  @since      V = 1.884 + 0.15I  (-10A< I <10A)       ��Ҫ����ʵ���������
//-------------------------------------------------------------------------------------------------------------------
ADC_Typedef Adc_Read(void)
{
    ADC_Typedef adc;

    adc.current_a = (float)adc_convert(ADC_NUMBER, A_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����
    adc.current_b = (float)adc_convert(ADC_NUMBER, B_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����
//    adc.current_c = (double)adc_convert(ADC_NUMBER, C_PHASE_PORT, ADC_12BIT);       //��ȡA��ADCԭʼ����

#if FILTER_ENABLE
    #if 0//�����˲�
        move_filter_double_calc(&current_a_filter, adc.current_a);
        move_filter_double_calc(&current_b_filter, adc.current_b);
        adc.current_a = current_a_filter.data_average;
        adc.current_b = current_b_filter.data_average;
    #else//��ͨ�˲�
        adc.current_a = Ifx_LowPassPt1F32_do(&current_filter, adc.current_a);
        adc.current_b = Ifx_LowPassPt1F32_do(&current_filter, adc.current_b);
    #endif
#endif

    adc.current_a = adc.current_a / 4095 * 22.0 - 12.56;                                     //����A�����
    adc.current_b = adc.current_b / 4095 * 22.0 - 12.56;                                     //����B�����
//    adc.current_c = adc.current_c / 4095 * 22.0 - 12.56;

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
#if FILTER_ENABLE
    LowPassFilter_Init();
    move_filter_double_init(&current_a_filter);//�����˲���ʼ��
    move_filter_double_init(&current_b_filter);//�����˲���ʼ��
#endif
}


