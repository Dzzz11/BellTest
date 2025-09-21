#ifndef __SHARESTRUCT
#define __SHARESTRUCT

typedef struct {
    uint8_t keyID;           // 按键ID
    uint32_t timestamp;      // 时间戳（用于消抖）
} KeyEvent_t;


#endif
