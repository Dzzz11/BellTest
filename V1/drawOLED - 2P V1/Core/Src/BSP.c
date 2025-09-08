#include "stm32f1xx_hal.h"
#include "BSP.h"
#include "Information.h"

uint8_t JudgePlace(uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount)
{
		const uint8_t peopleFeetY = peopleY + 9; // 人物底部Y坐标
    const uint8_t peopleLeft = peopleX + 1;
    const uint8_t peopleRight = peopleX + 8;
		if(State == 4)
		{
				if (peopleFeetY <= moveplatform.y  && peopleFeetY >= moveplatform.y - 1 ){
            if ((peopleRight > moveplatform.x) && (peopleLeft < moveplatform.x + moveplatform.width)) {
                return 1;
            }
					}
		}
    for (uint8_t i = 0; i < platformCount; i++) {
        Information p = platforms[i];
			if (peopleFeetY <= p.y  && peopleFeetY >= p.y - 1 ){
            if ((peopleRight > p.x) && (peopleLeft < p.x + p.width)) {
                return 1;
            }
        }

    }
    return 0;
}

uint8_t JudgeThorns(uint8_t peopleX, uint8_t peopleY)
{
		const uint8_t peopleFeetY = peopleY + 9; // 人物底部Y坐标
    const uint8_t peopleLeft = peopleX;
    const uint8_t peopleRight = peopleX + 16;
		if(State == 0)
		{
			return 0;
//			for (uint8_t i = 0; i < sizeof(thorns5)/sizeof(thorns5[0]); i++) {
//        Information p = thorns5[i];
//			if (peopleFeetY < p.y - 3 && peopleY > 42){
//            if ((peopleRight > p.x) && (peopleLeft < p.x )) {
//                return 1;
//            }
//        }
//     }
		}
		else if(State == 1)
		{
			return 0;
		}
		else if(State == 2)
		{
			for (uint8_t i = 0; i < thornCount; i++) {
        Information p = thorns4[i];
			if (peopleFeetY < p.y - 3 && peopleY > 25){
            if ((peopleRight > p.x) && (peopleLeft < p.x )) {
                return 1;
            }
        }
     }
		}
		else if(State == 3)
		{
			for (uint8_t i = 0; i < thornCount; i++) {
        Information p = thorns1[i];
			if (peopleFeetY < p.y - 3  && peopleY > 42){
            if ((peopleRight > p.x) && (peopleLeft < p.x )) {
                return 1;
            }
        }
     }
		}
		else if(State == 4)
		{
			for (uint8_t i = 0; i < sizeof(thorns5)/sizeof(thorns5[0]); i++) {
        Information p = thorns5[i];
			if (peopleFeetY < p.y - 3  && peopleY > 33){
            if ((peopleRight > p.x) && (peopleLeft < p.x )) {
                return 1;
            }
        }
     }
		}
    return 0;
	
}
uint8_t Judgethrough (uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount)
{
	const uint8_t peopleFeetY = peopleY + 6; 
	const uint8_t peoplehead = peopleY + 1 ;
	 const uint8_t peopleRight = peopleX + 16;
	 for (uint8_t i = 0; i < platformCount; i++) {
        Information p = platforms1[i];
				if(p.y + 8 > peoplehead && peopleFeetY > p.y + 8)
				{
					if(peopleRight > p.x)
					{
						return 1;
					}
				}
	 }
	 return 0;
}
