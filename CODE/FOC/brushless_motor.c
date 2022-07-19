#include "brushless_motor.h"

FOC_Parm_Typedef FOC;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������������     �ر���������
//  @param      periodAH:       A����PWMռ�ձ�
//  @param      periodBH:       B����PWMռ�ձ�
//  @param      periodCH:       C����PWMռ�ձ�
//  @return     void
//  @since      �������Ķ���ģʽ��װ��ֵԽ�󣬸ߵ�ƽʱ��Խ��
//-------------------------------------------------------------------------------------------------------------------
void Mos_All_High_Open(uint16 periodAH, uint16 periodBH, uint16 periodCH)
{
    ccu6SFR->MODCTR.B.T12MODEN = 0x3F;//0011 1111

    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_0, periodAH);
    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_1, periodBH);
    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_2, periodCH);
    IfxCcu6_enableShadowTransfer(ccu6SFR, TRUE, FALSE);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      �ر��������ţ������������ţ�ʹ�������Ȧ���Ըе綯�Ƶ��������Լ����ɲ��
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Mos_All_Low_Open(void)
{
    ccu6SFR->MODCTR.B.T12MODEN = 0x2A;//0010 1010

    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_0, 0);
    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_1, 0);
    IfxCcu6_setT12CompareValue(ccu6SFR, IfxCcu6_T12Channel_2, 0);
    IfxCcu6_enableShadowTransfer(ccu6SFR, TRUE, FALSE);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      �м��������
//  @param      clark: Alpha & Beta
//  @return     �м����������
//  @since      x,y,zΪ�м��������ʵ������
//-------------------------------------------------------------------------------------------------------------------
Instrument_Typedef Tool_Calc(CLARK_Typedef clark)
{
    Instrument_Typedef tool;

    tool.x=   clark.Beta;
    tool.y =  clark.Alpha * sqrt3 / 2.0 - clark.Beta / 2.0;
    tool.z = -clark.Alpha * sqrt3 / 2.0 - clark.Beta / 2.0;

    return tool;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��Ƕ������ж�
//  @param      temp:���ڸ���������м����
//  @return     N = ��1~6��
//              3 1 5 4 6 2 3��Nֵ��
//              1 2 3 4 5 6 1��������
//  @since      x,y,zΪ�м��������ʵ������
//-------------------------------------------------------------------------------------------------------------------
uint8 Electrical_Sector_Judge(Instrument_Typedef tool)
{
    uint8 N = 0;

    if(tool.x > 0) N = N + 1;
    if(tool.y > 0) N = N + 2;
    if(tool.z > 0) N = N + 4;

    return N;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ÿ����������ʸ������ʱ�����
//  @param      tool:���ڸ���������м����
//  @param      N:�ж�������Nֵ
//  @param      Udc:ĸ�ߵ�ѹ
//  @param      T:PWM���ڣ�װ��ֵ��
//  @return     ������ʱ�������
//  @since      ���������ֱ�����ڸ�����������ʸ��������ʱ��
//-------------------------------------------------------------------------------------------------------------------
VectorTime_Typedef Vector_Calc(Instrument_Typedef tool, uint8 N, uint8 Udc, uint16 T)
{
    VectorTime_Typedef vector;

    double temp = sqrt3 * T / Udc;//Ϊ�˵ȷ�ֵ�任���ѳ���2/3

    switch (N)
    {
        case 3://����1
            vector.ta =  temp * tool.y;
            vector.tb =  temp * tool.x;
            break;
        case 1://����2
            vector.ta = -temp * tool.y;
            vector.tb = -temp * tool.z;
            break;
        case 5://����3
            vector.ta =  temp * tool.x;
            vector.tb =  temp * tool.z;
            break;
        case 4://����4
            vector.ta = -temp * tool.x;
            vector.tb = -temp * tool.y;
            break;
        case 6://����5
            vector.ta =  temp * tool.z;
            vector.tb =  temp * tool.y;
            break;
        case 2://����6
            vector.ta = -temp * tool.z;
            vector.tb = -temp * tool.x;
            break;
        default:
            vector.ta = 0;
            vector.tb = 0;
            break;
    }

    return vector;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ÿ������������PWMռ�ձȼ���
//  @param      vector:������ʱ�������
//  @param      N:�ж�������Nֵ
//  @param      T:PWM���ڣ�װ��ֵ��
//  @return     �����������ŵ�PWMռ�ձ�
//  @since      ����value1, value2, value3�Ǹ����߶�ʽSVPWM���Ʒ������������еķֲ����ɵó�
//-------------------------------------------------------------------------------------------------------------------
Period_Typedef PeriodCal(VectorTime_Typedef vector, uint8 N, uint16 T)
{
    Period_Typedef period;
    uint16 value1, value2, value3;
    double Ttemp = vector.ta + vector.tb;

    if(Ttemp > T)
    {
        vector.ta = vector.ta / Ttemp * (double)T;
        vector.tb = vector.tb / Ttemp * (double)T;
    }
    value1 = (uint16)(((double)T - vector.ta - vector.tb) / 4.0);
    value2 = (uint16)(value1 + vector.ta / 2.0);
    value3 = (uint16)(value2 + vector.tb / 2.0);
    switch (N)
    {
        case 3:
            period.AH = value1;
            period.BH = value2;
            period.CH = value3;
            break;
        case 1:
            period.AH = value2;
            period.BH = value1;
            period.CH = value3;
            break;
        case 5:
            period.AH = value3;
            period.BH = value1;
            period.CH = value2;
            break;
        case 4:
            period.AH = value3;
            period.BH = value2;
            period.CH = value1;
            break;
        case 6:
            period.AH = value2;
            period.BH = value3;
            period.CH = value1;
            break;
        case 2:
            period.AH = value1;
            period.BH = value3;
            period.CH = value2;
            break;
        default:
            period.AH = 0;
            period.BH = 0;
            period.CH = 0;
            break;
    }

    return period;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      �����Ƕ�
//  @param      hall:�����������������
//  @param      speed:ת���ٶȣ�RPM��
//  @param      dir:ת���趨��ת����
//  @return     ��Ƕ�
//  @since
//-------------------------------------------------------------------------------------------------------------------
double Theta_Calc (HALL_Typedef hall, int32 speed, MOTOR_DIR_enum dir)
{
    static double theta = 0 ;

    if(speed < 0)  speed = -speed;

    if (dir)
    {
        if (hall.value.now != hall.value.last)
        {
            switch (hall.value.now)
            {
                case 1: theta  = degrees_180;   break;
                case 2: theta  = degrees_240;  break;
                case 3: theta  = degrees_300; break;
                case 4: theta  = degrees_0; break;
                case 5: theta  = degrees_60; break;
                case 6: theta  = degrees_120; break;
                default:                      break;
            }
        }
        else
        {
            theta -= speed * pi /600000.0;
            if (theta <= 0)     theta = pi_2;
        }
    }
    else
    {
        if (hall.value.now != hall.value.last)
        {
            switch (hall.value.now)
            {
                case 4: theta  = degrees_180;   break;
                case 5: theta  = degrees_240;  break;
                case 6: theta  = degrees_300; break;
                case 1: theta  = degrees_0; break;
                case 2: theta  = degrees_60; break;
                case 3: theta  = degrees_120; break;
                default:                      break;
            }
        }
        else
        {
            theta += speed * pi /600000.0;
            if (theta >= pi_2)  theta = 0;
        }
    }

    return theta;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ת����ת�ٶȼ���
//  @param      hall:�����������������
//  @return     ת���ٶȣ�RPM��
//  @since
//-------------------------------------------------------------------------------------------------------------------
void FOC_Status_Display(MOTOR_DIR_enum dir)
{
    switch (dir)
    {
        case FORWARD:
            Led_Ctrl(2, ON);
            Led_Ctrl(1, OFF);
            break;
        case REVERSE:
            Led_Ctrl(1, ON);
            Led_Ctrl(2, OFF);
            break;
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      �ж�ת�ӵ�ǰ��ת����
//  @param      hall:�����������������
//  @return     ת�ӵ�ǰ��ת����
//  @since
//-------------------------------------------------------------------------------------------------------------------
MOTOR_DIR_enum actual_dir_judgement(HALL_Typedef hall)
{
    MOTOR_DIR_enum dir;

    switch (hall.value_save[1] - hall.value_save[0])
    {
        case  4: dir = FORWARD; break;
        case  3: dir = FORWARD; break;
        case  2: dir = FORWARD; break;
        case  1: dir = FORWARD; break;
        case -5: dir = FORWARD; break;

        case -4: dir = REVERSE; break;
        case -3: dir = REVERSE; break;
        case -2: dir = REVERSE; break;
        case -1: dir = REVERSE; break;
        case  5: dir = REVERSE; break;
    }
    FOC_Status_Display(dir);

    return dir;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ת����ת�ٶȼ���
//  @param      hall:�����������������
//  @return     ת���ٶȣ�RPM��
//  @since
//-------------------------------------------------------------------------------------------------------------------
int32 calc_speed(HALL_Typedef hall)
{
    //ת�ټ���
    uint8 i;
    int32 speed;
    hall.commutation_time_sum = 0;
    for(i=0; i<6; i++)
    {
        hall.commutation_time_sum += hall.commutation_time_save[i];
    }

    //���ת�ټ���˵��
    //2.commutation_time_sum��ͳ�Ƶ������6�λ������ٴ�T12�����ж�
        //2.1 T12�����ж������жϵ�Ƶ��Ϊ20KHz
        //2.2 �������ù�ϵccu6_t12_pwm->scan_hall_status->calc_speed
        //2.3 commutation_timeΪͳ�ƻ���ʱ��ı���
    //3.ͨ�����ת�����Ƕ���RPM��ʾ��RPM��ʾÿ���ӵ����ת��
    //3.���תһȦ��Ҫ����Ĵ������� ���������*6
    //4.��˵��ת�ٵ���60*ADC�ж�Ƶ��/���������/commutation_time_sum���������Եõ����ÿ���ӵ�ת��

    speed = PWM_PIT_NUM/POLEPAIRS/hall.commutation_time_sum;

    if(REVERSE == FOC.BLmotor.actual_dir)//�����ת��ʱ����Ҫ���ٶ�ȡ��
    {
        speed = -speed;
    }
    move_filter_calc(&speed_filter, speed);

    speed = speed_filter.data_average;

    return speed;
}
#if CURRENT_CLOSE_LOOP_ENABLE
//-------------------------------------------------------------------------------------------------------------------
//  @brief      d��q��PI������
//  @param      ref_park:d��qĿ��ֵ
//  @param      I_park  :d��qʵ��ֵ
//  @return     d��q����ֵ
//  @since
//-------------------------------------------------------------------------------------------------------------------
PARK_Typedef Current_Close_Loop(PARK_Typedef ref_park, PARK_Typedef I_park)
{
    PARK_Typedef park_in;
    double error_d, error_q;
    static double error_sum_d = 0, error_sum_q = 0;

    error_d = ref_park.d - I_park.d;
    error_q = ref_park.q - I_park.q;
    error_sum_d += error_d;
    error_sum_q += error_q;

    if (error_sum_d >  2) error_sum_d =  2;
    if (error_sum_d < -2) error_sum_d = -2;
    if (error_sum_q >  2) error_sum_d =  2;
    if (error_sum_q < -2) error_sum_d = -2;

    park_in.d = FOC.Current_CL.d.kp * error_d + FOC.Current_CL.d.ki * error_sum_d;
    park_in.q = FOC.Current_CL.q.kp * error_q + FOC.Current_CL.q.ki * error_sum_q;

    return park_in;
}
#endif
////-------------------------------------------------------------------------------------------------------------------
////  @brief      svpwm�����㷨
////  @param      Udc:ĸ�ߵ�ѹ
////  @return     T:PWM���װ��ֵ
////  @since
////-------------------------------------------------------------------------------------------------------------------
void SVPWM_Algorithm(uint8 Udc, uint16 T)
{
    FOC.hall               = Get_Hall_Value();
    FOC.BLmotor.actual_dir = actual_dir_judgement(FOC.hall);
    FOC.BLmotor.speed      = calc_speed(FOC.hall);
    if (abs(FOC.BLmotor.speed - FOC.BLmotor.last_speed) > 220)//����ȥ��ë��
    {
        FOC.BLmotor.speed = FOC.BLmotor.last_speed;
    }
    else
    {
        FOC.BLmotor.last_speed = FOC.BLmotor.speed;
    }
    FOC.theta              = Theta_Calc (FOC.hall, FOC.BLmotor.speed, FOC.BLmotor.dir);

#if CURRENT_CLOSE_LOOP_ENABLE
//    FOC.Adc     = Adc_Read();
    FOC.I_Clrak = Clark_Calc(FOC.Adc);
    FOC.I_Park  = Park_Calc(FOC.I_Clrak, FOC.theta);
    FOC.Park_In = Current_Close_Loop(FOC.Ref_Park, FOC.I_Park);
#endif
#if CURRENT_CLOSE_LOOP_ENABLE
    FOC.V_Clark   = Anti_Park_Calc(FOC.Park_In, FOC.theta);//Id,Iq����������任
//    FOC.V_Clark   = Anti_Park_Calc(FOC.Ref_Park, FOC.theta);//Id,Iq����������任
#else
    FOC.V_Clark   = Anti_Park_Calc(FOC.Ref_Park, FOC.theta);//Id,Iq����������任
#endif
    FOC.tool    = Tool_Calc(FOC.V_Clark);//�м��������
    FOC.N       = Electrical_Sector_Judge(FOC.tool);//��Ƕ������ж�
    FOC.Vector  = Vector_Calc(FOC.tool, FOC.N, Udc, T);//ʸ������ʱ�����
    FOC.Period  = PeriodCal(FOC.Vector, FOC.N, T);//����PWMռ�ձȼ���

    Mos_All_High_Open(FOC.Period.AH, FOC.Period.BH, FOC.Period.CH);
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ��ˢ�������API
////  @param      duty:qֵ��С
////  @return     void
////  @since      ����ʱ����main.cʹ��
////-------------------------------------------------------------------------------------------------------------------
void Momemtum_Motor_Control(float duty)
{
    if (duty > 0)
    {
        if (duty >=  4) duty =  4;
        FOC.Ref_Park.q =  duty + Q_DEAD_PERIOD;
        FOC.BLmotor.dir = FORWARD;
    }
    else
    {
        if (duty <= -4) duty = -4;
        FOC.Ref_Park.q = -duty + Q_DEAD_PERIOD;
        FOC.BLmotor.dir = REVERSE;
    }
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      FOC������ݴ�ӡ
////  @param      void
////  @return     void
////  @since      ����ʱ����main.cʹ��
////-------------------------------------------------------------------------------------------------------------------
void FOC_TEST_PRINTF(void)
{
    printf("%f,%f,%f\r\n",FOC.Adc.current_a, FOC.Adc.current_b, FOC.Adc.current_c);
//    printf("%f,%f,%f,%f\r\n",FOC.Ref_Park.d, FOC.Ref_Park.q, FOC.I_Park.d, FOC.I_Park.q);
//    printf("%d\r\n",FOC.hall.value.now);
//    printf("%ld\r\n",FOC.BLmotor.speed);
//    printf("%f\r\n",FOC.theta);
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      FOC������ʼ��
////  @param      void
////  @return     void
////  @since
////-------------------------------------------------------------------------------------------------------------------
void FOC_Param_Init(void)
{
    FOC.Ref_Park.d = 0;
    FOC.Ref_Park.q = 0;//1.0
    FOC.BLmotor.dir = FORWARD;

#if CURRENT_CLOSE_LOOP_ENABLE
    FOC.Current_CL.d.kp = 0.53;//0.53
    FOC.Current_CL.d.ki = 0.000037;//0.000037

    FOC.Current_CL.q.kp = 0.26;//0.26
    FOC.Current_CL.q.ki = 0.000037;//0.000037
#endif
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      FOC��ʼ��
////  @param      void
////  @return     void
////  @since
////-------------------------------------------------------------------------------------------------------------------
void FOC_Init(void)
{
    FOC_Param_Init();               //FOC������ʼ��
    ccu6_pwm_init();                //CCU6 PWMģ���ʼ��
    Hall_Init();                    //������������ʼ��
    move_filter_init(&speed_filter);//�����˲���ʼ��
#if !CURRENT_CLOSE_LOOP_ENABLE
    move_filter_double_init(&current_a_filter);//�����˲���ʼ��
    move_filter_double_init(&current_b_filter);//�����˲���ʼ��
    Adc_Collection_Init();          //adc����������ʼ��
#endif
}
