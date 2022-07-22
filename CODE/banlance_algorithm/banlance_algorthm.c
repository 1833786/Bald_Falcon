#include "banlance_algorthm.h"

/*******************��������ر���������********************/
Balance_Param_TypeDef Balance_Param;

void Balance_Param_Init(void)
{
    Balance_Param.Angle_PID.kp = 1.1;//0.8
    Balance_Param.Angle_PID.ki = 0.0071;//0.02
    Balance_Param.Angle_PID.kd = 0.0065;//0.007

    Balance_Param.Velocity_PID.kp = -0.0022;//-0.2
    Balance_Param.Velocity_PID.ki = -0.0009;//-0.07
    Balance_Param.Velocity_PID.kd = 0;

    Balance_Param.Balance_Angle.left = -1.31;
    Balance_Param.Balance_Angle.mid = 0.5 ;
    Balance_Param.Balance_Angle.right = 2.6;

    Balance_Param.Speed_Correction.left  = 0.013;
    Balance_Param.Speed_Correction.right = 0.013;
}
uint8 Momentum_Motor_Flag = 1;  //��������ͣ��־
float Dynamic_Balance_Angle;     //��̬ƽ��Ƕ�
float Vetical_PWM = 0, Velocity_PWM = 0;//������PWM�м���
float Momentum_Motor_Duty = 0;  //������PWM������

float Momentum_Speed;
/*******************��������ر���������********************/



/**********************�����ֿ��ƺ���***********************/
float Middle_Angle_Counting(float head_angle, float bicycle_speed)
{
    float La, Lb, Ra, Rb;           //����ƽ��Ƕ����Ժ���ϵ��
    La = (Balance_Param.Balance_Angle.left - Balance_Param.Balance_Angle.mid)/(Max_Angle - Mid_Angle);
    Lb = (Balance_Param.Balance_Angle.left - Max_Angle * La);

    Ra = (Balance_Param.Balance_Angle.right - Balance_Param.Balance_Angle.mid)/(Min_Angle - Mid_Angle);
    Rb = (Balance_Param.Balance_Angle.right - Min_Angle * Ra);

    float Middle_Angle_Output;
    uint8 angle_flag = 1, speed_flag = 1;
    if (bicycle_speed <= 10)     speed_flag = 0;
    if (head_angle >= Mid_Angle) angle_flag = 0;//�Ƿ�����

    switch(speed_flag*10 + angle_flag)
    {
        case 00:
            Middle_Angle_Output = La*head_angle + Lb;
            break;
        case 01:
            Middle_Angle_Output = Ra*head_angle + Rb;
            break;
        case 10:
//            Middle_Angle_Output = -(La*head_angle + Lb);
//            Middle_Angle_Output *= bicycle_speed*Balance_Param.Speed_Correction.left;
            Middle_Angle_Output = La*head_angle + Lb;
            Middle_Angle_Output += bicycle_speed*Balance_Param.Speed_Correction.left;
            break;
        case 11:
//            Middle_Angle_Output = -(Ra*head_angle + Rb);
//            Middle_Angle_Output = bicycle_speed*Balance_Param.Speed_Correction.right;
            Middle_Angle_Output = Ra*head_angle + Rb;
            Middle_Angle_Output -= bicycle_speed*Balance_Param.Speed_Correction.right;
            break;
    }
    return Middle_Angle_Output;
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      �ǶȻ�
////  @param      target_angle:Ŀ��Ƕ�
////  @param      current:��ǰ�Ƕ�
////  @param      gyro_x:�Ǽ��ٶ�
////  @return
////  @since
////-------------------------------------------------------------------------------------------------------------------
float Vertical(float target_angle, float current_angle, float gyro_x)
{
    float Vetical_PWM;
    static float error;

    error += current_angle - target_angle;

    if(error>+8) error=+8;                                          //�����޷�
    if(error<-8) error=-8;                                          //�����޷�

    Vetical_PWM = Balance_Param.Angle_PID.kp * (current_angle - target_angle) +
                  Balance_Param.Angle_PID.ki * error +
                  Balance_Param.Angle_PID.kd * (gyro_x);

    return Vetical_PWM;
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      �ٶȻ�
////  @param      encoder:ת���ٶ�
////  @return
////  @since      ������
////-------------------------------------------------------------------------------------------------------------------
float Velocity(float encoder)
{
    static float Encoder, Encoder_Integral;
    float Velocity_PWM, Encoder_Least;

    Encoder_Least = encoder;                                                  //�ٶ��˲�
    Encoder *= 0.7;                                                           //һ�׵�ͨ�˲���
    Encoder += Encoder_Least*0.3;                                             //һ�׵�ͨ�˲���
    Encoder_Integral += Encoder;                                              //���ֳ�λ��

    if(Encoder_Integral > +200) Encoder_Integral = +200;                    //�����޷�
    if(Encoder_Integral < -200) Encoder_Integral = -200;                    //�����޷�

    Velocity_PWM = Balance_Param.Velocity_PID.kp * Encoder +
                   Balance_Param.Velocity_PID.ki/100 * Encoder_Integral;//��ȡ������ֵ

    if(Momentum_Motor_Flag == 0) Encoder_Integral=0, Encoder=0, Velocity_PWM=0;                //ֹͣʱ��������

    return Velocity_PWM;
}

////-------------------------------------------------------------------------------------------------------------------
////  @brief      ƽ���ܿ���
////  @param      void
////  @return     void
////  @since      ִ������:5ms
////-------------------------------------------------------------------------------------------------------------------
void Balance_Control(void)
{
    Momentum_Speed = (float)FOC.BLmotor.speed;
    Dynamic_Balance_Angle = Middle_Angle_Counting(Servo_Param.Head_Angle, Speed_Param.Speed);

    Vetical_PWM  = Vertical(Dynamic_Balance_Angle, eulerAngle.roll, icm_gyro_x);
    Velocity_PWM = Velocity(Momentum_Speed);
#if 0  //����˲����ٸ���ƽ�⣬ûʱ���
    if(FOC.Ref_Park.q > 3.6 && Total_Image.road_type != start && Total_Image.road_type != stop)
    {
        Speed_Param.Speed_Setup += 5.1;
    }
    else
    {
        Speed_Param.Speed_Setup = Speed_Param.Normal_Speed;
    }
#endif
    Momentum_Motor_Duty = (Vetical_PWM - Velocity_PWM);

    if(eulerAngle.roll > Dynamic_Balance_Angle + 12 || eulerAngle.roll < Dynamic_Balance_Angle - 12)//ˤ���жϣ�������������
    {
        Momentum_Motor_Flag = 0;
    }else
    {
        Momentum_Motor_Flag = 1;
    }
    if(Momentum_Motor_Flag == 0)                       // ͣ��
    {
        Momentum_Motor_Duty = 0;                      // ����ر�

    }

    Momemtum_Motor_Control(Momentum_Motor_Duty);
}
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ƽ��������ݴ�ӡ
////  @param      void
////  @return     void
////  @since      ����ʱ����main.cʹ�ã���ʹ��ʱ�ǵùر�
////-------------------------------------------------------------------------------------------------------------------
void BANLANCE_TEST_PRINTF(void)
{
//    printf("%d\r\n", hall_value_now);
//    printf("%.2f, %.2f\r\n", Head_Angle, Dynamic_Middle_Angle);//������
    printf("%.2f,%f,%f,%f\r\n",eulerAngle.roll, Dynamic_Balance_Angle, Momentum_Speed, Momentum_Motor_Duty);//������
}

////-------------------------------------------------------------------------------------------------------------------
////  @brief      ƽ���ܿ��Ƴ�ʼ��
////  @param      void
////  @return     void
////  @since 
////-------------------------------------------------------------------------------------------------------------------

void Banlance_Algorithm_Init(void)
{
    Balance_Param_Init();
}
