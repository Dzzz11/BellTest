#include "stm32f1xx_hal.h"
#include "Buzzer.h"
#include "cmsis_os.h"
void Buzzer_SetFreq(uint32_t freq,TIM_HandleTypeDef htim,uint32_t Channel ) 
{
  uint32_t timer_base_freq = 1000000;  // 1MHz (预分频器已固定为71)
  uint32_t arr = (timer_base_freq / freq) - 1;
	 __HAL_TIM_SET_AUTORELOAD(&htim, arr);          // 设置新ARR值
  __HAL_TIM_SET_COMPARE(&htim, Channel, arr / 2); // 保持50%占空比
  
} 


void Buzzer_PlayMelody(const NoteSegment *melody,TIM_HandleTypeDef htim,uint32_t Channel) {
  for(uint32_t i=0; ;i++) {
    MusicalNote note = melody[i].note;
    uint32_t duration = melody[i].duration;
    
    // 检测结束标志
    if(note == NOTE_REST && duration == 0) break;
    
    // 设置频率（休止符静音）
    if(note != NOTE_REST) {
      Buzzer_SetFreq((uint32_t)note,htim,Channel );
      HAL_TIM_PWM_Start(&htim, Channel);
    } else {
      HAL_TIM_PWM_Stop(&htim, Channel);
    }
    
    // 保持音符时值
    osDelay(pdMS_TO_TICKS(duration));
    
    // 短暂间隙（防止连音）
    HAL_TIM_PWM_Stop(&htim, Channel);
    osDelay(pdMS_TO_TICKS(20));
  }
  
  // 播放完成关闭输出
  HAL_TIM_PWM_Stop(&htim, Channel);
}
