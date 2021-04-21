/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define ARM_MATH_CM4
#include "arm_math.h"
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t rxBuf[16384] = {500};
uint16_t txBuf[16384];
float fftInBuf[2048] = {500};
float fftOutBuf[2048];

arm_rfft_fast_instance_f32 fftHandler;

float realSample = 48828;
uint8_t callbackState = 0;
uint8_t outBuf[14];
uint8_t uartFree = 1;

float complexABS(float real, float compl)
{
	return sqrt(real*real+compl*compl);
}

void doFFT()
{
	arm_rfft_fast_f32(&fftHandler, fftInBuf, fftOutBuf, 0);

	int freqs[512];
	int freqPoint = 0;
	int offset = 150; // variable noisefloor

	int i = 0;
	for (i = 0; i< 1024; i += 2)
	{
		freqs[freqPoint] = (int)(28*log10f(complexABS(fftOutBuf[i], fftOutBuf[i+1]))) - offset;
		if (freqs[freqPoint] < 0)
		{
			freqs[freqPoint] = 0;
		}
		freqPoint++;
	}

	uartFree = 0;
	callbackState = 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  /* FPU settings ------------------------------------------------------------*/
	  //#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	  //  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
	  //#endif
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //HAL_I2S_Transmit_DMA(&hi2s2, txBuf, 16384);
  //HAL_I2S_Receive_DMA(&hi2s1, rxBuf, 1024);

  arm_rfft_fast_init_f32(&fftHandler, 2048);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	  int fftInPtr = 0;
/*)
	  	  {
	  		  int i = 0;
	  		  for (i = 0; i < 8192; i += 4)
	  		  {
	  			  fftInBuf[fftInPtr] = (float) ((int) (rxBuf[i] << 16)|(rxBuf[i+1]));
	  			  fftInBuf[fftInPtr] += (float) ((int) (rxBuf[i+2] << 16)|(rxBuf[i+3]));
				  txBuf[i] = rxBuf[i];
				  txBuf[i+1] = rxBuf[i+1];
				  txBuf[i+2] = rxBuf[i+2];
				  txBuf[i+3] = rxBuf[i+3];
				  fftInPtr++;
	  		  }
	  	  }

	  	  if (callbackState == 2)
	  	  {
	  		  int i = 8192;
	  		  for (i = 8192; i < 16384; i += 4)
	  		  {
	  			  fftInBuf[fftInPtr] = (float) ((int) (rxBuf[i] << 16)|(rxBuf[i+1]));
	  			  fftInBuf[fftInPtr] = (float) ((int) (rxBuf[i+2] << 16)|(rxBuf[i+3]));
				  txBuf[i] = rxBuf[i];
				  txBuf[i+1] = rxBuf[i+1];
				  txBuf[i+2] = rxBuf[i+2];
				  txBuf[i+3] = rxBuf[i+3];
				  fftInPtr++;
	  		  }
	  	  }
	  	  */

	  	arm_rfft_fast_f32(&fftHandler, &fftInBuf[0], &fftOutBuf[0], 0);

	  		int freqs[512];
	  		int freqPoint = 0;
	  		int offset = 150; // variable noisefloor

	  		int i = 0;
	  		for (i = 0; i< 1024; i += 2)
	  		{
	  			freqs[freqPoint] = (int)(28*log10f(complexABS(fftOutBuf[i], fftOutBuf[i+1]))) - offset;
	  			if (freqs[freqPoint] < 0)
	  			{
	  				freqs[freqPoint] = 0;
	  			}
	  			freqPoint++;
	  		}

	  		uartFree = 0;
	  		callbackState = 0;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/*
//void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s1)
//{
//	  /*
//	  int left=(rxBuf[0]<<16 | rxBuf[1]);
//	  int right=(rxBuf[2]<<16 | rxBuf[3]);
//	  txBuf[0]=(left>>16)&0xFFFF;
//	  txBuf[1]=left&0xFFFF;
//	  txBuf[2]=(right>>16)&0xFFFF;
//	  txBuf[3]=right&0xFFFF;
//	  */
//	callbackState = 1;
//}


//void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s1)
//{
//		/*
//	  int left=(rxBuf[4]<<16 | rxBuf[5]);
//	  int right=(rxBuf[6]<<16 | rxBuf[7]);
//	  txBuf[4]=(left>>16)&0xFFFF;
//	  txBuf[5]=left&0xFFFF;
//	  txBuf[6]=(right>>16)&0xFFFF;
//	  txBuf[7]=right&0xFFFF;
//	  */
//	callbackState = 2;
//}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
