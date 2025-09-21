#ifndef __INFORMATION_H
#define __INFORMATION_H
#include "stm32f1xx_hal.h"
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
} Information;

Information platforms1[] = {
    {0, 55, 16, 16},   
    {24, 55, 16, 16},  
    {44, 42, 16, 16},
		{24, 20, 16, 16},
		{64, 14, 16, 16},
//		{96, 34, 16, 16},
		{96, 45, 16, 16},
};
Information platforms2[] = {
    {0, 55, 16, 16},   
    {30, 55, 16, 16},  
    {45, 42, 16, 16},
		{75, 42, 16, 16},
		{105, 50, 16, 16},
};
Information platforms3[] = {
    {0, 55, 16, 16},   
    {30, 55, 16, 16},  
    {45, 42, 16, 16},
		{20, 20, 16, 16},
		{60, 15, 16, 16},
		{105, 45, 16, 16},
};

Information platforms4[] = {
    {0, 55, 16, 16},   
    {25, 45, 16, 16},  
    //{45, 42, 16, 16},
		{45, 25, 16, 16},
		{80, 38, 16, 16},
		{105, 45, 16, 16},
};
Information platforms5[] = {
    {0, 55, 16, 16},   
    {20, 45, 16, 16},  
    //{45, 42, 16, 16},
		//{x, 25, 16, 16},
		{80, 50, 16, 16},
		{105, 45, 16, 16},
};
Information platforms6[] = {
    {0, 55, 16, 16},   
    {25, 45, 16, 16},  
    {45, 32, 16, 16},
		{80, 50, 16, 16},
		{105, 30, 16, 16},
		{130, 38, 16, 16},
		{158, 30, 16, 16},
		{220, 30, 16, 16},
};
Information thorns1[] = {
	{18, 62, 4, 2},
};
Information thorns4[] = {
	{80, 43, 4, 2},
};
Information thorns5[] = {
	{82, 54, 4, 2},
	{30, 49, 4, 2},
};
const uint8_t platformCount = sizeof(platforms1)/sizeof(platforms1[0]);
const uint8_t thornCount = sizeof(thorns1)/sizeof(thorns1[0]);




#endif
