#include "attitude_algorithm.h" 


#define delta_T      0.005f  //5ms����һ��
#define M_PI 3.1425f

float I_ex, I_ey, I_ez;  // ������


quaterInfo_t Q_info = {1, 0, 0};  // ȫ����Ԫ��
eulerianAngles_t eulerAngle; //ŷ����
gyro_param_t GyroOffset;

//float Q_info;
//float eulerAngle;

float param_Kp = 1.0;   // ���ٶȼ�(������)���������ʱ�������50
float param_Ki = 0.20;   //�������������ʵĻ������� 0.2

float values[10];



float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}




/*
    ����ת��
    get accel and gyro from iam20602
    ��accelһ�׵�ͨ�˲�(�ο�����)����gyroת�ɻ���ÿ��(2000dps)
*/

#define new_weight           0.35f
#define old_weight           0.65f

void IMU_getValues(float *values)
{
    static double lastaccel[3]= {0,0,0};
    int i;
    values[0] = ((float)icm_acc_x) * new_weight + lastaccel[0] * old_weight;
    values[1] = ((float)icm_acc_y) * new_weight + lastaccel[1] * old_weight;
    values[2] = ((float)icm_acc_z) * new_weight + lastaccel[2] * old_weight;
    for(i=0; i<3; i++)
    {
        lastaccel[i] = values[i];
    }

    values[3] = ((float)icm_gyro_x-GyroOffset.Xdata) * M_PI / 180 / 16.4f;
    values[4] = ((float)icm_gyro_y-GyroOffset.Ydata) * M_PI / 180 / 16.4f;
    values[5] = ((float)icm_gyro_z-GyroOffset.Zdata) * M_PI / 180 / 16.4f;

//    values[0] = ((float)icm_acc_x);
//    values[1] = ((float)icm_acc_y);
//    values[2] = ((float)icm_acc_z);
}


/**
  * brief IMU_AHRSupdate_noMagnetic  ��̬�����ںϣ���Crazepony�ͺ����㷨
  * ʹ�õ��ǻ����˲��㷨��û��ʹ��Kalman�˲��㷨
  * param float gx, float gy, float gz, float ax, float ay, float az
  *
  * return None
  */
static void IMU_AHRSupdate_noMagnetic(float gx, float gy, float gz, float ax, float ay, float az)
{
    float halfT = 0.5 * delta_T;
    float vx, vy, vz;    //��ǰ�Ļ�������ϵ�ϵ�������λ����
    float ex, ey, ez;    //��Ԫ������ֵ����ٶȼƲ���ֵ�����
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
//    float q0q3 = q0 * q3;
    float q1q1 = q1 * q1;
//    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;
    // float delta_2 = 0;

    //�Լ��ٶ����ݽ��й�һ�� �õ���λ���ٶ�
    float norm = invSqrt(ax*ax + ay*ay + az*az);
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

    vx = 2*(q1q3 - q0q2);
    vy = 2*(q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;

    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;

    //�ò���������PI����������ƫ��
    //ͨ������ param_Kp��param_Ki ����������
    //���Կ��Ƽ��ٶȼ����������ǻ�����̬���ٶȡ�
    I_ex += delta_T * ex;   // integral error scaled by Ki
    I_ey += delta_T * ey;
    I_ez += delta_T * ez;

    gx = gx+ param_Kp*ex + param_Ki*I_ex;
    gy = gy+ param_Kp*ey + param_Ki*I_ey;
    gz = gz+ param_Kp*ez + param_Ki*I_ez;


    /*����������ɣ���������Ԫ��΢�ַ���*/


    //��Ԫ��΢�ַ��̣�����halfTΪ�������ڵ�1/2��gx gy gzΪ�����ǽ��ٶȣ����¶�����֪��������ʹ����һ��������������Ԫ��΢�ַ���
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = q1 + ( q0*gx + q2*gz - q3*gy)*halfT;
    q2 = q2 + ( q0*gy - q1*gz + q3*gx)*halfT;
    q3 = q3 + ( q0*gz + q1*gy - q2*gx)*halfT;
    //    delta_2=(2*halfT*gx)*(2*halfT*gx)+(2*halfT*gy)*(2*halfT*gy)+(2*halfT*gz)*(2*halfT*gz);
    // ������Ԫ����    ��Ԫ��΢�ַ���  ��Ԫ�������㷨�����ױϿ���
    //    q0 = (1-delta_2/8)*q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    //    q1 = (1-delta_2/8)*q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    //    q2 = (1-delta_2/8)*q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    //    q3 = (1-delta_2/8)*q3 + (q0*gz + q1*gy - q2*gx)*halfT;
    // normalise quaternion
    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    Q_info.q0 = q0 * norm;
    Q_info.q1 = q1 * norm;
    Q_info.q2 = q2 * norm;
    Q_info.q3 = q3 * norm;
}



/*����Ԫ��ת����ŷ����*/
void IMU_quaterToEulerianAngles(void)
{
    IMU_getValues(values);
    IMU_AHRSupdate_noMagnetic(values[3], values[4], values[5], values[0], values[1], values[2]);

    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;
    eulerAngle.pitch = asin(-2*q1*q3 + 2*q0*q2) * 180/M_PI; // pitch
    eulerAngle.roll = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * 180/M_PI; // roll
    eulerAngle.yaw = atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1) * 180/M_PI; // yaw

/*   ���Բ�������̬�޶ȵ�����*/
    if(eulerAngle.roll>90 || eulerAngle.roll<-90)
    {
        if(eulerAngle.pitch > 0)
        {
            eulerAngle.pitch = 180-eulerAngle.pitch;
        }
        if(eulerAngle.pitch < 0)
        {
            eulerAngle.pitch = -(180+eulerAngle.pitch);
        }
    }
//    if(eulerAngle.yaw > 180)
//    {
//        eulerAngle.yaw -=360;
//    }
//    else if(eulerAngle.yaw <-180)
//    {
//        eulerAngle.yaw +=360;
//    }

    if (eulerAngle.yaw > 360)
    {
        eulerAngle.yaw -= 360;
    }
    else if (eulerAngle.yaw < 0)
    {
        eulerAngle.yaw += 360;
    }
}

void linearSmooth7 ( double in[], double out[], int N )
{
    int i;
    if ( N < 7 )
    {
        for ( i = 0; i <= N - 1; i++ )
        {
            out[i] = in[i];
        }
    }
    else
    {
        out[0] = ( 13.0 * in[0] + 10.0 * in[1] + 7.0 * in[2] + 4.0 * in[3] +
                  in[4] - 2.0 * in[5] - 5.0 * in[6] ) / 28.0;

        out[1] = ( 5.0 * in[0] + 4.0 * in[1] + 3 * in[2] + 2 * in[3] +
                  in[4] - in[6] ) / 14.0;

        out[2] = ( 7.0 * in[0] + 6.0 * in [1] + 5.0 * in[2] + 4.0 * in[3] +
                  3.0 * in[4] + 2.0 * in[5] + in[6] ) / 28.0;

        for ( i = 3; i <= N - 4; i++ )
        {
            out[i] = ( in[i - 3] + in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2] + in[i + 3] ) / 7.0;
        }

        out[N - 3] = ( 7.0 * in[N - 1] + 6.0 * in [N - 2] + 5.0 * in[N - 3] +
                      4.0 * in[N - 4] + 3.0 * in[N - 5] + 2.0 * in[N - 6] + in[N - 7] ) / 28.0;

        out[N - 2] = ( 5.0 * in[N - 1] + 4.0 * in[N - 2] + 3.0 * in[N - 3] +
                      2.0 * in[N - 4] + in[N - 5] - in[N - 7] ) / 14.0;

        out[N - 1] = ( 13.0 * in[N - 1] + 10.0 * in[N - 2] + 7.0 * in[N - 3] +
                      4 * in[N - 4] + in[N - 5] - 2 * in[N - 6] - 5 * in[N - 7] ) / 28.0;
    }
}


void Attitude_Algorithm(void)
{
    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();
    IMU_quaterToEulerianAngles();
}


void gyroOffset_init(void) /////////��������Ʈ��ʼ��
{
    GyroOffset.Xdata = 0;
    GyroOffset.Ydata = 0;
    GyroOffset.Zdata = 0;
    for (uint16 i = 0; i < 100; ++i)
    {
        get_icm20602_gyro_spi();
        get_icm20602_accdata_spi();
        GyroOffset.Xdata += icm_gyro_x;
        GyroOffset.Ydata += icm_gyro_y;
        GyroOffset.Zdata += icm_gyro_z;
        systick_delay_ms(STM0, 10);
    }

    GyroOffset.Xdata /= 100;
    GyroOffset.Ydata /= 100;
    GyroOffset.Zdata /= 100;
}

void Attitude_Algorithm_Init(void)
{
    icm20602_init_spi();        //δ����icm20602ʱ�������,�Է���ѭ��
    gyroOffset_init();          //������Yaw������,��Ҫ������ֹ1s
}

