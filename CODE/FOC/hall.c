#include "hall.h"

uint8 control_hall[6] = {3,1,5,4,6,2};

void clock_cfg(void)
{
    IfxGtm_enable(&MODULE_GTM);

    GTM_CMU_CLK_EN.U                    = CMU_CLK_ALL_DISABLE;

    GTM_CMU_GCLK_NUM.B.GCLK_NUM         = 1;//��Ƶϵ���ķ���
    GTM_CMU_GCLK_NUM.B.GCLK_NUM         = 1;
    GTM_CMU_GCLK_DEN.B.GCLK_DEN         = 1;//��Ƶϵ���ķ�ĸ

    GTM_CMU_CLK_0_CTRL.B.CLK_CNT        = 4;//��Ƶϵ��
    GTM_CMU_CLK_1_CTRL.B.CLK_CNT        = 0;
    GTM_CMU_FXCLK_CTRL.B.FXCLK_SEL      = 0;//ѡ��FXCLK��ʱ����Դ

    GTM_CMU_CLK_EN.B.EN_CLK0            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_CLK1            = CMU_CLK_ENABLE;
    GTM_CMU_CLK_EN.B.EN_FXCLK           = CMU_CLK_ENABLE;
}


void hall_tim_input_cfg(void)
{
    IfxGtm_PinMap_setTimTin(&HALL_A, IfxPort_InputMode_pullUp);
    IfxGtm_PinMap_setTimTin(&HALL_B, IfxPort_InputMode_pullUp);
    IfxGtm_PinMap_setTimTin(&HALL_C, IfxPort_InputMode_pullUp);

    GTM_TIM0_CH0_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;//ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_0   = 0;
    GTM_TIM0_IN_SRC.B.VAL_0    = 0;

    GTM_TIM0_CH1_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;//ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_1   = 0;
    GTM_TIM0_IN_SRC.B.VAL_1    = 0;

    GTM_TIM0_CH2_CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;//ʹ�õ�ǰͨ��
    GTM_TIM0_IN_SRC.B.MODE_2   = 0;
    GTM_TIM0_IN_SRC.B.VAL_2    = 0;


    GTM_TIM0_CH0_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH0_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;//�˲�����Ƶ��ѡ��
    GTM_TIM0_CH0_CTRL.B.FLT_EN       = TRUE;//�˲�ʹ��
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�½����˲�ģʽ ����
    GTM_TIM0_CH0_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�Ͻ����˲�ģʽ ����
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH0_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH0_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;//�����˲�����ֵ
    GTM_TIM0_CH0_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH1_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH1_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;//�˲�����Ƶ��ѡ��
    GTM_TIM0_CH1_CTRL.B.FLT_EN       = TRUE;//�˲�ʹ��
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�½����˲�ģʽ ����
    GTM_TIM0_CH1_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�Ͻ����˲�ģʽ ����
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH1_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH1_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;//�����˲�����ֵ
    GTM_TIM0_CH1_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH2_CTRL.B.CLK_SEL      = IfxGtm_Cmu_Clk_0;
    GTM_TIM0_CH2_CTRL.B.FLT_CNT_FRQ  = IfxGtm_Cmu_Tim_Filter_Clk_0;//�˲�����Ƶ��ѡ��
    GTM_TIM0_CH2_CTRL.B.FLT_EN       = TRUE;//�˲�ʹ��
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_FE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�½����˲�ģʽ ����
    GTM_TIM0_CH2_CTRL.B.FLT_MODE_RE  = IfxGtm_Tim_FilterMode_individualDeglitchTime;//�Ͻ����˲�ģʽ ����
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_FE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH2_CTRL.B.FLT_CTR_RE   = IfxGtm_Tim_FilterCounter_upDown;//��λ��ҪFLT_MODE_RE����Ϊ1����Ч
    GTM_TIM0_CH2_FLT_FE.B.FLT_FE     = FILTER_CON_NUM;//�����˲�����ֵ
    GTM_TIM0_CH2_FLT_RE.B.FLT_RE     = FILTER_CON_NUM;

    GTM_TIM0_CH0_CTRL.B.TIM_MODE = IfxGtm_Tim_Mode_inputEvent;//�����¼�
    GTM_TIM0_CH0_CTRL.B.ISL      = TRUE;//˫���ش���
    GTM_TIM0_CH0_CTRL.B.TIM_EN   = TRUE;//ʹ��
    GTM_TIM0_CH1_CTRL.B.TIM_MODE = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH1_CTRL.B.ISL      = TRUE;
    GTM_TIM0_CH1_CTRL.B.TIM_EN   = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_MODE = IfxGtm_Tim_Mode_inputEvent;
    GTM_TIM0_CH2_CTRL.B.ISL      = TRUE;
    GTM_TIM0_CH2_CTRL.B.TIM_EN   = TRUE;
}

void Hall_Init(void)
{
    clock_cfg();    //GTMģ��ʱ�����ó�ʼ��
    hall_tim_input_cfg();      //��ʼ�����������Լ�ɲ������
}


HALL_Typedef Get_Hall_Value(void)
{
    static HALL_Typedef hall;
    static uint8 hall_index = 0;

    hall.value.last = hall.value.now;
    hall.value.now = GTM_SPE0_CTRL_STAT.B.NIP;

    for(int i=0; i<6; i++)
    {
        if(hall.value.now == control_hall[i])
        {
            hall.value.now = i+1;
            break;
        }
    }

    hall.commutation_time++;
    if(hall.commutation_time >= COMMUTATION_TIMEOUT)
    {
        hall.commutation_time = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[0] = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[1] = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[2] = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[3] = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[4] = COMMUTATION_TIMEOUT;
        hall.commutation_time_save[5] = COMMUTATION_TIMEOUT;

        //����ƽ���˲���ʼ��
        move_filter_init(&speed_filter);
    }
    if (hall.value.now != hall.value.last)
    {
        hall_index++;
        if(hall_index >= 6)
        {
            hall_index = 0;
        }
        hall.commutation_time_save[hall_index] = hall.commutation_time;       //�ٶ���Ϣ����
        hall.commutation_time = 0;

        hall.value_save[0] = hall.value_save[1];
        hall.value_save[1] = hall.value.now;
    }


    return hall;
}





