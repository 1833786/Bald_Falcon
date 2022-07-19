#include "Draw_Line.h"

/*
 * @brief LCD��Ļ�ϻ�ʮ�ֽ���
 * @param ��ĺ�����
 * @param ���������
 * @param �����ɫ
 * @noticeһ��Ҫ��ͼ��ķ�Χ����ס,��Ȼ�ͻᵼ����Ļ����
 * */
void LCD_Draw_Cross_Point(Uint8 point_x, Uint8 point_y, Uint16 color)
{
    if (point_x > 2 && point_x < 91 && point_y > 2 && point_y < 57)
    {
        for (Uint8 i = point_x-2; i <= point_x+2; i++)
        {
            lcd_drawpoint(i,point_y,color);
        }
        for (Uint8 i = point_y-2; i <= point_y+2; i++)
        {
            lcd_drawpoint(point_x,i,color);
        }
    }
}

/*
 * @brief �����ͼ��ֱ��
 * @param ��һ����ĺ�����
 * @param ��һ�����������
 * @param �ڶ�����ĺ�����
 * @param �ڶ������������
 * @param ͼ��Ŀ��
 * @notice ��һ����͵ڶ����㶼����б��
 * */
void Draw_Straight_Line(Uint8 *imag_out, Uint8 first_point_x, Uint8 first_point_y, Uint8 second_point_x, Uint8 second_point_y, Uint8 Imag_x)
{
    Uint8 *ptd = imag_out;
    Int16 dx = second_point_x - first_point_x;
    Int16 dy = second_point_y - first_point_y;
    float K = (float)dx/dy;
    float b = first_point_x-K*first_point_y;
    Uint8 Y, supplementary_point, deviation;

    supplementary_point = first_point_x;
    *(ptd+first_point_x+Imag_x*first_point_y) = 0;
    for (Uint8 i = 1; i <= (-dy); i++)//����Y������ȥ��x
    {
        Y = (Uint8)((first_point_y-i)*K+b);

        //Ϊ��ʹ��ѭ�߽��ʱ���������������
        if (Y > supplementary_point)
        {
            deviation = Y - supplementary_point;
            for (Uint8 j = 0; j < deviation; j++)
            {
                *(ptd+(Y-j-1)+Imag_x*(first_point_y-i)) = 0;
            }
        }
        else
        {
            deviation = supplementary_point - Y;
            for (Uint8 j = 0; j < deviation; j++)
            {
                *(ptd+(Y+j+1)+Imag_x*(first_point_y-i)) = 0;
            }
        }

        *(ptd+Y+Imag_x*(first_point_y-i)) = 0;
        supplementary_point = Y;
    }
}

/*
 * @brief �����ͼ������
 * @param ���ͼ���ַ
 * @param ��һ����ĺ�����
 * @param ��һ�����������
 * @param �ڶ�����ĺ�����
 * @param �ڶ������������
 * @param ͼ��Ŀ��
 * @notice ��һ����Ҳ����б��
 * */
Uint8 Draw_Ray_Line(Uint8 *imag_out, Uint8 first_point_x, Uint8 first_point_y, Uint8 second_point_x, Uint8 second_point_y, Uint8 Imag_x)
{
    Uint8 *ptd = imag_out;
    Uint8 temp_x, temp_y;
    Uint8 deviation;
    ARRAY start_point;
    float slope, intercept;

    start_point.x = first_point_x;
    start_point.y = first_point_y;
    *(ptd+first_point_x+first_point_y*Imag_x) = 0;
    Calculate_Boder_Line_Slope(first_point_x, first_point_y, second_point_x, second_point_y, start_point, &slope, &intercept);

    do{
        Fixed_Middle_Line_With_Slope(slope, intercept, start_point, &temp_x, &temp_y);
        if (temp_x < start_point.x)
        {
            deviation = start_point.x - temp_x;
            for (Uint8 i = 0; i < deviation; i++)
            {
                *(ptd+temp_x+i+1+temp_y*Imag_x) = 0;
            }
        }
        else
        {
            deviation = temp_x - start_point.x;
            for (Uint8 i = 0; i < deviation; i++)
            {
                *(ptd+temp_x-i-1+temp_y*Imag_x) = 0;
            }
        }
        *(ptd+temp_x+temp_y*Imag_x) = 0;
        start_point.x = temp_x;
        start_point.y = temp_y;
    }while(temp_y > 2);

    return temp_x;
}

/*
 * @brief ��������
 * @param ���ͼ���ַ
 * @param ��һ����ĺ�����
 * @param ��һ�����������
 * @param �ڶ�����ĺ�����
 * @param �ڶ������������
 * @param ����ͼ��ĺ�����
 * */
void Draw_Curve(Uint8 *imag_out, Uint8 first_point_x, Uint8 first_point_y, Uint8 second_point_x, Uint8 second_point_y, Uint8 Imag_x)
{
    float center_x, center_y;
    Uint8 dy = first_point_y - second_point_y;
    Uint8 temp_x, supplementary_point, deviation;
    Uint8 distance;

    //��������ľ���
    if (first_point_x < second_point_x)
    {
        distance = My_Sqrt((first_point_y - second_point_y)*(first_point_y - second_point_y) + (second_point_x - first_point_x)*(second_point_x - first_point_x));
    }
    else
    {
        distance = My_Sqrt((first_point_y - second_point_y)*(first_point_y - second_point_y) + (first_point_x - second_point_x)*(first_point_x - second_point_x));
    }


    //���Բ������
    float c1 = (float)(second_point_x*second_point_x - first_point_x*first_point_x + second_point_y*second_point_y - first_point_y*first_point_y) / (2 *(second_point_x - first_point_x));
    float c2 = (float)(second_point_y - first_point_y) / (second_point_x - first_point_x);  //б��
    float AA = (c2*c2 + 1);
    float BB = (2 * first_point_x*c2 - 2 * c1*c2 - 2 * first_point_y);
    float CC = first_point_x*first_point_x - 2 * first_point_x*c1 + c1*c1 + first_point_y*first_point_y - distance*distance;

    center_y = (-BB + My_Sqrt(BB*BB - 4 * AA*CC)) /(float)(2 * AA);
    center_x = c1 - c2 * center_y;

    //����Բ�ı�׼����,���ÿһ��x
    *(imag_out+first_point_x+first_point_y*Imag_x) = 0;
    supplementary_point = first_point_x;
    for (Uint8 i = 1; i <= dy; i++)
    {
        temp_x = My_Sqrt(distance*distance - (first_point_y-i-center_y)*(first_point_y-i-center_y))+center_x;
        if (temp_x < supplementary_point)
        {
            deviation = supplementary_point - temp_x;
            for (Uint8 j = 0; j <= deviation; j++)
            {
                *(imag_out+temp_x+j+(first_point_y-i)*Imag_x) = 0;
            }
        }
        else
        {
            deviation = temp_x - supplementary_point;
            for (Uint8 j = 0; j <= deviation; j++)
            {
                *(imag_out+temp_x-j+(first_point_y-i)*Imag_x) = 0;
            }
        }
        supplementary_point = temp_x;
    }
}

/*
 * @brief ����һ��ֱ�ߵ�б��
 * @param ��һ����ĺ�����
 * @param ��һ�����������
 * @param �ڶ�����ĺ�����
 * @param �ڶ������������
 * @param �����ֱ��б��
 * @param �����ֱ�߽ؾ�
 * */
void Calculate_Boder_Line_Slope(Uint8 first_point_x, Uint8 first_point_y, Uint8 second_point_x, Uint8 second_point_y, ARRAY start_point, float *slope, float *intercept)
{
    Int16 dx = second_point_x - first_point_x;
    Int16 dy = second_point_y - first_point_y;
    *slope = (float)dx/(float)dy;
    *intercept = start_point.x - (*slope)*start_point.y;
}

/*
 * @brief ����ֱ��б�������һ���������
 * @param ��ֱ֪�ߵ�б��
 * @param ��ֱ֪�ߵĽؾ�
 * @param ��֪�������
 * @param Ҫ��ĵ�ĺ�����
 * @param Ҫ��ĵ��������
 * */
void Fixed_Middle_Line_With_Slope(float slope, float intercept, ARRAY start_point, Uint8 *output_x, Uint8 *output_y)
{
    *output_y = (start_point.y-1);
    *output_x = (Uint8)(slope*(*output_y)+intercept);
}

/*
 * @brief ��������
 * @param �ߵ���ɫ
 * @noticeһ��Ҫ��ͼ��ķ�Χ����ס,��Ȼ�ͻᵼ����Ļ����
 * */
void LCD_Draw_Middle_Line(Uint8 stack_length, Uint16 color)
{
    ARRAY temp_store;

    for (Uint8 i = 0; i < stack_length; i++)
    {
        temp_store.x = Total_Image.middle_border_line[i];
        temp_store.y = 58-i;
        if (temp_store.x > 2 && temp_store.x < 91 && temp_store.y > 2 && temp_store.y < 59)
        {
            lcd_drawpoint(temp_store.x, temp_store.y, color);
        }
    }
}

/*
 * @brief ����ǰհ������
 * @param ǰհ�ߵ�������
 * @param �ߵ���ɫ
 * */
void LCD_Draw_Propect_Line(Uint8 propect_y, Uint16 color)
{
    for (Uint8 i = 20; i < 64; i++)
    {
        lcd_drawpoint(i, propect_y, color);
    }
}

/*
 * @brief ����ֱ�ߵ�б��
 * @param ��һ���������
 * @param �ڶ����������
 * @return ��ֱ�ߵ�б��
 * */
float Calculate_Slope(ARRAY first_point, ARRAY second_point)
{
    Int16 dx = second_point.x - first_point.x;
    Int16 dy = second_point.y - first_point.y;
    float K = (float)dx / (float)dy;

    return K;
}

/*
 * @brief ���ݱ���б�����������
 * @param ������ߵ�����
 * @param ��һ���������
 * @param �ڶ����������
 * @param ������ߵ���ʼ������
 * */
void Fitting_Middle_Line_With_Border(Uint8* middle_line, Uint8 first_x, Uint8 second_x, Uint8 first_y, Uint8 second_y, Uint8 start_x, Uint8 start_y, Uint8 *last_x, Uint8 *last_y)
{
    Int16 dx = second_x - first_x;
    Int16 dy = second_y - first_y;
    float K = (float)dx / (float)dy;
    float b = start_x-K*start_y;
    Uint8 Y;

    *middle_line = start_x;
    for (Uint8 i = 1; i <= (-dy); i++)
    {
        Y = (Uint8)((start_y-i)*K+b);
        *(middle_line+i) = Y;
    }
    *last_x = Y;
    *last_y = start_y+dy;
}


