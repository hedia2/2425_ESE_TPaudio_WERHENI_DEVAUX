/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "components/MCP23S17.h"
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
h_shell_t h_shell;

extern I2C_HandleTypeDef hi2c2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t tx_buffer[128]; // Buffer pour la transmission
uint16_t rx_buffer[128]; // Buffer pour la réception
extern SAI_HandleTypeDef hsai_BlockA2;
extern SemaphoreHandle_t sem_usart1;

extern SAI_HandleTypeDef hsai_BlockB2;
int size = 20; // Taille du tableau
#define BUFFER_SIZE 128
uint8_t tx_buffert[BUFFER_SIZE];
#define SAMPLES 100

#define BUFFER_SIZE 100
uint16_t audio_buffer[BUFFER_SIZE];

void Generate_Triangle_Wave(void) {
    for (int i = 0; i < BUFFER_SIZE / 2; i++) {
        audio_buffer[i] = i * (65535 / (BUFFER_SIZE / 2));
    }
    for (int i = BUFFER_SIZE / 2; i < BUFFER_SIZE; i++) {
        audio_buffer[i] = 65535 - (i - BUFFER_SIZE / 2) * (65535 / (BUFFER_SIZE / 2));
    }
}



uint16_t getCHIP_ID(uint8_t reg, uint8_t I2C_addr)
{
   uint16_t data = 0;
   HAL_StatusTypeDef status;

   status = HAL_I2C_Mem_Read(&hi2c2, I2C_addr, reg, I2C_MEMADD_SIZE_8BIT, &data, 2, HAL_MAX_DELAY);
   if (status != HAL_OK){
       return -1;
   }
   return data;
}
void remplir(void) {
    for (int i = 0; i < 128; i++) {
        tx_buffer[i] = 0xA0; // Valeur constante (mi-hauteur pour un signal 16 bits non signé)
    }
}

void TAskFonction(void *argument)
{
	for (;;)
	{
		shell_run(&h_shell);

	}
}

void task_usart1IT(void){

	//shell_init(&h_shell);
	printf("partie d'interruption \r\n");
	shell_run(&h_shell);

}

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

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  MX_I2C2_Init();
  MX_SAI2_Init();
  /* USER CODE BEGIN 2 */

	__HAL_SAI_ENABLE(&hsai_BlockA2);

	h_shell.drv.receive = drv_uart2_receive;
	h_shell.drv.transmit = drv_uart2_transmit;

	xTaskCreate(task_usart1IT, "task_usart1IT", 256, NULL, 1, NULL);

	//MCP23S17_Init();
	//sgtl5000_init();
	//MCP23S17_WriteRegister(MCP23S17_GPIOA, 0x00);

	Generate_Triangle_Wave();
	uint16_t chip_ID = getCHIP_ID(CODEC_ID_REG, CODEC_ADDR);
/*
	remplir();
	HAL_SAI_Transmit_DMA(&hsai_BlockA2, (uint8_t*)tx_buffer, sizeof(tx_buffer) / sizeof(uint16_t));
			// Démarrer la réception
	HAL_SAI_Receive_DMA(&hsai_BlockB2, (uint8_t*)rx_buffer, sizeof(rx_buffer) / sizeof(uint16_t));
*/



	// CHIP ID = 160
	shell_init(&h_shell);
	//shell_run(&h_shell);
	//xTaskCreate(TAskFonction, "Task1", 128, NULL, osPriorityNormal, NULL);
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
		HAL_SAI_Transmit_DMA(&hsai_BlockA2, (uint8_t *)audio_buffer, BUFFER_SIZE);
		HAL_Delay(1000); // Transmission périodique
		printf("Toggle Led... \r\n");
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(1000);
		chenillard();

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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
  PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

        // Traitez les données reçues (rx_buffer contient l'octet reçu)
    	BaseType_t higher_priority_task_woken = pdFALSE;
    	xSemaphoreGiveFromISR(sem_usart1, &higher_priority_task_woken);
    	portYIELD_FROM_ISR(higher_priority_task_woken);

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
