/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "AOLED.h"
#include "winrecord.h"
#include "OLED_Data.h"
#include "String.h"
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

/* USER CODE BEGIN PV */
//uint8_t RX_Buffer[4] = {0};
uint32_t Nowwakeup_Time = 0;
uint32_t Lastwakeup_Time = 0;
uint8_t Player1[32] = {0};
uint8_t Player2[32] = {0};
uint8_t ChoseDoneFlag = 0;
uint8_t AChoseDoneFlag = 0;
uint8_t Tempbuffer[4] = {0};
uint8_t ATempbuffer[4] = {0};

extern uint8_t Buffer[4];
extern uint8_t ABuffer[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	
	OLED_Init();
	AOLED_Init();
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_UART_IRQHandler(&huart3);
	HAL_UART_IRQHandler(&huart1);
	//HAL_UART_Receive_IT(&huart3,Tempbuffer,sizeof(Tempbuffer));
	//HAL_UART_Receive_IT(&huart2,ATempbuffer,sizeof(ATempbuffer));
//	HAL_UART_Receive_IT(&huart3 , RX_Buffer , sizeof(RX_Buffer));
	WinRecord currentRecord;
  LoadWinRecord(&currentRecord); 
	if (currentRecord.player1Wins == 0xFFFF || currentRecord.player2Wins == 0xFFFF) {
        currentRecord.player1Wins = 0;
				currentRecord.player2Wins = 0;
        SaveWinRecord(&currentRecord); // 首次保存默认值
    }
	while(!ChoseDoneFlag)
	{
		OLED_Printf(10, 20, OLED_6X8, "P1 choose roles");
		AOLED_Printf(10, 20, AOLED_6X8, "P1 choose roles");
		OLED_Update();
		AOLED_Update();
		//HAL_UART_Receive(&huart3 , Tempbuffer, sizeof(Tempbuffer), HAL_MAX_DELAY);
//		HAL_UART_Receive_IT(&huart3,Tempbuffer,sizeof(Tempbuffer));
		HAL_UART_Receive_IT(&huart3,Buffer,sizeof(Buffer));
		if(Buffer[0] == 0x20 && Buffer[3] == 0x61 )
		{
			if(Buffer[1] == 0x01 && Buffer[2] == 0x02)
			{
				memcpy(Player1, people1, 32);
				ChoseDoneFlag = 1;
				HAL_UART_Transmit(&huart3, Buffer, sizeof(Buffer), 100);
			}
			else if(Buffer[1] == 0x01 && Buffer[2] == 0x03)
			{
				memcpy(Player1, people2, 32);
				ChoseDoneFlag = 1;
				HAL_UART_Transmit(&huart3, Buffer, sizeof(Buffer), 100);
			}
			
		}
	}
	while(!AChoseDoneFlag)
	{
		OLED_Printf(10, 20, OLED_6X8, "P2 choose roles");
		AOLED_Printf(10, 20, AOLED_6X8, "P2 choose roles");
		OLED_Update();
		AOLED_Update();
		//HAL_UART_Receive(&huart2 , ATempbuffer, sizeof(ATempbuffer), HAL_MAX_DELAY);
		//HAL_UART_Receive_IT(&huart2,ATempbuffer,sizeof(ATempbuffer));
		HAL_UART_Receive_IT(&huart1,ABuffer,sizeof(ABuffer));
		if(ABuffer[0] == 0x20 && ABuffer[3] == 0x61 )
		{
			if(ABuffer[1] == 0x02 && ABuffer[2] == 0x02)
			{
				memcpy(Player2, people1, 32);
				AChoseDoneFlag = 1;
				HAL_UART_Transmit(&huart1, ABuffer, sizeof(ABuffer), 100);
			}
			else if(ABuffer[1] == 0x02 && ABuffer[2] == 0x03)
			{
				memcpy(Player2, people2, 32);
				AChoseDoneFlag = 1;
				HAL_UART_Transmit(&huart1, ABuffer, sizeof(ABuffer), 100);
			}
			
		}
	}
	OLED_Clear();
	AOLED_Clear();
	OLED_Printf(44, 20, OLED_6X8, "Welcome");
	OLED_Printf(2, 40, OLED_6X8, "Press Button to start");
	OLED_Printf(46, 56, OLED_6X8, " %d : %d",(int)currentRecord.player1Wins,(int)currentRecord.player2Wins);
	OLED_Update();
	AOLED_Printf(44, 20, AOLED_6X8, "Welcome");
	AOLED_Printf(2, 40, AOLED_6X8, "Press Button to start");
	AOLED_Printf(46, 56, AOLED_6X8, " %d : %d",(int)currentRecord.player1Wins,(int)currentRecord.player2Wins);
	AOLED_Update();
	while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == RESET){}
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
