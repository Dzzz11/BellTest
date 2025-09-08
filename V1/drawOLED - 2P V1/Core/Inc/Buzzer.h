#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f1xx_hal.h"

typedef enum {
  NOTE_C4  = 262,
  NOTE_CS4 = 277,
  NOTE_D4  = 294,
  NOTE_DS4 = 311,
  NOTE_E4  = 330,
  NOTE_F4  = 349,
  NOTE_FS4 = 370,
  NOTE_G4  = 392,
  NOTE_GS4 = 415,
  NOTE_A4  = 440,
  NOTE_AS4 = 466,
  NOTE_B4  = 494,
  
  NOTE_C5  = 523,
  NOTE_A3  = 220,
  NOTE_G3  = 196,
  NOTE_REST = 0  // 休止符
} MusicalNote;

// 时值定义（基于BPM=72，四分音符=60000/72 ≈ 833ms）
#define WHOLE    3200  // 全音符 (4拍)
#define HALF     1600  // 二分音符 (2拍)
#define QUARTER  800   // 四分音符 (1拍)
#define EIGHTH   400   // 八分音符 (1/2拍)
#define SIXTEENTH 200  // 十六分音符 (1/4拍)
 typedef struct {
  MusicalNote note;
  uint32_t duration;
} NoteSegment;

static const NoteSegment SunnyChorus[] = {
  // 第一小节
  {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH},
  {NOTE_D4, EIGHTH}, {NOTE_C4, EIGHTH}, 
  {NOTE_C4, QUARTER}, {NOTE_A3, HALF}, {NOTE_REST, EIGHTH},
  
  // 第二小节
  {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH},
  {NOTE_D4, EIGHTH}, {NOTE_C4, EIGHTH}, 
  {NOTE_D4, EIGHTH}, {NOTE_C4, HALF}, {NOTE_REST, EIGHTH},
  
  // 第三小节
  {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH}, {NOTE_E4, EIGHTH},
  {NOTE_D4, EIGHTH}, {NOTE_C4, EIGHTH}, 
  {NOTE_C4, QUARTER}, {NOTE_A3, HALF}, {NOTE_REST, EIGHTH},
  
  // 第四小节
  {NOTE_G4, EIGHTH}, {NOTE_G4, EIGHTH}, {NOTE_G4, EIGHTH},
  {NOTE_G4, EIGHTH}, {NOTE_A4, EIGHTH},
  {NOTE_E4, HALF}, {NOTE_REST, WHOLE}, 
  
  {NOTE_REST, 0}  // 结束标志
};


void Buzzer_SetFreq(uint32_t freq,TIM_HandleTypeDef htim,uint32_t Channel );
void Buzzer_PlayMelody(const NoteSegment *melody,TIM_HandleTypeDef htim,uint32_t Channel);
#endif
