#include "headfile.h"
#pragma section all "cpu0_dsram"

int core0_main(void)
{
    uint32 Time;
    Uint8 propect_y;

    get_clk();

    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    lcd_init();
    Gpio_Init();
    FOC_Init();
    Servo_Init();
    Motor_Init();
    Buttons_Init();
    Banlance_Algorithm_Init();
    mt9v03x_init();

    Attitude_Algorithm_Init();
    pit_interrupt_ms(CCU6_0, PIT_CH0, 5);//������ʱ���жϽ����˶�����
    pit_interrupt_ms(CCU6_0, PIT_CH1, 5);//������ʱ���жϽ���ƽ�����

    Total_Image.error = error_normal;
    Total_Image.road_type = start;
    Total_Image.three_way_status = three_way_normal;
    Total_Image.crossing_road_status = crossing_road_normal;
    Total_Image.round_road_status = round_road_normal;

    Bicycle_Configuration();//ϵͳ��ʼ��
    while (TRUE)
    {
/******************��ӡ���ݷ��ڴ˴�,������ʱ�ǵùر�*******************/
//        FOC_TEST_PRINTF();
//        BANLANCE_TEST_PRINTF();
//        systick_delay_ms(STM0, 5);
/******************��ӡ���ݷ��ڴ˴�,������ʱ�ǵùر�*******************/
#if 1
        if(mt9v03x_finish_flag && !start_stop_flag)
#else
        if(mt9v03x_finish_flag)
#endif
        {
//            Time = systick_getval_ms(STM1);
            //ͼ������
            Global_Init();
            Image_Cut(mt9v03x_image[0], Cut_Image[0]);
            OTSU_Filter(Cut_Image[0], Binary_Image[0], Cut_Imag_X, Cut_Imag_Y);
            Edge_Extraction(Binary_Image[0], Binary_Imag_X, Binary_Imag_Y);
            Fixed_Border_Line(Total_Image.road_type);
            Fixed_Middle_Line(Total_Image.road_type, Border_Imag_X, Border_Imag_Y);
            propect_y = Output_Error(Total_Image.middle_line_cnt, 20);

//            Time = systick_getval_ms(STM1) - Time;
            //ͼ����ʾ����
//            lcd_displayimage032(Border_Image[0], Border_Imag_X, Border_Imag_Y);
//            user_lcd_displayimage032(Binary_Image[0], Binary_Imag_X, Binary_Imag_Y, 0, 64);

            //Debug����
//            lcd_showuint8(96,0,Total_Image.road_type);
//            lcd_showfloat(96,1,eulerAngle.pitch, 2, 2);
//            lcd_showuint8(96,1,error);
//            lcd_showuint8(96,2,Total_Image.left_white_line);lcd_showuint8(120,2,Total_Image.right_white_line);
//            lcd_showfloat(96,2,Total_Image.left_line_curvature,2,3);
//            lcd_showfloat(96,3,Total_Image.right_line_curvature,2,3);
//            lcd_showuint8(96,3,Total_Image.white_line);

//            lcd_showint16(96,6,Total_Image.Output_error);
//            lcd_showuint8(96,7,(Uint8)Time);

            //�������㺯��
//            LCD_Draw_Cross_Point(Total_Image.special_point[C].x, Total_Image.special_point[C].y, RED);
//            LCD_Draw_Cross_Point(Total_Image.special_point[D].x, Total_Image.special_point[D].y, BLUE);
//            LCD_Draw_Cross_Point(Total_Image.special_point[G].x, Total_Image.special_point[G].y, YELLOW);
//            LCD_Draw_Cross_Point(Total_Image.special_point[H].x, Total_Image.special_point[H].y, GREEN);
//            LCD_Draw_Cross_Point(Total_Image.special_point[L].x, Total_Image.special_point[L].y, PURPLE);
//            LCD_Draw_Middle_Line(RED);
//            LCD_Draw_Propect_Point(BLUE);

            mt9v03x_finish_flag  = 0;
        }
    }
}

#pragma section all restore


