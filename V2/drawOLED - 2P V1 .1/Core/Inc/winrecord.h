#ifndef __WINRECORD_H
#define __WINRECORD_H

#pragma pack(push, 1)  // ǿ��1�ֽڶ��루����ṹ����䣩
typedef struct {
    uint16_t player1Wins; // ���1ʤ��������2�ֽڣ�
    uint16_t player2Wins; // ���2ʤ��������2�ֽڣ�
} WinRecord;  // �ܴ�С=4�ֽ�
#pragma pack(pop)
#define FLASH_SAVE_ADDR   0x0800FC00 // Page 63����ʼ��ַ�����2KB��






#endif


