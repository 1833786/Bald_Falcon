#include "encoder.h"

void Motor_Encoder_Init(void)
{
    gpt12_init(Motor_TIM, Motor_LSB, Motor_Dir);
}



float Motor_Speed(void)
{
    float speed;

    speed = (float)gpt12_get(Motor_TIM);
    gpt12_clear(Motor_TIM);
    return speed;
    //��ӡ�ɼ����ı��������ݣ�������ѧϰprintf���̣��˽����ʹ��printf
}
