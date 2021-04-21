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
I2S_HandleTypeDef hi2s1;
DMA_HandleTypeDef hdma_spi1_rx;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2S1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t rxBuf[16384];
uint16_t txBuf[16384];
float fft_in_buf[2048];
float fft_out_buf[2048];


arm_rfft_fast_instance_f32 fft_handler;

float real_fsample = 46875;
uint8_t callback_state = 0;
uint8_t outarray[14];
uint8_t uartfree = 1;

float complexABS(float real, float compl) {
	return sqrtf(real*real+compl*compl);
}
void DoFFT() {
	//Do FFT
	arm_rfft_fast_f32(&fft_handler, &fft_in_buf,&fft_out_buf,0);

	int freqs[1024];
	int freqpoint = 0;
	int offset = 150; //variable noisefloor offset

	//calculate abs values and linear-to-dB
	for (int i=0; i<2048; i=i+2) {
		freqs[freqpoint] = (int)(20*log10f(complexABS(fft_out_buf[i], fft_out_buf[i+1])))-offset;
		if (freqs[freqpoint]<0) freqs[freqpoint]=0;
		freqpoint++;
	}


	//push out data to Uart
	outarray[0] = 0xff; //frame start
	outarray[1] = (uint8_t)freqs[1]; //31-5Hz
	outarray[2] = (uint8_t)freqs[3]; //63 Hz
	outarray[3] = (uint8_t)freqs[5]; //125 Hz
	outarray[4] = (uint8_t)freqs[11]; //250 Hz
	outarray[5] = (uint8_t)freqs[22]; //500 Hz
	outarray[6] = (uint8_t)freqs[44]; //1 kHz
	outarray[7] = (uint8_t)freqs[96]; //2.2 kHz
	outarray[8] = (uint8_t)freqs[197]; //4.5 kHz
	outarray[9] = (uint8_t)freqs[393]; //9 kHz
	outarray[10] = (uint8_t)freqs[655]; //15 lHz


	if (uartfree==1) HAL_UART_Transmit_DMA(&huart2, &outarray[0], 11);
	uartfree = 0;
	callback_state=0;

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_I2S1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_I2S_Transmit_DMA(&hi2s2, txBuf, 16384);
  HAL_I2S_Receive_DMA(&hi2s1, rxBuf, 16384);

  arm_rfft_fast_init_f32(&fft_handler, 2048);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  int fft_in_ptr = 0;
	  if (callback_state == 1) {
		  for (int i=0; i<8192; i=i+4) {
			  fft_in_buf[fft_in_ptr] = (float) ((int) (rxBuf[i]<<16)|rxBuf[i+1]);
			  fft_in_buf[fft_in_ptr] += (float) ((int) (rxBuf[i+2]<<16)|rxBuf[i+3]);
			  txBuf[i] = rxBuf[i];
			  txBuf[i+1] = rxBuf[i+1];
			  txBuf[i+2] = rxBuf[i+2];
			  txBuf[i+3] = rxBuf[i+3];
			  fft_in_ptr++;
		  }

		  DoFFT();
	  }

	  if (callback_state == 2) {
		  for (int i=8192; i<16384; i=i+4) {
			  fft_in_buf[fft_in_ptr] = (float) ((int) (rxBuf[i]<<16)|rxBuf[i+1]);
			  fft_in_buf[fft_in_ptr] += (float) ((int) (rxBuf[i+2]<<16)|rxBuf[i+3]);
			  txBuf[i] = rxBuf[i];
			  txBuf[i+1] = rxBuf[i+1];
			  txBuf[i+2] = rxBuf[i+2];
			  txBuf[i+3] = rxBuf[i+3];
			  fft_in_ptr++;
		  }


		  DoFFT();

	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB2;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 50;
  PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLI2SP_DIV2;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
  PeriphClkInitStruct.PLLI2SDivQ = 1;
  PeriphClkInitStruct.I2sApb2ClockSelection = RCC_I2SAPB2CLKSOURCE_PLLI2S;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S1_Init(void)
{

  /* USER CODE BEGIN I2S1_Init 0 */

  /* USER CODE END I2S1_Init 0 */

  /* USER CODE BEGIN I2S1_Init 1 */

  /* USER CODE END I2S1_Init 1 */
  hi2s1.Instance = SPI1;
  hi2s1.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s1.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s1.Init.CPOL = I2S_CPOL_LOW;
  hi2s1.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s1.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S1_Init 2 */

  /* USER CODE END I2S1_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s1)
{
	  /*
	  int left=(rxBuf[0]<<16 | rxBuf[1]);
	  int right=(rxBuf[2]<<16 | rxBuf[3]);
	  txBuf[0]=(left>>16)&0xFFFF;
	  txBuf[1]=left&0xFFFF;
	  txBuf[2]=(right>>16)&0xFFFF;
	  txBuf[3]=right&0xFFFF;
	  */
	callback_state = 1;
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s1)
{
		/*
	  int left=(rxBuf[4]<<16 | rxBuf[5]);
	  int right=(rxBuf[6]<<16 | rxBuf[7]);
	  txBuf[4]=(left>>16)&0xFFFF;
	  txBuf[5]=left&0xFFFF;
	  txBuf[6]=(right>>16)&0xFFFF;
	  txBuf[7]=right&0xFFFF;
	  */
	callback_state = 2;
}

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
