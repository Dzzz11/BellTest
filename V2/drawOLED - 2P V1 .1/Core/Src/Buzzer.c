#include "stm32f1xx_hal.h"
#include "Buzzer.h"
#include "cmsis_os.h"
void Buzzer_SetFreq(uint32_t freq,TIM_HandleTypeDef htim,uint32_t Channel ) 
{
  uint32_t timer_base_freq = 1000000;  // 1MHz (Ԥ��Ƶ���ѹ̶�Ϊ71)
  uint32_t arr = (timer_base_freq / freq) - 1;
	 __HAL_TIM_SET_AUTORELOAD(&htim, arr);          // ������ARRֵ
  __HAL_TIM_SET_COMPARE(&htim, Channel, arr / 2); // ����50%ռ�ձ�
  
} 


void Buzzer_PlayMelody(const NoteSegment *melody,TIM_HandleTypeDef htim,uint32_t Channel) {
  for(uint32_t i=0; ;i++) {
    MusicalNote note = melody[i].note;
    uint32_t duration = melody[i].duration;
    
    // ��������־
    if(note == NOTE_REST && duration == 0) break;
    
    // ����Ƶ�ʣ���ֹ��������
    if(note != NOTE_REST) {
      Buzzer_SetFreq((uint32_t)note,htim,Channel );
      HAL_TIM_PWM_Start(&htim, Channel);
    } else {
      HAL_TIM_PWM_Stop(&htim, Channel);
    }
    
    // ��������ʱֵ
    osDelay(pdMS_TO_TICKS(duration));
    
    // ���ݼ�϶����ֹ������
    HAL_TIM_PWM_Stop(&htim, Channel);
    osDelay(pdMS_TO_TICKS(20));
  }
  
  // ������ɹر����
  HAL_TIM_PWM_Stop(&htim, Channel);
}
