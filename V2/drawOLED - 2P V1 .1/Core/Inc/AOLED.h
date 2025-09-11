#ifndef __AOLED_H
#define __AOLED_H

#include <stdint.h>
#include "OLED_Data.h"

/*�����궨��*********************/

/*FontSize����ȡֵ*/
/*�˲���ֵ���������жϣ��������ڼ�������ַ�ƫ�ƣ�Ĭ��ֵΪ�������ؿ��*/
#define AOLED_8X16				8
#define AOLED_6X8				6

/*IsFilled������ֵ*/
#define AOLED_UNFILLED			0
#define AOLED_FILLED				1

/*********************�����궨��*/


/*��������*********************/
void AOLED_WriteCommand(uint8_t Command);
/*��ʼ������*/
void AOLED_Init(void);

/*���º���*/
void AOLED_Update(void);
void AOLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*�Դ���ƺ���*/
void AOLED_Clear(void);
void AOLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void AOLED_Reverse(void);
void AOLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*��ʾ����*/
void AOLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void AOLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void AOLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void AOLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void AOLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void AOLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void AOLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void AOLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void AOLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);

/*��ͼ����*/
void AOLED_DrawPoint(int16_t X, int16_t Y);
uint8_t AOLED_GetPoint(int16_t X, int16_t Y);
void AOLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void AOLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled);
void AOLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void AOLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void AOLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void AOLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);

/*********************��������*/

#endif


/*****************��Э�Ƽ�|��Ȩ����****************/
/*****************jiangxiekeji.com*****************/
