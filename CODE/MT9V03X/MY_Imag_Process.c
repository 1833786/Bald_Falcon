#include "MY_Imag_Process.h"

Uint8 Cut_Image[Cut_Imag_Y][Cut_Imag_X];
Uint8 Binary_Image[Binary_Imag_Y][Binary_Imag_X];
OTSU_FLITER OTSU_Struct;
TOTAL_IMAGE Total_Image;

/* *************************��·���************************************* */
const Uint8 HARF_ROAD_WIDTH[60] = {
        53,53,52,51,51,50,50,50,48,48,
        48,47,47,46,46,45,45,49,48,48,
        48,57,44,45,48,49,50,58,60,63,
        63,58,48,49,51,52,52,53,53,54,
        54//41
};
/* ************************************************************************* */

/* *************************������غ���************************************* */
void Queue_Init(C_D_QUEUE *Q)
{
    Q->rear = 0;
    Q->front = 0;
}

void Enter_Queue(C_D_QUEUE *Q, Uint8 data_x, Uint8 data_y)
{
    Q->data[Q->rear].x = data_x;
    Q->data[Q->rear].y = data_y;
    Q->rear = (Q->rear+1)%2;
}

void Pop_Queue(C_D_QUEUE *Q, ARRAY *data_out)
{
    (*data_out).x = Q->data[Q->front].x;
    (*data_out).y = Q->data[Q->front].y;
    Q->front = (Q->front+1)%2;
}
/* ************************************************************************* */

/* *************************ջ��غ���************************************** */
void Stack_Init(E_F_STACK *S)
{
    S->top = -1;
}

void Enter_Stack(E_F_STACK *S, Uint8 data_x, Uint8 data_y)
{
    S->top++;
    S->data[S->top].x =  data_x;
    S->data[S->top].y =  data_y;
}

void Pop_Stack(E_F_STACK *S, ARRAY *data_out)
{
    (*data_out).x = S->data[S->top].x;
    (*data_out).y = S->data[S->top].y;
    S->top--;
}

Uint8 Stack_Full(E_F_STACK *S)
{
    if (S->top == Stack_Size-1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

Uint8 Stack_Length(E_F_STACK *S)
{
    return (S->top)+1;
}
/* ************************************************************************* */

/* **************************ȫ�ֳ�ʼ��************************************* */
void Global_Init(void)
{
    memset(Total_Image.special_point, 0, sizeof(Total_Image.special_point));
    memset(&Border_Image, 255, sizeof(Border_Image));
}
/* ************************************************************************* */

/*
 * @brief ��ͼ�����������2
 * @param ����ͼ���ַ
 * @param ���ͼ���ַ
 * */
void Image_Cut(Uint8 *Image_In, Uint8 *Image_Out)
{
    Uint8 *ptr = Image_In;
    Uint8 *ptd = Image_Out;
    Int16 y = -1;
    Int16 x = -1;

    while((++y) < Cut_Imag_Y)
    {
        x = -1;
        while((++x) < Cut_Imag_X)
        {
            *(ptd+y*Cut_Imag_X+x) = *(ptr+y*2*Cut_Imag_X+x*2);
        }
    }
}

/*
 * @brief �������ֵ���Ҷ�ֵ��ͼ��
 * @param ����ͼ���ַ
 * @param ͼ����
 * @param ͼ��߶�
 * @param ��ֵ
 * */
void OTSU_Filter(Uint8 *Image_In, Uint8 *Image_Out, Uint8 Image_x, Uint8 Image_y)
{
    Uint8 *ptr = Image_In;//ͼ����ʼ��ַ
    Uint8 *ptrend = Image_In+Image_x*Image_y;//ͼ���յ��ַ
    Uint16 histogram[256] = {0};//ɫ��Ƶ��ֱ��ͼ
    Uint8 min_value = 0, max_value = 255;//ɫ�ص����ֵ����Сֵ
    Uint32 PixelAmount = 0, Value_Sum = 0;//�����ܺͣ��Ҷ��ܺ�
    Uint32 Pixel_Integral[256] = {0};//���ػ���
    Uint32 Value_Integral[256] = {0};//�ҶȻ���
    Uint64 sigma = 0, maxSigma = 0;
    float w1 = 0, w2 = 0;
    Int32 u1 = 0, u2 = 0;
    Uint16 OTSUThreshold;

    while(ptr != ptrend)
    {
        histogram[*ptr]++;
        ptr++;
    }
    Uint8 i = 0;
    while(i < 256)
    {
        if (histogram[i] != 0)
        {
            min_value = i;
            break;
        }
        i++;
    }

    i = 255;

    while(i >= 0)
    {
       if (histogram[i] != 0)
        {
            max_value = i;
            break;
        }
       i--;
    }

    i = min_value;
    while(i < max_value)//��������õ���Uint8�����ͣ���ô��ֻ�ܼ��㵽254�ģ��޷�����255������ֵ������Ŵ�i�����ͣ�����������Ӱ�촦���ٶ�
    {
        PixelAmount += histogram[i];      //��������
        Value_Sum += histogram[i] * i;     //�Ҷ��ܺ�
        Pixel_Integral[i] = PixelAmount;
        Value_Integral[i] = Value_Sum;
        i++;
    }

    i = min_value;
    while(i < max_value)
    {
        w1 = (float)Pixel_Integral[i] / PixelAmount;//ǰ�����ص������Ҫע�������ǵ�����Pixel_Integral[i]����ǿ��ת��������ͻ���Զ���0�Ľ��
        w2 = 1-w1;//��������
        u1 = (Int32) (Value_Integral[i] / w1);                   //ǰ��ƽ���Ҷ�
        u2 = (Int32) ((Value_Sum - Value_Integral[i]) / w2);      //����ƽ���Ҷ�
        sigma = (Uint64) (w1 * w2 * (u1 - u2) * (u1 - u2));
        if (sigma >= maxSigma)
        {
            maxSigma = sigma;
            OTSUThreshold = i;
        }
        else
        {
            break;
        }
        i++;
    }

    //ͼ���ֵ�����
    ptr = Image_In;
    ptrend = Image_Out;
    for (Uint8 i = 0; i < Image_y; i++)
    {
        for (Uint8 j = 0; j < Image_x; j++)
        {
            if (*(i*Image_x+ptr+j) < OTSUThreshold) *(i*Image_x+ptrend+j) = 0;
            else *(i*Image_x+ptrend+j) = 255;
        }
    }
    OTSU_Struct.OTSUThreshold = OTSUThreshold;
}

/*
 * @Brief ���ߺ���ٴα߽���ȡ
 * @param ����ͼ���ַ
 * @param ���ͼ���ַ
 * @param ͼ����
 * @param ͼ��߶�
 * */
void Re_Edge_Extraction(Uint8 *Image_In, Uint8 Image_x, Uint8 Image_y)
{
    Uint8 *ptr = Image_In;
    Uint8 start_x = Image_x/2;
    Uint8 start_y = Image_y-2;
    Uint8 error_type = 0;
    Uint8 clockwise_anticlockwise = 0;//˳ʱ������������ʱ��������
    Uint8 C_findout = 0;
    Uint8 D_findout = 0;
    Uint8 left_border_line_cnt = 0;
    Uint8 right_border_line_cnt = 0;
    ARRAY A_point, B_point;
    ARRAY last_Without_Back_point;
    C_D_QUEUE Without_Back_Queue;

    //�ҵ���߽��
    for (Uint8 i = start_x; i > 0; i--)
    {
        if (*(start_y*Image_x+ptr+i) - *(start_y*Image_x+ptr+i-1)== 255)
        {
            A_point.x = i-1;
            A_point.y = start_y;
            break;
        }
        if (i == 1)
        {
            A_point.x = i-1;
            A_point.y = start_y;
            error_type = A_point_error;
            break;
        }
    }

    //�ҵ��ұ߽��
    for (Uint8 i = start_x; i < 93; i++)
    {
        if (*(start_y*Image_x+ptr+i) - *(start_y*Image_x+ptr+i+1) == 255)
        {
            B_point.x = i+1;
            B_point.y = start_y;
            break;
        }
        if (i == 92)
        {
            B_point.x = i+1;
            B_point.y = start_y;
            error_type = B_point_error;
            break;
        }
    }

    //�����������
    start_x = A_point.x;
    start_y = A_point.y-1;
    Queue_Init(&Without_Back_Queue);
    Enter_Queue(&Without_Back_Queue, start_x, start_y+1);
    Enter_Queue(&Without_Back_Queue, start_x, start_y);
    Pop_Queue(&Without_Back_Queue, &last_Without_Back_point);
    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
    left_border_line_cnt++;
    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
    left_border_line_cnt++;
    for (Uint8 i = 0; i < 60; i++)
    {
        if (clockwise_anticlockwise == 0)
        {
            if (*(start_y*Image_x+ptr+start_x+1) == 0 && last_Without_Back_point.x != (start_x+1))
            {
                start_x = start_x + 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y - 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*(start_y*Image_x+ptr+start_x-1) == 0)
            {
                start_x = start_x - 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y + 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
                if (C_findout == 0) C_findout = 1;
                clockwise_anticlockwise = 1;//�任ѭ�߷���
            }
        }
        else if (clockwise_anticlockwise == 1)
        {
            if (*(start_y*Image_x+ptr+start_x-1) == 0)
            {
                start_x = start_x - 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y - 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*(start_y*Image_x+ptr+start_x+1) == 0 && last_Without_Back_point.x != (start_x+1))
            {
                start_x = start_x + 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
            else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y + 1;
                *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                left_border_line_cnt++;
            }
        }

        Enter_Queue(&Without_Back_Queue, start_x, start_y);
        Pop_Queue(&Without_Back_Queue, &last_Without_Back_point);

        if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
        {
            error_type = C_point_to_border;
            break;
        }
    }

    //�����ұ�����
    start_x = B_point.x;
    start_y = B_point.y-1;
    clockwise_anticlockwise = 0;
    Queue_Init(&Without_Back_Queue);
    Enter_Queue(&Without_Back_Queue, start_x, start_y+1);
    Enter_Queue(&Without_Back_Queue, start_x, start_y);
    Pop_Queue(&Without_Back_Queue, &last_Without_Back_point);
    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
    right_border_line_cnt++;
    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
    right_border_line_cnt++;
    for (Uint8 i = 0; i < 60; i++)
    {
        if (clockwise_anticlockwise == 0)
        {
            if (*(start_y*Image_x+ptr+start_x-1) == 0 && last_Without_Back_point.x != (start_x-1))
            {
                start_x = start_x - 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y - 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*(start_y*Image_x+ptr+start_x+1) == 0)
            {
                start_x = start_x + 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y + 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
                if (D_findout == 0) D_findout = 1;
                clockwise_anticlockwise = 1;//�任ѭ�߷���
            }
        }
        else if (clockwise_anticlockwise == 1)
        {
            if (*(start_y*Image_x+ptr+start_x+1) == 0)
            {
                start_x = start_x + 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y - 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*(start_y*Image_x+ptr+start_x-1) == 0 && last_Without_Back_point.x != (start_x-1))
            {
                start_x = start_x - 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
            else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y + 1;
                *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                right_border_line_cnt++;
            }
        }

        Enter_Queue(&Without_Back_Queue, start_x, start_y);
        Pop_Queue(&Without_Back_Queue, &last_Without_Back_point);

        if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
        {
            error_type = D_point_to_border;
            break;
        }
    }

    Total_Image.left_line_cnt = left_border_line_cnt;
    Total_Image.right_line_cnt = right_border_line_cnt;
}

/*
 * @Brief �߽���ȡ
 * @param ����ͼ���ַ
 * @param ���ͼ���ַ
 * @param ͼ����
 * @param ͼ��߶�
 * */
void Edge_Extraction(Uint8 *Image_In, Uint8 Image_x, Uint8 Image_y)
{
    Uint8 *ptr = Image_In;
    Uint8 start_x = Image_x/2;//��ͼ����м�������Ѱ��A,B��
    Uint8 start_y = Image_y-2;//��ͼ��ĵ����ڶ��п�ʼѰ��A,B��
    Uint8 left_border_line_cnt = 0;
    Uint8 right_border_line_cnt = 0;
    Uint8 white_line = 0;//��¼����հ���
    Uint8 left_white_line = 0;//��¼���հ���
    Uint8 right_white_line = 0;//��¼�Ҳ�հ���
//    Uint8 garage_left_white_line = 0;
    Uint8 left_black_point, right_black_point;//��¼ÿһ�е����߻����Ұ�ߺڵ�ĸ���
    ARRAY last_C_D_border_point;
    ARRAY last_J_L_border_point;
    ARRAY now_J_L_border_point;
    ARRAY now_G_H_border_point;
    ARRAY last_G_H_border_point;
    Uint8 clockwise_anticlockwise = 0;//˳ʱ������������ʱ��������
    Uint8 A_findout = 0;
    Uint8 B_findout = 0;
    Uint8 C_findout = 0;
    Uint8 D_findout = 0;
    Uint8 E_findout = 0;
    Uint8 F_findout = 0;
    Uint8 G_findout = 0;
    Uint8 H_findout = 0;
    Uint8 I_findout = 0;
    Uint8 J_findout = 0;
//    Uint8 L_findout = 0;
    Uint8 Round_about = 0;
    Uint8 Exit_Round_about = 0;
    C_D_QUEUE C_D_Queue;
    E_F_STACK E_F_Stack;
    E_F_STACK J_L_Stack;

    //�ҵ�A��
    for (Uint8 i = start_x; i > 0; i--)
    {
        if (*(start_y*Image_x+ptr+i) - *(start_y*Image_x+ptr+i-1)== 255)
        {
            Total_Image.special_point[A].x = i-1;
            Total_Image.special_point[A].y = start_y;
            A_findout = 1;
            break;
        }
        if (i == 1)
        {
            Total_Image.special_point[A].x = i-1;
            Total_Image.special_point[A].y = start_y;
            Total_Image.error = A_point_error;
            break;
        }
    }

    //�ҵ�B��
    for (Uint8 i = start_x; i < 93; i++)
    {
        if (*(start_y*Image_x+ptr+i) - *(start_y*Image_x+ptr+i+1) == 255)
        {
            Total_Image.special_point[B].x = i+1;
            Total_Image.special_point[B].y = start_y;
            B_findout = 1;
            break;
        }
        if (i == 92)
        {
            Total_Image.special_point[B].x = i+1;
            Total_Image.special_point[B].y = start_y;
            Total_Image.error = B_point_error;
            break;
        }
    }

    //Ѱ��߽�60��,�ҵ�C��
    if (A_findout == 1)
    {
        start_x = Total_Image.special_point[A].x;
        start_y = Total_Image.special_point[A].y-1;
        Border_Image[start_y+1][start_x] = 0;
        Border_Image[start_y][start_x] = 0;
        *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
        left_border_line_cnt++;
        *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
        left_border_line_cnt++;
        Queue_Init(&C_D_Queue);
        Enter_Queue(&C_D_Queue, start_x, start_y+1);
        Enter_Queue(&C_D_Queue, start_x, start_y);
        Pop_Queue(&C_D_Queue, &last_C_D_border_point);

        for (Uint8 i = 0; i < 90; i++)
        {
            //��ʱ��������
            if (clockwise_anticlockwise == 0)
            {
                if (*(start_y*Image_x+ptr+start_x+1) == 0 && last_C_D_border_point.x != (start_x+1))
                {
                    start_x = start_x + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
                else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
                else if (*(start_y*Image_x+ptr+start_x-1) == 0)
                {
                    start_x = start_x - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                    //�����һ���յ�
                    if (C_findout == 0)
                    {
                        Total_Image.special_point[C].x = start_x+1;
                        Total_Image.special_point[C].y = start_y;
                        C_findout = 1;
                    }
                }
                else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                    if (C_findout == 0)
                    {
                        Total_Image.special_point[C].x = start_x;
                        Total_Image.special_point[C].y = start_y+1;
                        C_findout = 1;
                    }
                    clockwise_anticlockwise = 1;//�任ѭ�߷���
                }
            }
            //˳ʱ��������
            else if (clockwise_anticlockwise == 1)
            {
                if (*(start_y*Image_x+ptr+start_x-1) == 0)
                {
                    start_x = start_x - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
                else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
                else if (*(start_y*Image_x+ptr+start_x+1) == 0 && last_C_D_border_point.x != (start_x+1))
                {
                    start_x = start_x + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
                else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.left_border_line+left_border_line_cnt) = start_x;
                    left_border_line_cnt++;
                }
            }

            Enter_Queue(&C_D_Queue, start_x, start_y);
            Pop_Queue(&C_D_Queue, &last_C_D_border_point);

            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = C_point_to_border;
                break;
            }
        }
    }

    //Ѱ�ұ߽�60��,�ҵ�D��
    if (B_findout == 1)
    {
        start_x = Total_Image.special_point[B].x;
        start_y = Total_Image.special_point[B].y-1;
        Border_Image[start_y+1][start_x] = 0;
        Border_Image[start_y][start_x] = 0;
        *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
        right_border_line_cnt++;
        *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
        right_border_line_cnt++;
        clockwise_anticlockwise = 0;
        Queue_Init(&C_D_Queue);
        Enter_Queue(&C_D_Queue, start_x, start_y+1);
        Enter_Queue(&C_D_Queue, start_x, start_y);
        Pop_Queue(&C_D_Queue, &last_C_D_border_point);
        for (Uint8 i = 0; i < 90; i++)
        {
            //˳ʱ��������
            if (clockwise_anticlockwise == 0)
            {
                if (*(start_y*Image_x+ptr+start_x-1) == 0 && last_C_D_border_point.x != (start_x-1))
                {
                    start_x = start_x - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
                else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
                else if (*(start_y*Image_x+ptr+start_x+1) == 0)
                {
                    start_x = start_x + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                    //�����һ���յ�
                    if (D_findout == 0)
                    {
                        Total_Image.special_point[D].x = start_x-1;
                        Total_Image.special_point[D].y = start_y;
                        D_findout = 1;
                    }
                }
                else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                    clockwise_anticlockwise = 1;//�任ѭ�߷���
                    if (D_findout == 0)
                    {
                        Total_Image.special_point[D].x = start_x;
                        Total_Image.special_point[D].y = start_y-1;
                        D_findout = 1;
                    }
                }
            }
            //��ʱ��������
            else if (clockwise_anticlockwise == 1)
            {
                if (*(start_y*Image_x+ptr+start_x+1) == 0)
                {
                    start_x = start_x + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
                else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
                else if (*(start_y*Image_x+ptr+start_x-1) == 0 && last_C_D_border_point.x != (start_x-1))
                {
                    start_x = start_x - 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
                else if (*((start_y+1)*Image_x+ptr+start_x) == 0)
                {
                    start_y = start_y + 1;
                    Border_Image[start_y][start_x] = 0;
                    *(Total_Image.right_border_line+right_border_line_cnt) = start_x;
                    right_border_line_cnt++;
                }
            }

            Enter_Queue(&C_D_Queue, start_x, start_y);
            Pop_Queue(&C_D_Queue, &last_C_D_border_point);

            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = D_point_to_border;
                break;
            }
        }
    }

    Total_Image.left_line_cnt = left_border_line_cnt;
    Total_Image.right_line_cnt = right_border_line_cnt;

    //C,D����������,�ҵ�E,F��
    if (Total_Image.special_point[C].y > 30 && Total_Image.special_point[D].y > 30 && ABS(Total_Image.special_point[C].y - Total_Image.special_point[D].y) < 30 && Total_Image.crossing_road_status != second_crossing_road_way_in)
    {
        //�ҵ�E��
        start_y = Total_Image.special_point[C].y-1;
        start_x = Total_Image.special_point[C].x;
        for (Uint8 i = start_y; i > 0; i--)
        {
            if (*(i*Image_x+ptr+start_x) - *((i-1)*Image_x+ptr+start_x) == 255)
            {
                Total_Image.special_point[E].x = start_x;
                Total_Image.special_point[E].y = i-1;
                E_findout = 1;
                break;
            }
            if (i == 1)
            {
                Total_Image.special_point[E].x = start_x;
                Total_Image.special_point[E].y = i-1;
                Total_Image.error = E_point_error;
                break;
            }
        }

        //�ҵ�F��
        start_y = Total_Image.special_point[D].y-1;
        start_x = Total_Image.special_point[D].x;
        for (Uint8 i = start_y; i > 0; i--)
        {
            if (*(i*Image_x+ptr+start_x) - *((i-1)*Image_x+ptr+start_x) == 255)
            {
                Total_Image.special_point[F].x = start_x;
                Total_Image.special_point[F].y = i-1;
                F_findout = 1;
                break;
            }
            if (i == 1)
            {
                Total_Image.special_point[F].x = start_x;
                Total_Image.special_point[F].y = i-1;
                Total_Image.error = F_point_error;
                break;
            }
        }
    }

    //�ҵ�E,F��G,H��
    if (E_findout == 1 && F_findout == 1)
    {
        //��G��
        start_x = Total_Image.special_point[E].x;
        start_y = Total_Image.special_point[E].y;
        Border_Image[start_y][start_x] = 0;
        Stack_Init(&E_F_Stack);
        for (Uint8 i = 0; i < 30; i++)
        {
            if (*(start_y*Image_x+ptr+start_x+1) == 0)
            {
                start_x = start_x + 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y -1;
                Border_Image[start_y][start_x] = 0;
                if (G_findout == 0)
                {
                    Enter_Stack(&E_F_Stack, start_x, start_y+1);
                }
            }
            else
            {
                break;
            }

            //�ȵ�ջ��
            if (Stack_Full(&E_F_Stack) == 1 && G_findout == 0)
            {
                Pop_Stack(&E_F_Stack, &last_G_H_border_point);
                for (Uint8 j = Stack_Size-1; j > 0; j--)
                {
                    Pop_Stack(&E_F_Stack, &now_G_H_border_point);
                    //�ҵ���һ��ջ�������ڵ�����X��ƫ��ϴ���Ǹ���Ϊʮ���Ϲյ�
                    if (ABS(last_G_H_border_point.x - now_G_H_border_point.x) > 3)
                    {
                        Total_Image.special_point[G].x = last_G_H_border_point.x;
                        Total_Image.special_point[G].y = last_G_H_border_point.y;
                        G_findout = 1;
                        break;
                    }
                    else
                    {
                        last_G_H_border_point.x = now_G_H_border_point.x;
                        last_G_H_border_point.y = now_G_H_border_point.y;
                    }
                    //����ƫ���С�Ļ�,ȡջ�׵�������Ϊʮ���Ϲյ�
                    if (j == 1)
                    {
                        Total_Image.special_point[G].x = last_G_H_border_point.x;
                        Total_Image.special_point[G].y = last_G_H_border_point.y;
                        G_findout = 1;
                    }
                }
            }

            //�ж��Ƿ�Խ��
            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = G_point_to_border;
                break;
            }
        }

        //��H��
        start_x = Total_Image.special_point[F].x;
        start_y = Total_Image.special_point[F].y;
        Border_Image[start_y][start_x] = 0;
        Stack_Init(&E_F_Stack);
        for (Uint8 i = 0; i < 30; i++)
        {
            if (*(start_y*Image_x+ptr+start_x-1) == 0)
            {
                start_x = start_x - 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y -1;
                Border_Image[start_y][start_x] = 0;
                if (H_findout == 0)
                {
                    Enter_Stack(&E_F_Stack, start_x, start_y+1);
                }
            }
            else
            {
                break;
            }

            //�ȵ�ջ��
            if (Stack_Full(&E_F_Stack) == 1 && H_findout == 0)
            {
                Pop_Stack(&E_F_Stack, &last_G_H_border_point);
                for (Uint8 j = Stack_Size-1; j > 0; j--)
                {
                    Pop_Stack(&E_F_Stack, &now_G_H_border_point);
                    //�ҵ���һ��ջ�������ڵ�����X��ƫ��ϴ���Ǹ���Ϊʮ���Ϲյ�
                    if (ABS(now_G_H_border_point.x - last_G_H_border_point.x) > 3)
                    {
                        Total_Image.special_point[H].x = last_G_H_border_point.x;
                        Total_Image.special_point[H].y = last_G_H_border_point.y;
                        H_findout = 1;
                        break;
                    }
                    else
                    {
                        last_G_H_border_point.x = now_G_H_border_point.x;
                        last_G_H_border_point.y = now_G_H_border_point.y;
                    }
                    //����ƫ���С�Ļ�,ȡջ�׵�������Ϊʮ���Ϲյ�
                    if (j == 1)
                    {
                        Total_Image.special_point[H].x = last_G_H_border_point.x;
                        Total_Image.special_point[H].y = last_G_H_border_point.y;
                        H_findout = 1;
                    }
                }
            }

            //�ж��Ƿ�Խ��
            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = H_point_to_border;
                break;
            }
        }
    }

    //C,D����������,�ҵ�E,F��
    if (Total_Image.special_point[C].y > 45 && Total_Image.special_point[D].y > 45 && ABS(Total_Image.special_point[C].y - Total_Image.special_point[D].y) < 30 && Total_Image.crossing_road_status == second_crossing_road_way_in)
    {
        //�ҵ�E��
        start_y = Total_Image.special_point[C].y-1;
        start_x = Total_Image.special_point[C].x;
        for (Uint8 i = start_y; i > 0; i--)
        {
            if (*(i*Image_x+ptr+start_x) - *((i-1)*Image_x+ptr+start_x) == 255)
            {
                Total_Image.special_point[E].x = start_x;
                Total_Image.special_point[E].y = i-1;
                E_findout = 1;
                break;
            }
            if (i == 1)
            {
                Total_Image.special_point[E].x = start_x;
                Total_Image.special_point[E].y = i-1;
                Total_Image.error = E_point_error;
                break;
            }
        }

        //�ҵ�F��
        start_y = Total_Image.special_point[D].y-1;
        start_x = Total_Image.special_point[D].x;
        for (Uint8 i = start_y; i > 0; i--)
        {
            if (*(i*Image_x+ptr+start_x) - *((i-1)*Image_x+ptr+start_x) == 255)
            {
                Total_Image.special_point[F].x = start_x;
                Total_Image.special_point[F].y = i-1;
                F_findout = 1;
                break;
            }
            if (i == 1)
            {
                Total_Image.special_point[F].x = start_x;
                Total_Image.special_point[F].y = i-1;
                Total_Image.error = F_point_error;
                break;
            }
        }
    }

    //�ҵ�E,F��G,H��
    if (E_findout == 1 && F_findout == 1)
    {
        //��G��
        start_x = Total_Image.special_point[E].x;
        start_y = Total_Image.special_point[E].y;
        Border_Image[start_y][start_x] = 0;
        Stack_Init(&E_F_Stack);
        for (Uint8 i = 0; i < 30; i++)
        {
            if (*(start_y*Image_x+ptr+start_x+1) == 0)
            {
                start_x = start_x + 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y -1;
                Border_Image[start_y][start_x] = 0;
                if (G_findout == 0)
                {
                    Enter_Stack(&E_F_Stack, start_x, start_y+1);
                }
            }
            else
            {
                break;
            }

            //�ȵ�ջ��
            if (Stack_Full(&E_F_Stack) == 1 && G_findout == 0)
            {
                Pop_Stack(&E_F_Stack, &last_G_H_border_point);
                for (Uint8 j = Stack_Size-1; j > 0; j--)
                {
                    Pop_Stack(&E_F_Stack, &now_G_H_border_point);
                    //�ҵ���һ��ջ�������ڵ�����X��ƫ��ϴ���Ǹ���Ϊʮ���Ϲյ�
                    if (ABS(last_G_H_border_point.x - now_G_H_border_point.x) > 3)
                    {
                        Total_Image.special_point[G].x = last_G_H_border_point.x;
                        Total_Image.special_point[G].y = last_G_H_border_point.y;
                        G_findout = 1;
                        break;
                    }
                    else
                    {
                        last_G_H_border_point.x = now_G_H_border_point.x;
                        last_G_H_border_point.y = now_G_H_border_point.y;
                    }
                    //����ƫ���С�Ļ�,ȡջ�׵�������Ϊʮ���Ϲյ�
                    if (j == 1)
                    {
                        Total_Image.special_point[G].x = last_G_H_border_point.x;
                        Total_Image.special_point[G].y = last_G_H_border_point.y;
                        G_findout = 1;
                    }
                }
            }

            //�ж��Ƿ�Խ��
            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = G_point_to_border;
                break;
            }
        }

        //��H��
        start_x = Total_Image.special_point[F].x;
        start_y = Total_Image.special_point[F].y;
        Border_Image[start_y][start_x] = 0;
        Stack_Init(&E_F_Stack);
        for (Uint8 i = 0; i < 30; i++)
        {
            if (*(start_y*Image_x+ptr+start_x-1) == 0)
            {
                start_x = start_x - 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y -1;
                Border_Image[start_y][start_x] = 0;
                if (H_findout == 0)
                {
                    Enter_Stack(&E_F_Stack, start_x, start_y+1);
                }
            }
            else
            {
                break;
            }

            //�ȵ�ջ��
            if (Stack_Full(&E_F_Stack) == 1 && H_findout == 0)
            {
                Pop_Stack(&E_F_Stack, &last_G_H_border_point);
                for (Uint8 j = Stack_Size-1; j > 0; j--)
                {
                    Pop_Stack(&E_F_Stack, &now_G_H_border_point);
                    //�ҵ���һ��ջ�������ڵ�����X��ƫ��ϴ���Ǹ���Ϊʮ���Ϲյ�
                    if (ABS(now_G_H_border_point.x - last_G_H_border_point.x) > 3)
                    {
                        Total_Image.special_point[H].x = last_G_H_border_point.x;
                        Total_Image.special_point[H].y = last_G_H_border_point.y;
                        H_findout = 1;
                        break;
                    }
                    else
                    {
                        last_G_H_border_point.x = now_G_H_border_point.x;
                        last_G_H_border_point.y = now_G_H_border_point.y;
                    }
                    //����ƫ���С�Ļ�,ȡջ�׵�������Ϊʮ���Ϲյ�
                    if (j == 1)
                    {
                        Total_Image.special_point[H].x = last_G_H_border_point.x;
                        Total_Image.special_point[H].y = last_G_H_border_point.y;
                        H_findout = 1;
                    }
                }
            }

            //�ж��Ƿ�Խ��
            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                Total_Image.error = H_point_to_border;
                break;
            }
        }
    }

    //��G,H���������޶�
    if (G_findout == 1 && H_findout == 1)
    {
        if (Total_Image.special_point[H].x < Total_Image.special_point[G].x)
        {
            G_findout = 0;
            H_findout = 0;
        }
    }

    //������·ʱ��ȡI��������
    if (C_findout == 1 && D_findout == 1 && G_findout == 0 && H_findout == 0 && I_findout == 0)
    {
        ARRAY temp_point;
        temp_point.x = (Total_Image.special_point[C].x + Total_Image.special_point[D].x)/2;
        temp_point.y = Total_Image.special_point[C].y;
        for (Uint8 i = temp_point.y; i > 0; i--)
        {
            if (*(ptr+temp_point.x+i*Image_x) - *(ptr+temp_point.x+(i-1)*Image_x) == 255)
            {
                Total_Image.special_point[I].x = temp_point.x;
                Total_Image.special_point[I].y = i-1;
                I_findout = 1;
                break;
            }
        }
    }

//    //�һ���ʱ��ȡ���ϽǵĹյ�
//    if (Total_Image.road_type == right_round_about_status4 || Total_Image.road_type == right_round_about_status5 || Total_Image.road_type == right_round_about_status10 || Total_Image.road_type == right_round_about_status11)
//    {
//        start_x = Total_Image.special_point[A].x;
//        start_y = 7;
//        //�ҵ��ұ߽��Ϸ��ĺڵ�
//        for (Uint8 i = start_x; i < 92; i++)
//        {
//            if (*(ptr+(i+1)+start_y*Image_x) - *(ptr+i+start_y*Image_x) == 255)
//            {
//                start_x = i+1;
//                premise1_K_findout = 1;
//                break;
//            }
//        }
//        for (Uint8 i = start_x; i < 92; i++)
//        {
//            if (*(ptr+i+start_y*Image_x) - *(ptr+(i+1)+start_y*Image_x) == 255)
//            {
//                start_x = i+1;
//                premise2_K_findout = 1;
//                break;
//            }
//        }
//
//        //�ҵ����Ͻǹյ�
//        if (premise1_K_findout == 1 && premise2_K_findout == 1)
//        {
//            start_x = start_x + 1;
//            Queue_Init(&J_L_Queue);
//            Enter_Queue(&J_L_Queue, start_x-1, start_y);
//            Enter_Queue(&J_L_Queue, start_x, start_y);
//            Pop_Queue(&J_L_Queue, &last_J_L_border_point);
//            for (Uint8 i = 0; i < 70; i++)
//            {
//                if (*(ptr+start_x+(start_y+1)*Image_x) == 0 && last_J_L_border_point.y != start_y+1)
//                {
//                    start_y = start_y+1;
//                    Border_Image[start_y][start_x] = 0;
//                }
//                else if (*(ptr+start_x+1+start_y*Image_x) == 0)
//                {
//                    start_x = start_x+1;
//                    Border_Image[start_y][start_x] = 0;
//                }
//                else if (*(ptr+start_x+(start_y-1)*Image_x) == 0)
//                {
//                    start_y = start_y - 1;
//                    Border_Image[start_y][start_x] = 0;
//                    if (L_findout == 0)
//                    {
//                        L_findout = 1;
//                        Total_Image.special_point[L].x = start_x;
//                        Total_Image.special_point[L].y = start_y+1;
//                    }
//                }
//
//                Enter_Queue(&J_L_Queue, start_x, start_y);
//                Pop_Queue(&J_L_Queue, &last_J_L_border_point);
//
//                if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
//                {
//                    Total_Image.error = L_point_to_border;
//                    break;
//                }
//            }
//        }
//    }

    //�󻷵�ʱ��ȡ���ϽǵĹյ�
    if (Total_Image.road_type == left_round_about_status4 || Total_Image.road_type == left_round_about_status5 || Total_Image.road_type == left_round_about_status11 || Total_Image.road_type == left_round_about_status12)
    {
        start_x = Total_Image.special_point[C].x+5;
        start_y = Total_Image.special_point[C].y;;
        for (Uint8 i = 0; i < 60; i++)
        {
            if (*(ptr+start_x+(start_y-i)*Image_x) - *(ptr+start_x+(start_y-i-1)*Image_x) == 255)
            {
                start_y = start_y-i-1;
                break;
            }
        }

        Stack_Init(&J_L_Stack);
        //�ҵ��յ�
        for (Uint8 i = 0; i < 30; i++)
        {
            if (*((start_y+1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y + 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*(start_y*Image_x+ptr+start_x+1) == 0)
            {
                start_x = start_x + 1;
                Border_Image[start_y][start_x] = 0;
            }
            else if (*((start_y-1)*Image_x+ptr+start_x) == 0)
            {
                start_y = start_y - 1;
                Border_Image[start_y][start_x] = 0;
                if (J_findout == 0) Enter_Stack(&J_L_Stack, start_x, start_y+1);
            }

            //�ȵ�ջ��
            if (Stack_Full(&J_L_Stack) == 1 && J_findout == 0)
            {
                Pop_Stack(&J_L_Stack, &last_J_L_border_point);
                for (Uint8 j = Stack_Size-1; j > 0; j--)
                {
                    Pop_Stack(&J_L_Stack, &now_J_L_border_point);
                    //�ҵ���һ��ջ�������ڵ�����X��ƫ��ϴ���Ǹ���Ϊʮ���Ϲյ�
                    if (ABS(now_J_L_border_point.x - last_J_L_border_point.x) > 3)
                    {
                        Total_Image.special_point[J].x = last_J_L_border_point.x;
                        Total_Image.special_point[J].y = last_J_L_border_point.y;
                        break;
                    }
                    else
                    {
                        last_J_L_border_point.x = now_J_L_border_point.x;
                        last_J_L_border_point.y = now_J_L_border_point.y;
                    }
                    //����ƫ���С�Ļ�,ȡջ�׵�������Ϊʮ���Ϲյ�
                    if (j == 1)
                    {
                        Total_Image.special_point[J].x = last_J_L_border_point.x;
                        Total_Image.special_point[J].y = last_J_L_border_point.y;
                        J_findout = 1;
                    }
                }
            }

            //�ж��Ƿ�Խ��
            if (start_y < 1 || start_y  > 58 || start_x < 1 || start_x > 92)
            {
                break;
            }
        }
    }

    //�������������
    if (Total_Image.road_type == road_normal)
    {
        start_x = (Total_Image.special_point[A].x + Total_Image.special_point[B].x)/2;
        Uint8 temp_point = *(ptr+start_x+31*Image_x);
        Uint8 temp_white_point = 0;
        Uint8 temp_black_point = 0;
        for (Uint8 i = 30; i > 1; i--)
        {
            if (*(ptr+start_x+i*Image_x) == temp_point)
            {
                temp_white_point++;
            }
            else
            {
                temp_black_point++;
            }
        }
        if (temp_white_point > 26)
        {
            Round_about = 1;
        }
    }

    //���������һ����̬�İ���
    if (Total_Image.road_type == left_round_about_status10)
    {
        start_x = 46;
        Uint8 temp_point = *(ptr+start_x+50*Image_x);
        Uint8 temp_white_point = 0;
        Uint8 temp_black_point = 0;
        for (Uint8 i = 30; i > 1; i--)
        {
            if (*(ptr+start_x+i*Image_x) == temp_point)
            {
                temp_white_point++;
            }
            else
            {
                temp_black_point++;
            }
        }
        if (temp_white_point > 26)
        {
            Exit_Round_about = 1;
        }
    }

    //�����հ���ɨ��
    for (Uint8 i = 40; i < 59; i++)
    {
        left_black_point = 0;
        right_black_point = 0;
        for (Uint8 j = 46; j > 5; j--)
        {
            if (*(ptr+j+i*Image_x) == 0) left_black_point++;
        }
        for (Uint8 j = 47; j < 90; j++)
        {
            if (*(ptr+j+i*Image_x) == 0) right_black_point++;
        }
        if (left_black_point < 5) left_white_line++;
        if (right_black_point < 5) right_white_line++;
    }
    if (left_white_line >= right_white_line) white_line = right_white_line;
    else white_line = left_white_line;
    Total_Image.left_white_line = left_white_line;
    Total_Image.right_white_line = right_white_line;
    Total_Image.white_line = white_line;

    //����·���������ж�
    if (Total_Image.three_way_status >= exit_right_three_way)
    {
        if (Total_Image.left_white_line > 0)
        {
            Total_Image.road_type = stop;
        }
    }

    /* ****************************************���ļ��****************************************** */
    if (Total_Image.three_way_status == exit_right_three_way)
    {
        goto skip;
    }
    /* ********************************************************************************************** */

    /* ****************************************ʮ��·��״̬****************************************** */
    //��һ�ν�ʮ��ǰ
    if (Total_Image.road_type == road_normal && G_findout == 1 && H_findout == 1 && Total_Image.crossing_road_status == crossing_road_normal)
    {
        Total_Image.road_type = in_crossing_road2;
    }
    //�ڶ��ν�ʮ��ǰ
    if (Total_Image.road_type == road_normal && G_findout == 1 && H_findout == 1 && Total_Image.crossing_road_status == first_crossing_road_way_out)
    {
        Total_Image.road_type = in_crossing_road1;
    }
    //�����ν�ʮ��ǰ
    if (Total_Image.road_type == road_normal && G_findout == 1 && H_findout == 1 && Total_Image.crossing_road_status == second_crossing_road_way_in)
    {
        Total_Image.road_type = in_crossing_road4;
    }
    //���Ĵν�ʮ��ǰ
    if (Total_Image.road_type == road_normal && G_findout == 1 && H_findout == 1 && Total_Image.crossing_road_status == second_crossing_road_way_out)
    {
        Total_Image.road_type = in_crossing_road3;
    }
    //��һ����ʮ����
    if (Total_Image.road_type == before_crossing_road1 && Total_Image.special_point[C].y > 45 && Total_Image.special_point[D].y > 45 && Total_Image.crossing_road_status == crossing_road_normal)
    {
        Total_Image.road_type = in_crossing_road1;
    }
    //�ڶ�����ʮ����
    if (Total_Image.road_type == before_crossing_road2 && Total_Image.special_point[D].y > 36 && Total_Image.crossing_road_status == first_crossing_road_way_out)
    {
        Total_Image.road_type = in_crossing_road2;
    }
    //��������ʮ����
    if (Total_Image.road_type == before_crossing_road3 && Total_Image.special_point[C].y > 30 && Total_Image.special_point[D].y > 45 && Total_Image.crossing_road_status == second_crossing_road_way_in)
    {
        Total_Image.road_type = in_crossing_road1;
    }
    //���Ĵ���ʮ����
    if (Total_Image.road_type == before_crossing_road4 && Total_Image.special_point[D].y > 36 && Total_Image.crossing_road_status == second_crossing_road_way_out)
    {
        Total_Image.road_type = in_crossing_road3;
    }
    //��ʮ��
    if (Total_Image.road_type == in_crossing_road1 && Total_Image.left_white_line == 0 && Total_Image.right_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.crossing_road_status++;
    }
    if (Total_Image.road_type == in_crossing_road2 && Total_Image.left_white_line == 0 && Total_Image.right_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.crossing_road_status++;
    }
    if (Total_Image.road_type == in_crossing_road4 && Total_Image.left_white_line == 0 && Total_Image.right_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.crossing_road_status++;
    }
    if (Total_Image.road_type == in_crossing_road3 && Total_Image.left_white_line == 0 && Total_Image.right_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.crossing_road_status = crossing_road_normal;
    }
    /* ********************************************************************************************** */

    /* **********************************����·״̬************************************************** */
    //��������ǰ
    if (Total_Image.road_type == road_normal && I_findout == 1 && Total_Image.special_point[I].y > 10 && ABS(Total_Image.special_point[C].y - Total_Image.special_point[D].y) < 12 && (int)eulerAngle.pitch < 5)
    {
        Total_Image.road_type = before_right_three_way_road;
    }
    //����������
    if (Total_Image.road_type == before_right_three_way_road && Total_Image.left_white_line > 10)
    {
        Total_Image.road_type = in_right_three_way_road;
    }
    //��������
    if (Total_Image.road_type == in_right_three_way_road && Total_Image.left_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.three_way_status++;
    }
    if (Total_Image.three_way_status == enter_right_three_way)
    {
        goto skip;
    }

//    //��������ǰ
//    if (Total_Image.road_type == road_normal && I_findout == 1 && Total_Image.special_point[I].y > 10 && Total_Image.three_way_status >= exit_right_three_way && ABS(Total_Image.special_point[C].y - Total_Image.special_point[D].y) < 8)
//    {
//        Total_Image.road_type = before_left_three_way_road;
//    }
//    //����������
//    if (Total_Image.road_type == before_left_three_way_road && Total_Image.right_white_line > 10)
//    {
//        Total_Image.road_type = in_left_three_way_road;
//    }
//    //��������
//    if (Total_Image.road_type == in_left_three_way_road && Total_Image.right_white_line == 0)
//    {
//        Total_Image.road_type = road_normal;
//        Total_Image.three_way_status++;
//        Total_Image.three_way_status = Total_Image.three_way_status % 4;
//    }
    /* ********************************************************************************************** */

    /* *****************************************�һ���״̬*********************************************** */

//    if (Total_Image.road_type == road_normal && C_findout == 0 && D_findout == 1 && Total_Image.special_point[D].y > 30)
//    {
//        Total_Image.road_type = right_round_about_status1;
//    }
//    if (Total_Image.road_type == right_round_about_status1 && C_findout == 0 && D_findout == 1 && Total_Image.special_point[D].y > 50)
//    {
//        Total_Image.road_type = right_round_about_status1_5;
//    }
//    if (Total_Image.road_type == right_round_about_status1_5 && C_findout == 0 && Total_Image.right_white_line > 15)
//    {
//        Total_Image.road_type = right_round_about_status2;
//    }
//    if (Total_Image.road_type == right_round_about_status2 && C_findout == 0 && Total_Image.right_white_line == 0)
//    {
//        Total_Image.road_type = right_round_about_status3;
//    }
//    if (Total_Image.road_type == right_round_about_status3 && D_findout == 1 && Total_Image.special_point[B].x <= Total_Image.special_point[D].x && (Total_Image.special_point[B].y - Total_Image.special_point[D].y) < 20)
//    {
//        Total_Image.road_type = right_round_about_status4;
//    }
//    if (Total_Image.road_type == right_round_about_status4 && L_findout == 1 && Total_Image.special_point[L].y > 15)
//    {
//        Total_Image.road_type = right_round_about_status5;
//    }
//    if (Total_Image.road_type == right_round_about_status5 && Total_Image.left_white_line > 10)
//    {
//        Total_Image.road_type = right_round_about_status6;
//    }
//    if (Total_Image.road_type == right_round_about_status6 && Total_Image.left_white_line == 0)
//    {
//        Total_Image.road_type = right_round_about_status7;
//    }
//    if (Total_Image.road_type == right_round_about_status7 && C_findout == 1 && D_findout == 1 && Total_Image.special_point[C].y > 25)
//    {
//        Total_Image.road_type = right_round_about_status8;
//    }
//    if (Total_Image.road_type == right_round_about_status8 && C_findout == 1 && D_findout == 1 && Total_Image.special_point[C].y > 45)
//    {
//        Total_Image.road_type = right_round_about_status8_5;
//    }
//    if (Total_Image.road_type == right_round_about_status8_5 && Total_Image.left_white_line > 10)
//    {
//        Total_Image.road_type = right_round_about_status9;
//    }
//    if (Total_Image.road_type == right_round_about_status9 && Total_Image.left_white_line == 0)
//    {
//        Total_Image.road_type = right_round_about_status10;
//    }
//    if (Total_Image.road_type == right_round_about_status10 && Total_Image.special_point[B].x <= Total_Image.special_point[D].x && (Total_Image.special_point[B].y - Total_Image.special_point[D].y) < 20)
//    {
//        Total_Image.road_type = right_round_about_status11;
//    }
//    if (Total_Image.road_type == right_round_about_status11 && Total_Image.right_white_line > 10)
//    {
//        Total_Image.road_type = right_round_about_status12;
//    }
//    if (Total_Image.road_type == right_round_about_status12 && Total_Image.right_white_line == 0)
//    {
//        Total_Image.road_type = road_normal;
//    }

    /* ************************************************************************************************* */

    /* *****************************************�󻷵�״̬*********************************************** */

    if (Total_Image.road_type == road_normal && C_findout == 1 && D_findout == 0 && Total_Image.special_point[C].y > 30 && Round_about == 1 && Total_Image.round_road_status == round_road_normal)
    {
        Total_Image.road_type = left_round_about_status1;
    }
    if (Total_Image.road_type == left_round_about_status1 && C_findout == 1 && D_findout == 0 && Total_Image.special_point[C].y > 50)
    {
        Total_Image.road_type = left_round_about_status1_5;
    }
    if (Total_Image.road_type == left_round_about_status1_5 && D_findout == 0 && Total_Image.left_white_line > 15)
    {
        Total_Image.road_type = left_round_about_status2;
    }
    if (Total_Image.road_type == left_round_about_status2 && D_findout == 0 && Total_Image.left_white_line == 0)
    {
        Total_Image.road_type = left_round_about_status3;
    }
    if (Total_Image.road_type == left_round_about_status3 && Total_Image.left_white_line > 0)
    {
        Total_Image.road_type = left_round_about_status4;
    }
    if (Total_Image.road_type == left_round_about_status4)
    {
        Total_Image.road_type = left_round_about_status5;
    }
    if (Total_Image.road_type == left_round_about_status5 && Total_Image.right_white_line > 2)
    {
        Total_Image.road_type = left_round_about_status6;
    }
    if (Total_Image.road_type == left_round_about_status6 && Total_Image.right_white_line == 0)
    {
        Total_Image.road_type = left_round_about_status7;
    }
    if (Total_Image.road_type == left_round_about_status7 && D_findout == 1 && Total_Image.special_point[D].y > 25)
    {
        Total_Image.road_type = left_round_about_status8;
    }
    if (Total_Image.road_type == left_round_about_status8 && D_findout == 1 && Total_Image.special_point[D].y > 50)
    {
        Total_Image.road_type = left_round_about_status8_5;
    }
    if (Total_Image.road_type == left_round_about_status8_5 && Total_Image.right_white_line > 10)
    {
        Total_Image.road_type = left_round_about_status9;
    }
    if (Total_Image.road_type == left_round_about_status9 && Total_Image.right_white_line == 0)//����һ������0����ֱ�ӵ�40
    {
        Total_Image.road_type = left_round_about_status10;
    }
    if (Total_Image.road_type == left_round_about_status10 && (int)eulerAngle.yaw > 344)//���������̬����
    {
        Total_Image.road_type = left_round_about_status11;
    }
    if (Total_Image.road_type == left_round_about_status11 && Total_Image.left_white_line == 0)
    {
        Total_Image.road_type = road_normal;
        Total_Image.round_road_status++;
    }
//    if (Total_Image.road_type == left_round_about_status12 && Total_Image.left_white_line == 0)
//    {
//        Total_Image.road_type = road_normal;
//    }
skip:;
    /* ************************************************************************************************* */

}






