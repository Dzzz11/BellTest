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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "OLED.h"
#include "MPU6050.h"
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
uint8_t Left_flag = 0;
uint8_t Right_flag = 0;
uint8_t Jump_flag = 0;
uint8_t Using_flag = 0;
uint8_t LeftNowState = 0;
uint8_t LeftLastState = 0;
uint8_t RightNowState = 0;
uint8_t RightLastState = 0;
uint8_t JumpNowState = 0;
uint8_t JumpLastState = 0;
uint8_t wakeup_bag[4] = {0x20,0x02,0x01,0x61};//0x20：包头，0x02：二号遥控器�???0x01：一号功能表示唤醒，0x61：包�???
uint8_t ChosePlayer1[4] = {0x20,0x02,0x02,0x61};//0x20：包头，0x02：二号遥控器�???0x02：二号功能表示�?�择角色1�???0x61：包�???
uint8_t ChosePlayer2[4] = {0x20,0x02,0x03,0x61};//0x20：包头，0x02：二号遥控器�??? 0x03：三号功能表示�?�择角色2�??? 0x61：包�???
uint8_t Left[4] = {0x20,0x02,0x04,0x61};//0x20：包头，0x02：二号遥控器�??? 0x04：四号功能表示左移，0x61：包�???
uint8_t Jump[4] = {0x20,0x02,0x05,0x61};//0x20：包头，0x02：二号遥控器�??? 0x05：五号功能表示跳跃，0x61：包�???
uint8_t Right[4] = {0x20,0x02,0x06,0x61};//0x20：包头，0x02：二号遥控器�??? 0x06：六号功能表示又移，0x61：包�???
uint8_t ResetBag[4] = {0x20,0x02,0x00,0x61};
uint8_t ChosePlayer1Flag = 0;
uint8_t ChosePlayer2Flag = 0;
uint8_t AnswerBuffer[4] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  MPU6050_Init();
  MPU6050_t MPU6050Data;
  CalculateBias(&MPU6050Data);
  while (!(ChosePlayer1Flag || ChosePlayer2Flag)) {
    if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == GPIO_PIN_RESET) {
      HAL_Delay(50);
      if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == GPIO_PIN_RESET) {
        ChosePlayer1Flag = 1;
      }
    }
    if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == GPIO_PIN_RESET) {
      HAL_Delay(50);
      if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == GPIO_PIN_RESET) {
        ChosePlayer2Flag = 1;
      }
    }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (ChosePlayer1Flag ) {
      while ( AnswerBuffer[0] != 0x20 && AnswerBuffer[1] != 0x02 && AnswerBuffer[2] != 0x02 && AnswerBuffer[3] != 0x61) {
        HAL_UART_Transmit(&huart1, ChosePlayer1, sizeof(ChosePlayer1), 100);
        HAL_UART_Receive(&huart1, AnswerBuffer, sizeof(AnswerBuffer), HAL_MAX_DELAY);
      }
      memset(AnswerBuffer, 0, sizeof(AnswerBuffer));
      ChosePlayer1Flag = 0;
    }
    if (ChosePlayer2Flag) {
      while ( AnswerBuffer[0] != 0x20 && AnswerBuffer[1] != 0x02 && AnswerBuffer[2] != 0x03 && AnswerBuffer[3] != 0x61) {
        HAL_UART_Transmit(&huart1, ChosePlayer2, sizeof(ChosePlayer2), 100);
        HAL_UART_Receive(&huart1, AnswerBuffer, sizeof(AnswerBuffer), HAL_MAX_DELAY);
      }
      memset(AnswerBuffer, 0, sizeof(AnswerBuffer));
      ChosePlayer2Flag = 0;
    }
    MPU6050_ReadRaw(&MPU6050Data);
    ComputeEulerAngles(&MPU6050Data);
    OLED_ShowFloatNum(0,0,MPU6050Data.pitch,3,3,OLED_6X8);
    OLED_ShowFloatNum(0,16,MPU6050Data.roll,3,3,OLED_6X8);

    if (MPU6050Data.pitch > 20) {
      HAL_UART_Transmit(&huart1, wakeup_bag, sizeof(wakeup_bag), 100);
    }
    if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == GPIO_PIN_SET || MPU6050Data.roll > 15) {
      HAL_Delay(50);
      if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == GPIO_PIN_SET || MPU6050Data.roll > 15) {
        HAL_UART_Transmit(&huart1,Left,sizeof(Left),100);
      }
    }
    if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == GPIO_PIN_SET || MPU6050Data.pitch < -15) {
      HAL_Delay(50);
      if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == GPIO_PIN_SET || MPU6050Data.pitch < -15) {
        HAL_UART_Transmit(&huart1,Jump,sizeof(Jump),100);
      }
    }
    if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == GPIO_PIN_SET || MPU6050Data.roll < -15) {
      HAL_Delay(50);
      if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == GPIO_PIN_SET || MPU6050Data.roll < -15) {
        HAL_UART_Transmit(&huart1,Right,sizeof(Right),100);
      }
    }

    OLED_Update();
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
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
