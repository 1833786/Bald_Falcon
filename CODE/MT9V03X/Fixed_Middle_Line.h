#ifndef FIXED_MIDDLE_LINE_H
#define FIXED_MIDDLE_LINE_H

#include "MY_Common.h"
#include "Draw_Line.h"
#include "MY_Imag_Process.h"

#define Border_Line_Size      100//��ֹջ��,��Ϊ��������ֻ��60��
#define PROPECT               20
//�洢��������ݵ�ջ
typedef struct _border_stack_
{
        ARRAY data[Border_Line_Size];
        Int8  top;
}BORDER_SATCK;

extern BORDER_SATCK middle_line_stack;

/* ************************ջ��غ���************************************ */
void Border_Stack_Init(BORDER_SATCK *S);
void Border_Enter_Stack(BORDER_SATCK *S, Uint8 data_x, Uint8 data_y);
void Border_Pop_Stack(BORDER_SATCK *S, ARRAY *data_out);
Uint8 Border_Stack_length(BORDER_SATCK *S);
Int8 Border_Stack_Get_Pointer(BORDER_SATCK *S);
void Border_Stack_Change_Pointer(BORDER_SATCK *S, Int8 pointer);
/* ********************************************************************** */

/* *************************��ջ����ջ��ȡ******************************** */
void Border_Pop_Stack_From_Bottom(BORDER_SATCK *S, ARRAY *data_out);
/* ********************************************************************** */

void Fixed_Border_Line(Uint8 road_type);
void Fixed_Middle_Line(Uint8 road_type, Uint8 Image_x, Uint8 Image_y);
Uint8 Output_Error(Uint8 stack_length, Uint8 prospect);

#endif
