#ifndef __WINRECORD_H
#define __WINRECORD_H

#pragma pack(push, 1)  // 强制1字节对齐（避免结构体填充）
typedef struct {
    uint16_t player1Wins; // 玩家1胜利次数（2字节）
    uint16_t player2Wins; // 玩家2胜利次数（2字节）
} WinRecord;  // 总大小=4字节
#pragma pack(pop)
#define FLASH_SAVE_ADDR   0x0800FC00 // Page 63的起始地址（最后2KB）






#endif


