/* USER CODE BEGIN Header */
///**
//  ******************************************************************************
//  * @file           : main.c
//  * @brief          : Main program body
//  ******************************************************************************
//  * @attention
//  *
//  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
//  * All rights reserved.</center></h2>
//  *
//  * This software component is licensed by ST under BSD 3-Clause license,
//  * the "License"; You may not use this file except in compliance with the
//  * License. You may obtain a copy of the License at:
//  *                        opensource.org/licenses/BSD-3-Clause
//  *
//  ******************************************************************************
//  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ws2812b.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#if defined (TEST_ENABLED)
//#include "../Tests/ws2812b_tests.h"
//#define TEST 1
//#else
//#define TEST 0
//#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /*
   * The LED strip uses PA7, PA7 is AF#0 (SPI1 SDO / MISO).
   * - set to alt func mode
   * - set AF5 for PA7 is SPI1_MOSI (GPIO_AFRL_AFSEL5)
   */
  GPIOA->MODER  &= ~( 0x3 << ( 7 * 2 ) );
  GPIOA->MODER  |= ( 0x2 << ( 7 * 2 ) );
  GPIOA->AFR[0] &= ~( 0x00 << (7 * 2) );
  GPIOA->AFR[0] |= ( 0x5 << (7 * 4) );

  /*
   * GPIO config
   * - set as input
   * - enable pull up resistor
   *
   * Button pins
   * - Power button uses PA0
   * - Mode switch left uses PA1
   * - Mode switch right uses PA3
   */
  GPIOA->MODER	&= ~( (0x3 << 0 * 2 ) | (0x3 << 1 * 2 ) | (0x3 << 3 * 2 ) );
  GPIOA->PUPDR  &= ~( (0x0 << 0 * 2 ) | (0x0 << 1 * 2 ) | (0x0 << 3 * 2 ) );
  GPIOA->MODER  |=  ( (0x0 << 0 * 2 ) | (0x0 << 1 * 2 ) | (0x0 << 3 * 2 ) );
  GPIOA->PUPDR  |=  ( (0x1 << 0 * 2 ) | (0x1 << 1 * 2 ) | (0x1 << 3 * 2 ) );

  /*
   * DMA config (ch1)
   * - Memory to peripheral
   * - Circular mode
   * - Increment memory ptr, don't increment peripheral ptr
   * - Bit data size for both source and destination
   * - High priority
   */
  DMA1_Channel3->CCR &= ~( DMA_CCR_MEM2MEM |
						   DMA_CCR_PL |
						   DMA_CCR_MSIZE |
						   DMA_CCR_PSIZE |
						   DMA_CCR_PINC |
						   DMA_CCR_EN );
  DMA1_Channel3->CCR |=  (( 0x2 << DMA_CCR_PL_Pos ) |
						   DMA_CCR_MINC |
						   DMA_CCR_CIRC |
						   DMA_CCR_DIR );

  // Source: Address of the framebuffer.
  DMA1_Channel3->CMAR  = ( uint32_t )&COLORS;
  // Destination: SPI1 data register.
  DMA1_Channel3->CPAR  = ( uint32_t )&( SPI1->DR );
  // Set DMA data transfer length (framebuffer length).
  DMA1_Channel3->CNDTR = ( uint16_t )LED_BYTES;

  /*
   * SPI1 configuration:
   * - Clock phase = 1
   * - Clock polarity = 1
   * - Enable software peripheral control
   * - MSB first
   * - 8-bit frames
   * - Baud rate prescaler of 8
   * - TX DMA requests enabled.
   */
  SPI1->CR1 &= ~( SPI_CR1_LSBFIRST |
				  SPI_CR1_BR );
  SPI1->CR1 |=  ( SPI_CR1_SSM |
				  SPI_CR1_SSI |
				  0x2 << SPI_CR1_BR_Pos |
				  SPI_CR1_MSTR |
				  SPI_CR1_CPOL |
				  SPI_CR1_CPHA );
  SPI1->CR2 &= ~( SPI_CR2_DS );
  SPI1->CR2 |=  ( 0x7 << SPI_CR2_DS_Pos |
				  SPI_CR2_TXDMAEN );

  SPI1->CR1 |=  ( SPI_CR1_SPE );
  DMA1_Channel3->CCR |= ( DMA_CCR_EN );

  init_fire_effect();
  unsigned int on_state = 1;
  unsigned int mode = 1;
  unsigned int mode_upper_bound = 4;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // test build config program loop
//  if (TEST == 1)
//  {
//	  test_main();
//	  return;
//  }

  while (1)
  {
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */

	// power switching logic
//	if ( !(GPIOA->IDR & ( 1 << 3)) )
//	{
//		if (on_state == 1)
//		{
//			on_state = 0;
//			reset_leds();
//			delay_cycles( 2000000 );
//		}
//		else
//		{
//			on_state = 1;
//			init_fire_effect();
//		}
//	}
//


	if (on_state == 1)
	{
		// mode switching logic
		if ( !(GPIOA->IDR & ( 1 << 1 )) )
		{
			mode -= 1;
			if (mode <= 0)
			{
				mode = mode_upper_bound;
			}
			delay_cycles( 1000000 );
			init_fire_effect();
		}
		else if ( !(GPIOA->IDR & ( 1 << 0 )) )
		{
			mode += 1;
			if (mode > mode_upper_bound)
			{
				mode = 1;
			}
			delay_cycles( 1000000 );
			init_fire_effect();
		}

		// LED animation logic
		if (mode == 1)
		{
			set_fire_effect();
		}
		else if (mode == 2)
		{
			set_bi_effect();
		}
		else if (mode == 3)
		{
			set_funky_effect();
		}
		else if (mode == 4)
		{
			set_L_effect();
		}
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 19;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
