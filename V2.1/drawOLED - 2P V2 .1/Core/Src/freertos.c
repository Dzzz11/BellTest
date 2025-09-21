/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AOLED.h"
#include "OLED.h"
#include "Information.h"
#include <stdint.h>
#include <string.h>
#include "winrecord.h"
#include "usart.h"
#include "wifi.h"
#include "adc.h"
#include "tim.h"
#include "ShareStruct.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern void SystemClock_Config(void);
uint8_t LowPowerFlag = 0;
WinRecord record;
extern uint8_t OnePMode;
extern uint8_t P1XMax;
uint8_t OnePState = 0;
int16_t PWM = 0;
uint8_t Buffer[4];
uint8_t ABuffer[4];
uint16_t LightControl = 0;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;
QueueHandle_t AKeyQueue = NULL;
QueueHandle_t KeyQueue = NULL;
int16_t people1X = 0;
int16_t people1Y = 46;
int16_t people2X = 0;
int16_t people2Y = 46;
uint8_t temp = 0; //人物一跳跃高度计数变量
uint8_t Atemp = 0; //人物二跳跃高度计数变量
SemaphoreHandle_t P1Mutex = NULL; //人物一动作与画图互斥锁
SemaphoreHandle_t P2Mutex = NULL; //人物二动作与画图互斥锁
int HP = 3;
int AHP = 3;
extern Information platforms1[];
extern Information platforms2[];
extern Information platforms3[];
extern Information platforms4[];
extern Information platforms5[];
extern Information platforms6[];
extern Information thorns1[];
extern Information thorns4[];
extern Information thorns5[]; 
uint8_t gameover = 0; //双人模式人物一游戏结束标志
uint8_t Agameover = 0; //双人模式人物二游戏结束标志
uint8_t OPgameover = 0; //单人模式角色游戏结束标志
uint32_t lasttick = 0;
uint32_t nowtick = 0;
uint32_t Alasttick = 0;
uint32_t Anowtick = 0;
uint8_t State = 0; //关卡检测
static uint8_t Aflag = 0; //人物二第五关移动平台检测标志
static uint8_t flag = 0; //人物一第五关移动平台检测标志
Information moveplatform = {5,25,16,16};
Information Amoveplatform = {5,25,16,16};
Information OPM_moveplatform = {5,50,16,16};
int winround = 0; //人物一单局胜利关卡数
int Awinround = 0; //人物二单局胜利关卡数
uint8_t Whowin = 0; //判断是谁获胜
uint8_t wakeup_flag = 1; //屏幕唤醒标志
extern uint32_t Nowwakeup_Time; 
extern uint32_t Lastwakeup_Time;
uint8_t RX_Buffer[4] = {0};
uint8_t ARX_Buffer[4] = {0};
extern uint8_t Player1[32];
extern uint8_t Player2[32];
/* USER CODE END Variables */
osThreadId OLED_DrawHandle;
osThreadId KeyTaskHandle;
osThreadId AOLED_DrawHandle;
osThreadId AKeyTaskHandle;
osThreadId JudgeWinHandle;
osThreadId BuzzerPlayHandle;
osThreadId WakeUpHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint8_t JudgePlace(uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount);
uint8_t JudgeThorns(uint8_t peopleX, uint8_t peopleY);
uint8_t Judgethrough (uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount);
void LoadWinRecord(WinRecord *record);
void SaveWinRecord(WinRecord *record);
/* USER CODE END FunctionPrototypes */

void DrawPic(void const * argument);
void Key(void const * argument);
void ADraw_Pic(void const * argument);
void AKey(void const * argument);
void Judgewhowin(void const * argument);
void Playmusic(void const * argument);
void GetwakeupFlag(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void ) //低功耗启动函数
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
	if(Nowwakeup_Time - Lastwakeup_Time > 15000)
	{
		LowPowerFlag = 1;
	}
	if(LowPowerFlag)
	{
		HAL_SuspendTick(); // 停止TIM4
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); //进入低功耗stop模式
    SystemClock_Config(); // 恢复时钟
    HAL_ResumeTick();    // 恢复 Tick 中断
		
		Lastwakeup_Time = Nowwakeup_Time;
	}
	
	
	
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName) //栈溢出检测
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	lasttick = HAL_GetTick();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

	//xSemaphoreGive(P1YBinary);
	P1Mutex = xSemaphoreCreateMutex();
	KeyQueue = xQueueCreate(4,sizeof(KeyEvent_t));
	AKeyQueue = xQueueCreate(4,sizeof(KeyEvent_t));
	P2Mutex = xSemaphoreCreateMutex();

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of OLED_Draw */
  osThreadDef(OLED_Draw, DrawPic, osPriorityBelowNormal, 0, 256);
  OLED_DrawHandle = osThreadCreate(osThread(OLED_Draw), NULL);

  /* definition and creation of KeyTask */
  osThreadDef(KeyTask, Key, osPriorityBelowNormal, 0, 400);
  KeyTaskHandle = osThreadCreate(osThread(KeyTask), NULL);

  /* definition and creation of AOLED_Draw */
  osThreadDef(AOLED_Draw, ADraw_Pic, osPriorityBelowNormal, 0, 256);
  AOLED_DrawHandle = osThreadCreate(osThread(AOLED_Draw), NULL);

  /* definition and creation of AKeyTask */
  osThreadDef(AKeyTask, AKey, osPriorityBelowNormal, 0, 400);
  AKeyTaskHandle = osThreadCreate(osThread(AKeyTask), NULL);

  /* definition and creation of JudgeWin */
  osThreadDef(JudgeWin, Judgewhowin, osPriorityHigh, 0, 256);
  JudgeWinHandle = osThreadCreate(osThread(JudgeWin), NULL);

  /* definition and creation of BuzzerPlay */
  osThreadDef(BuzzerPlay, Playmusic, osPriorityNormal, 0, 128);
  BuzzerPlayHandle = osThreadCreate(osThread(BuzzerPlay), NULL);

  /* definition and creation of WakeUp */
  osThreadDef(WakeUp, GetwakeupFlag, osPriorityHigh, 0, 128);
  WakeUpHandle = osThreadCreate(osThread(WakeUp), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_DrawPic */
/**
  * @brief  Function implementing the OLED_Draw thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_DrawPic */
void DrawPic(void const * argument) //人物一画图任务：判断现在游戏模式->亮度检测->判断关卡->画图
{
  /* USER CODE BEGIN DrawPic */
	
	
	
  /* Infinite loop */
  for(;;)
  {
	
				OLED_Update();
		if(wakeup_flag == 1 && OnePMode == 2)
		{
			 OLED_WriteCommand(0x81);
			LightControl = LightSensor_GetValue()*255/4095;
     OLED_WriteCommand(LightControl);
			
			if (xSemaphoreTake(P1Mutex, 100) == pdTRUE) {
				
				if(State == 0)
				{
					OLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms2)/sizeof(platforms2[0]); i++) {
      Information p = platforms2[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
				OLED_ShowImage(124,34,4,16,Door);
				
				OLED_ShowImage(people1X,people1Y,16,16,Player2);
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
			
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				}
				else if(State == 1)
				{
					OLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms3)/sizeof(platforms3[0]); i++) {
      Information p = platforms3[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
				OLED_ShowImage(124,34,4,16,Door);
				
				OLED_ShowImage(people1X,people1Y,16,16,Player2);
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				}
				else if(State == 2)
				{
					OLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms4)/sizeof(platforms4[0]); i++) {
      Information p = platforms4[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
				OLED_ShowImage(124,34,4,16,Door);
				OLED_ShowImage(80,43,4,2,thorn);
				OLED_ShowImage(people1X,people1Y,16,16,Player2);
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				}
				else if(State == 3)
				{
					OLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms1)/sizeof(platforms1[0]); i++) {
      Information p = platforms1[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
				OLED_ShowImage(124,34,4,16,Door);
				OLED_ShowImage(18,62,4,2,thorn);
				OLED_ShowImage(people1X,people1Y,16,16,Player2);
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				}
				else if(State == 4)
				{
				 OLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms5)/sizeof(platforms5[0]); i++) {
      Information p = platforms5[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, platform);
		
  }
		for (uint8_t i = 0; i < sizeof(thorns5)/sizeof(thorns5[0]); i++) {
      Information p = thorns5[i];
      OLED_ShowImage(p.x, p.y, p.width, p.height, thorn);
		
  }
			OLED_ShowImage(moveplatform.x, 25, 16, 16, platform);
			if(moveplatform.x == 110)
			{
				flag = 1;
			}
			else if(moveplatform.x == 5)
			{
				flag = 0;
			}
			if(flag == 0)
			{
				moveplatform.x += 1;
			}
			else 
			{
				moveplatform.x -= 1;
			}
				OLED_ShowImage(124,34,4,16,Door);
				OLED_ShowImage(people1X,people1Y,16,16,Player2);
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
			
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				}
			}

		}

			else if(wakeup_flag == 1 && OnePMode == 1)
		{
			OLED_WriteCommand(0x81);
			uint16_t i = LightSensor_GetValue()*255/4095;
			OLED_WriteCommand(i);
			if (xSemaphoreTake(P1Mutex, 100) == pdTRUE) {
				
					OLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms6)/sizeof(platforms6[0]); i++) {
      Information p = platforms6[i];
			if(p.x > 128)
			{
				OLED_ShowImage(p.x - 128, p.y, p.width, p.height, platform);
			}
  }
				OLED_ShowImage(OPM_moveplatform.x, 50, 16, 16, platform);
			if(OPM_moveplatform.x == 110)
			{
				flag = 1;
			}
			else if(OPM_moveplatform.x == 5)
			{
				flag = 0;
			}
			if(flag == 0)
			{
				OPM_moveplatform.x += 1;
			}
			else 
			{
				OPM_moveplatform.x -= 1;
			}
				OLED_ShowImage(124,34,4,16,Door);
				if(people1X > 128)
				{
					OLED_ShowImage(people1X - 128,people1Y,16,16,Player2);
				}
				OLED_Printf(100,0,OLED_6X8,"HP:%d",HP);
				OLED_Update();
				xSemaphoreGive(P1Mutex);
				
		}
	}
    osDelay(1);
  }
  /* USER CODE END DrawPic */
}

/* USER CODE BEGIN Header_Key */
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key */
void Key(void const * argument) //人物一动作控制任务->判断当前游戏模式->配合串口中断使用switch-case语句判断人物动作
{
  /* USER CODE BEGIN Key */
 
  KeyEvent_t Keyevent;
	
	
  /* Infinite loop */
  for(;;)
  {
		
		if(OnePMode == 2)
		{
			nowtick = HAL_GetTick();
    if(people1Y > 46)  //落地扣血逻辑
		{
			
			if(nowtick - lasttick >= 2500 )
						{
							
							HP--;
							lasttick = nowtick;
						}
			
		}
		if(HP == 0)
		{
			gameover = 1;
		}
			Nowwakeup_Time = HAL_GetTick();
		if(xQueueReceive(KeyQueue, &Keyevent, pdMS_TO_TICKS(100)) == pdPASS)
	 {
			if(xSemaphoreTake(P1Mutex,pdMS_TO_TICKS(100)) == pdTRUE)
		{
      Lastwakeup_Time = Nowwakeup_Time;

			
				switch(Keyevent.keyID)
			{

				case 5:
					if(State == 0)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms2,sizeof(platforms2)/sizeof(platforms2[0])) == 0)
					 {
						people1X +=5;
					 }
					}
					else if(State == 1)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms3,sizeof(platforms3)/sizeof(platforms3[0])) == 0)
					 {
						people1X +=5;
					 }
					}
					else if(State == 2)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms4,sizeof(platforms4)/sizeof(platforms4[0])) == 0)
					 {
						people1X +=5;
					 }
					}
					else if(State == 3)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms1,sizeof(platforms1)/sizeof(platforms1[0])) == 0)
					 {
						people1X +=5;
					 }
					}
					else if(State == 4)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms5,sizeof(platforms5)/sizeof(platforms5[0])) == 0)
					 {
						people1X +=5;
					 }
					}
					if(JudgeThorns(people1X,people1Y) == 1)//踩到刺扣血
				{
					nowtick = HAL_GetTick();

					if(nowtick - lasttick >=1500 )
						{
							HP--;
							lasttick = nowtick;
						}
				}
				if(HP == 0)
				{
					gameover = 1;
					
				}
					
					break;
				case 4:
					if(people1X > 0)
					{
						people1X -=5;
					}
					;
					
					break;
				case 6:
					if(temp <= 20)
					{
						people1Y -= 5;
						temp += 5;
						break;
					}
					else
					{
						people1Y += 2;
					}
			}
			
			
		}
			xSemaphoreGive(P1Mutex);
		}

		 else  //下落到平台逻辑
		 {
			 
			 if(State == 0)			 
			{
				if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms2 , sizeof(platforms2)/sizeof(platforms2[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			} 
			  else if(State == 1)
			{
					if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms3 , sizeof(platforms3)/sizeof(platforms3[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			}
			else if(State == 2)
			{
					if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms4 , sizeof(platforms4)/sizeof(platforms4[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			}
			else if(State == 3)
			{
					if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms1 , sizeof(platforms1)/sizeof(platforms1[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			}
			else if(State == 4)
			{
					if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms5 , sizeof(platforms5)/sizeof(platforms5[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			}
		  temp = 0;
		 }
		}
		else if(OnePMode == 1)
		{
			nowtick = HAL_GetTick();
    if(people1Y > 46)
		{
			
			if(nowtick - lasttick >= 2500 )
						{
							
							HP--;
							lasttick = nowtick;
						}
			
		}
		if(HP == 0)
		{
			OPgameover = 1;
		}
			Nowwakeup_Time = HAL_GetTick();
		if(xQueueReceive(KeyQueue, &Keyevent, pdMS_TO_TICKS(100)) == pdPASS)
	 {
			if(xSemaphoreTake(P1Mutex,pdMS_TO_TICKS(100)) == pdTRUE)
		{
      Lastwakeup_Time = Nowwakeup_Time;

			
				switch(Keyevent.keyID)
			{

				case 5:
					if(OnePState == 0)
					{
						if(people1X <= P1XMax && Judgethrough(people1X,people1Y,platforms6,sizeof(platforms6)/sizeof(platforms6[0])) == 0)
					 {
						people1X +=5;
					 }
							
					}
					
					if(JudgeThorns(people1X,people1Y) == 1)
				{
					nowtick = HAL_GetTick();
					if(HP == 3)
					{
						HP--;
					}
					else
					{
						if(nowtick - lasttick >=1500 )
						{
							HP--;
							lasttick = nowtick;
						}
						
					}
				}
				if(HP == 0)
				{
					gameover = 1;
					
				}
					
					break;
				case 4:
					if(people1X > 0)
					{
						people1X -=5;
					}
					break;
				case 6:
					if(temp <= 20)
					{
						people1Y -= 5;
						temp += 5;
						break;
					}
					else
					{
						people1Y += 2;
					}
			}
			
			
		}
			xSemaphoreGive(P1Mutex);
		}

	 

		 else
		 {
			 
			 if(OnePState == 0)			 
			{
				if(people1Y < 48 && (JudgePlace(people1X,people1Y, platforms6 , sizeof(platforms6)/sizeof(platforms6[0])) == 0))
		    {
			
			  people1Y += 2;
		    }
			} 
			
		  temp = 0;
		 }
		}
    osDelay(1);
  }
  /* USER CODE END Key */
}

/* USER CODE BEGIN Header_ADraw_Pic */
/**
* @brief Function implementing the AOLED_Draw thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ADraw_Pic */
void ADraw_Pic(void const * argument) //人物二画图函数：判断现在游戏模式->亮度检测->判断关卡->画图
{
  /* USER CODE BEGIN ADraw_Pic */
	
	
  /* Infinite loop */
  for(;;)
  {
		
      if(wakeup_flag == 1 && OnePMode == 2)
		{
		AOLED_WriteCommand(0x81);
    LightControl = LightSensor_GetValue()*255/4095;
    AOLED_WriteCommand(LightControl);
		 
		if(xSemaphoreTake(P2Mutex , pdMS_TO_TICKS(100))== pdTRUE)
		{

			if(State == 0)
			{
			AOLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms2)/sizeof(platforms2[0]); i++) {
      Information p = platforms2[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
		

			
		AOLED_ShowImage(124,34,4,16,Door);
		AOLED_ShowImage(people2X,people2Y,16,16,Player1);
		AOLED_Printf(100,0,AOLED_6X8,"HP:%d",AHP);
		AOLED_Update();
		xSemaphoreGive(P2Mutex);
			}
			else if(State == 1)
			{
				
			AOLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms3)/sizeof(platforms3[0]); i++) {
      Information p = platforms3[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
		
		AOLED_ShowImage(124,34,4,16,Door);
		AOLED_ShowImage(people2X,people2Y,16,16,Player1);
		AOLED_Printf(100,0,AOLED_6X8,"HP:%d",AHP);
		AOLED_Update();
		xSemaphoreGive(P2Mutex);
			}
			else if(State == 2)
			{
				
			AOLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms4)/sizeof(platforms4[0]); i++) {
      Information p = platforms4[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
		
		AOLED_ShowImage(124,34,4,16,Door);
		AOLED_ShowImage(80,43,4,2,thorn);
		AOLED_ShowImage(people2X,people2Y,16,16,Player1);
		AOLED_Printf(100,0,AOLED_6X8,"HP:%d",AHP);
		AOLED_Update();
		xSemaphoreGive(P2Mutex);
			}
  		else if(State == 3)
			{
				
			AOLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms1)/sizeof(platforms1[0]); i++) {
      Information p = platforms1[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
		
		AOLED_ShowImage(124,34,4,16,Door);
		AOLED_ShowImage(18,62,4,2,thorn);
		AOLED_ShowImage(people2X,people2Y,16,16,Player1);
		AOLED_Printf(100,0,AOLED_6X8,"HP:%d",AHP);
		AOLED_Update();
		xSemaphoreGive(P2Mutex);
			}
			else if(State == 4)
			{
				
				AOLED_Clear();
		for (uint8_t i = 0; i < sizeof(platforms5)/sizeof(platforms5[0]); i++) {
      Information p = platforms5[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
		
  }
		for (uint8_t i = 0; i < sizeof(thorns5)/sizeof(thorns5[0]); i++) {
      Information p = thorns5[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, thorn);
		
  }
			AOLED_ShowImage(moveplatform.x, 25, 16, 16, platform);
			if(Amoveplatform.x == 110)
			{
				Aflag = 1;
			}
			else if(Amoveplatform.x == 5)
			{
				Aflag = 0;
			}
			if(Aflag == 0)
			{
				Amoveplatform.x += 1;
			}
			else 
			{
				Amoveplatform.x -= 1;
			}
		
		AOLED_ShowImage(124,34,4,16,Door);
		AOLED_ShowImage(people2X,people2Y,16,16,Player1);
		AOLED_Printf(100,0,AOLED_6X8,"HP:%d",AHP);
		AOLED_Update();
		xSemaphoreGive(P2Mutex);
			}
					
	}


	}

			else if(wakeup_flag == 1 && OnePMode == 1)
		{
			AOLED_WriteCommand(0x81);
			uint16_t i = LightSensor_GetValue()*255/4095;
			AOLED_WriteCommand(i);
			if (xSemaphoreTake(P1Mutex, 100) == pdTRUE) {
				
					AOLED_Clear();
				for (uint8_t i = 0; i < sizeof(platforms6)/sizeof(platforms6[0]); i++) {
      Information p = platforms6[i];
      AOLED_ShowImage(p.x, p.y, p.width, p.height, platform);
  }
				
				
				AOLED_ShowImage(people1X,people1Y,16,16,Player2);

				AOLED_Update();
				xSemaphoreGive(P1Mutex);
				
		}
	}
    osDelay(1);
  }

  /* USER CODE END ADraw_Pic */
}

/* USER CODE BEGIN Header_AKey */
/**
* @brief Function implementing the AKeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AKey */
void AKey(void const * argument) //人物二动作控制任务->配合串口中断使用switch-case语句判断人物动作
{
  /* USER CODE BEGIN AKey */
	KeyEvent_t AKeyevent;
	
	
  /* Infinite loop */
  for(;;)
  {
		Anowtick = HAL_GetTick();
		if(people2Y > 46)
		{
			
			if(Anowtick - Alasttick >= 2500 )
						{
							
							AHP--;
							Alasttick = Anowtick;
						}
			
		}
		if(AHP == 0)
		{
			Agameover = 1;
		}
		Nowwakeup_Time = HAL_GetTick();
		if(xQueueReceive(AKeyQueue, &AKeyevent, pdMS_TO_TICKS(100)) == pdPASS)
	 {
			if(xSemaphoreTake(P2Mutex,pdMS_TO_TICKS(100)) == pdTRUE)
		{
       Lastwakeup_Time = Nowwakeup_Time;
		   
		
				switch(AKeyevent.keyID)
			{

				case 2:
				
					if(State == 0)
					{
						if(people2X <= 110 && Judgethrough(people2X,people2Y,platforms2,sizeof(platforms2)/sizeof(platforms2[0])) == 0)
					 {
						people2X +=5;
					 }
					}
					else if(State == 1)
					{
						if(people2X <= 110 && Judgethrough(people2X,people2Y,platforms3,sizeof(platforms3)/sizeof(platforms3[0])) == 0)
					 {
						people2X +=5;
					 }
					}
					else if(State == 2)
					{
						if(people2X <= 110 && Judgethrough(people2X,people2Y,platforms4,sizeof(platforms4)/sizeof(platforms4[0])) == 0)
					 {
						people2X +=5;
					 }
					}
					else if(State == 3)
					{
						if(people2X <= 110 && Judgethrough(people2X,people2Y,platforms1,sizeof(platforms1)/sizeof(platforms1[0])) == 0)
					 {
						people2X +=5;
					 }
					}
					else if(State == 4)
					{
						if(people2X <= 110 && Judgethrough(people2X,people2Y,platforms5,sizeof(platforms5)/sizeof(platforms5[0])) == 0)
					 {
						people2X +=5;
					 }
					}
					if(JudgeThorns(people2X,people2Y) == 1)
				{
					Anowtick = HAL_GetTick();

					if(Anowtick - Alasttick >=1500 )
						{
							AHP--;
							Alasttick = Anowtick;
						}
				}
				if(AHP == 0)
				{
					Agameover = 1;
					
				}
					
					break;
			
				case 3:
					if(people2X > 0)
					{
						people2X -=5;
					}
					
					
					break;
			
			  case 1:
					
					if(Atemp <= 20)
					{
						people2Y -= 5;
						Atemp +=5;
						
						break;
					}
					else
					{
						people2Y +=2;
					}
				
			}
			
		}
			xSemaphoreGive(P2Mutex);
		}
	 else
		 {
			 
			if(State == 0)			 
			{
				if(people2Y < 48 && (JudgePlace(people2X,people2Y, platforms2 , sizeof(platforms2)/sizeof(platforms2[0])) == 0))
		    {
			
			  people2Y += 2;
		    }
			} 
			  else if(State == 1)
			{
					if(people2Y < 48 && (JudgePlace(people2X,people2Y, platforms3 , sizeof(platforms3)/sizeof(platforms3[0])) == 0))
		    {
			
			  people2Y += 2;
		    }
			}
			 else if(State == 2)
			{
					if(people2Y < 48 && (JudgePlace(people2X,people2Y, platforms4 , sizeof(platforms4)/sizeof(platforms4[0])) == 0))
		    {
			
			  people2Y += 2;
		    }
			}
			 else if(State == 3)
			{
					if(people2Y < 48 && (JudgePlace(people2X,people2Y, platforms1 , sizeof(platforms1)/sizeof(platforms1[0])) == 0))
		    {
			
			  people2Y += 2;
		    }
			}
			 else if(State == 4)
			{
					if(people2Y < 48 && (JudgePlace(people2X,people2Y, platforms5 , sizeof(platforms5)/sizeof(platforms5[0])) == 0))
		    {
			
			  people2Y += 2;
		    }
			}
		  Atemp = 0;
		 }

		
    osDelay(1);
  }
  /* USER CODE END AKey */
}

/* USER CODE BEGIN Header_Judgewhowin */
/**
* @brief Function implementing the JudgeWin thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Judgewhowin */
void Judgewhowin(void const * argument) //判断胜利任务 判断当前关卡数->记录数据->擦除图像并画新图->延时后重置人物位置并进入新关卡，若是最后一关则游戏结束并上传数据到flash和物联网
{
  /* USER CODE BEGIN Judgewhowin */
//	uint8_t Whowin = 0;
	uint8_t finalflag = 0;
	uint8_t winflag = 0;

  /* Infinite loop */
  for(;;)
  {
		 if(OnePMode == 1)
		 {
			 if(people1X + 14 > 252 && people1Y > 34 && OnePMode == 1)
	     {
				 OLED_Clear();
				 AOLED_Clear();
				 OLED_Printf(50, 20, OLED_6X8, "WIN!");
				 AOLED_Printf(50, 20, AOLED_6X8, "WIN!");
				 OLED_Update();
				 AOLED_Update();
				 
			 }
			 if(OPgameover && OnePState == 0)
			 {
				  OLED_Clear();
				 AOLED_Clear();
				 OLED_Printf(50, 20, OLED_6X8, "FAIL!");
				 AOLED_Printf(50, 20, AOLED_6X8, "FAIL!");
				 OLED_Update();
				 AOLED_Update();
			 }
		 
		 }
		 if(State == 5)
		 {
			 finalflag = 1;
		 }

    if(finalflag == 0)
		{
			if(people1X + 14 > 124 && people1Y > 34 && OnePMode == 2)
	  {
		  Whowin = 1;
			HP = 3;
			AHP = 3;
			if(Whowin == 1 && winflag == 0)
			{
				winround++;
				winflag = 1;
			}
	  }
		else if(people2X + 14 > 124 && people2Y > 34 && OnePMode == 2)
	  {
		 Whowin = 2;
			HP = 3;
			AHP = 3;
			if(Whowin == 2 && winflag == 0)
			{
				Awinround++;
				winflag = 1;
			}
	  }
		
		if(gameover)
		{
			Whowin = 2;
			HP = 3;
			AHP = 3;
			gameover = 0;
			Awinround ++;
		}
		else if(Agameover)
		{
			Whowin = 1;
			HP = 3;
			AHP = 3;
			Agameover = 0;
			winround ++;
		}

		if(Whowin == 1)
		{
			
			OLED_Clear();
			if(State == 0)
			{
				OLED_Printf(44, 20, OLED_6X8, "R1 P1WIN!");
			 
			  OLED_Update();
			}
			else if(State == 1)
			{
				OLED_Printf(44, 20, OLED_6X8, "R2 P1WIN!");
			  
			  OLED_Update();
			}
			else if(State == 2)
			{
				OLED_Printf(44, 20, OLED_6X8, "R3 P1WIN!");
			  
			  OLED_Update();
			}
			else if(State == 3)
			{
				OLED_Printf(44, 20, OLED_6X8, "R4 P1WIN!");
			 
			  OLED_Update();
			}
			else if(State == 4)
			{
				OLED_Printf(44, 20, OLED_6X8, "R5 P1WIN!");
			  
			  OLED_Update();
			}
			
		
			
			AOLED_Clear();
			if(State == 0)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R1 P1WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 1)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R2 P1WIN!");
			
			  AOLED_Update();
			}
			else if(State == 2)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R3 P1WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 3)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R4 P1WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 4)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R5 P1WIN!");
			 
			  AOLED_Update();
			}
	
		
		}
		else if(Whowin == 2)
		{
			
			OLED_Clear();
			if(State == 0)
			{
				OLED_Printf(44, 20, OLED_6X8, "R1 P2WIN!");
			  
			  OLED_Update();
			}
			else if(State == 1)
			{
				OLED_Printf(44, 20, OLED_6X8, "R2 P2WIN!");
			  
			  OLED_Update();
			}
			else if(State == 2)
			{
				OLED_Printf(44, 20, OLED_6X8, "R3 P2WIN!");
			  
			  OLED_Update();
			}
			else if(State == 3)
			{
				OLED_Printf(44, 20, OLED_6X8, "R4 P2WIN!");
			 
			  OLED_Update();
			}
			else if(State == 4)
			{
				OLED_Printf(44, 20, OLED_6X8, "R5 P2WIN!");
			 
			  OLED_Update();
			}
		

			
			AOLED_Clear();
			if(State == 0)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R1 P2WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 1)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R2 P2WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 2)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R3 P2WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 3)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R4 P2WIN!");
			  
			  AOLED_Update();
			}
			else if(State == 4)
			{
				AOLED_Printf(44, 20, AOLED_6X8, "R5 P2WIN!");
			 
			  AOLED_Update();
			}
		
		 
		}
		}
		if(State == 5)
		{
			AOLED_Clear();
			AOLED_Printf(46, 20, AOLED_6X8, "GameOver");
			AOLED_Printf(46, 40, AOLED_6X8, " %d : %d",winround,Awinround);
			AOLED_Update();
			
		
		
			OLED_Clear();
			OLED_Printf(46, 20, OLED_6X8, "GameOver");
			OLED_Printf(46, 40, OLED_6X8, " %d : %d",winround,Awinround);
			OLED_Update();
			if(winround > Awinround)
			{

				LoadWinRecord(&record);

				State ++;
				record.player1Wins ++;
			
				Wifi_SendData(record.player1Wins , record.player2Wins);
				SaveWinRecord(&record);
			}
			else if(winround < Awinround )
			{

				LoadWinRecord(&record);

				State ++;
				record.player2Wins ++;
			
				Wifi_SendData(record.player1Wins , record.player2Wins);
					SaveWinRecord(&record);
			}
			

			
		
		}
		if(Whowin == 1 || Whowin == 2)
		{

			
			
			  HAL_Delay(1500);
				State ++;
			  Whowin = 0;
				winflag = 0;
				people1X = 0;
				people1Y = 46;
				people2X = 0;
				people2Y = 46;
			
			
			
		}
		
    osDelay(1);
  }
  /* USER CODE END Judgewhowin */
}

/* USER CODE BEGIN Header_Playmusic */
/**
* @brief Function implementing the BuzzerPlay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Playmusic */
void Playmusic(void const * argument) //播放音乐
{
  /* USER CODE BEGIN Playmusic */
	 //uint16_t PWM = 0;
  /* Infinite loop */
  for(;;)
  {
		if(PWM > 100)
		{
			PWM = 100;
		}
		if(PWM <= 0)
		{
			PWM = 0;
		}
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,PWM);
    PWM += Encoder_Get();

		Buzzer_SetFrequency(261);
    osDelay(200);
    Buzzer_SetFrequency(392);
    osDelay(200);
   	Buzzer_SetFrequency(440);
    osDelay(200);
    Buzzer_SetFrequency(261);
    osDelay(200);
		
    osDelay(1);
  }
  /* USER CODE END Playmusic */
}

/* USER CODE BEGIN Header_GetwakeupFlag */
/**
* @brief Function implementing the WakeUp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GetwakeupFlag */
void GetwakeupFlag(void const * argument) //获取唤醒标志任务
{
  /* USER CODE BEGIN GetwakeupFlag */
  /* Infinite loop */
  for(;;)
  {
		if(Nowwakeup_Time - Lastwakeup_Time > 10000)
		{
			wakeup_flag = 0;
			OLED_Clear();
			OLED_Update();
			AOLED_Clear();
			AOLED_Update();
		}
		if(wakeup_flag == 0)
		{
			HAL_UART_Receive_IT(&huart3,Buffer,sizeof(Buffer)); 
			if(Buffer[0] == 0x20 && Buffer[1] == 0x01 && Buffer[2] == 0x01&& Buffer[3] == 0x61)
				{
					
						wakeup_flag = 1;
						Lastwakeup_Time = Nowwakeup_Time;
						memset(Buffer, 0, sizeof(Buffer));
					
				}
			
		}
		if(wakeup_flag == 0)
		{
			HAL_UART_Receive_IT(&huart1,ABuffer,sizeof(ABuffer));
			if(ABuffer[0] == 0x20 && ABuffer[1] == 0x02 && ABuffer[2] == 0x01 && ABuffer[3] == 0x61)
				{
					
						wakeup_flag = 1;
						Lastwakeup_Time = Nowwakeup_Time;
						memset(ABuffer, 0, sizeof(ABuffer));
					
				}
			
		}
		
		
    osDelay(1);
  }
  /* USER CODE END GetwakeupFlag */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
uint8_t JudgePlace(uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount) //判断平台函数
{
		const uint8_t peopleFeetY = peopleY + 9; // 人物底部Y坐标
    const uint8_t peopleLeft = peopleX + 1;
    const uint8_t peopleRight = peopleX + 8;
		if(OnePState == 0)
		{
				if (peopleFeetY <= OPM_moveplatform.y  && peopleFeetY >= OPM_moveplatform.y - 1 ){
            if ((peopleRight - 128 > OPM_moveplatform.x) && (peopleLeft - 128 < OPM_moveplatform.x + OPM_moveplatform.width)) {
                return 1;
            }
					}
		}
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

uint8_t JudgeThorns(uint8_t peopleX, uint8_t peopleY) //判断刺函数
{
		const uint8_t peopleFeetY = peopleY + 9; // 人物底部Y坐标
    const uint8_t peopleLeft = peopleX;
    const uint8_t peopleRight = peopleX + 16;
		if(State == 0)
		{
			return 0;
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
uint8_t Judgethrough (uint8_t peopleX, uint8_t peopleY ,Information *platforms ,uint8_t platformCount) //判断能否向前走函数
{


const uint16_t peopleFeetY = peopleY + 16; 
	const uint16_t peoplehead = peopleY + 1 ;
	 const uint16_t peopleRight = peopleX + 16;
	
	 for (uint8_t i = 0; i < platformCount; i++) {
        Information p = platforms[i];
		 
				if(p.y + 8 > peoplehead && peopleFeetY >= p.y + 10)
				{
					if(peopleRight > p.x && p.x > peopleX)
					{
						return 1;
					}
				}
			
		

	 }
	 return 0;
}


void LoadWinRecord(WinRecord *record) //读flash函数 
{ 
    // 直接读取Flash地址的数据
    WinRecord *flashData = (WinRecord*)FLASH_SAVE_ADDR;
    memcpy(record, flashData, sizeof(WinRecord));
}
void SaveWinRecord(WinRecord *record) //存储flash函数
{
    // 擦除Flash页（必需）
    FLASH_EraseInitTypeDef eraseConfig;
    uint32_t pageError;
    eraseConfig.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseConfig.PageAddress = FLASH_SAVE_ADDR;
    eraseConfig.NbPages = 1;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&eraseConfig, &pageError); // 擦除整个页

    // 按半字（16位）写入数据
    uint16_t *pData = (uint16_t*)record;
    for (uint16_t i = 0; i < sizeof(WinRecord); i += 2) {  // 共写入2次
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 
                         FLASH_SAVE_ADDR + i, 
                         pData[i/2]);
    }

    HAL_FLASH_Lock();
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //外部中断唤醒低功耗模式
{
    if (GPIO_Pin == GPIO_PIN_12)  
    {
        LowPowerFlag = 0;
    }
}


/* USER CODE END Application */

