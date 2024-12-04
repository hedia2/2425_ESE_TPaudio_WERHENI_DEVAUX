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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "shell.h"
#include "drv_uart1.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


//#define CS_Pin GPIO_PIN_4 // Pin CS (Chip Select) - à adapter
//#define CS_GPIO_Port GPIOA // Port de la pin CS - à adapter

// Commandes pour MCP23S17
#define MCP23S17_WRITE 0x40
#define MCP23S17_READ  0x41

// Registres MCP23S17
//#define MCP23S17_IODIRA 0x00
//#define MCP23S17_GPIOA  0x12

#define MCP23S17_IODIRB 0x01
#define MCP23S17_GPIOB  0x13

#define MCP23S17_IODIRA 0x00
#define MCP23S17_IODIRB 0x01
#define MCP23S17_IOCON 0x0A
#define MCP23S17_OLATA 0x14
#define MCP23S17_OLATB 0x15

#define PortA 0x13
#define PortB 0x14

#define DEVICEADD 0x40

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

int fonction(h_shell_t * h_shell, int argc, char ** argv)
{
	int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Je suis une fonction bidon\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}



void MCP23S17_Init(void) {
    // Configurer tous les pins comme sortie (IODIRA)
    HAL_GPIO_WritePin(Vu_Reset_GPIO_Port, Vu_Reset_Pin, GPIO_PIN_SET); // Réinitialiser le MCP23S17
    HAL_Delay(1); // Attendre un peu après la réinitialisation
    MCP23S17_Write(0x00, 0x00); // Configurer tous les pins comme sortie
    MCP23S17_Write(0x01, 0x00);
    printf("TestInit SPI\r\n");
}

void MCP23S17_Write(uint8_t reg, uint8_t data) {
    uint8_t spiData[3];
    spiData[0] = MCP23S17_WRITE; // Code d'écriture
    spiData[1] = reg;                     // Registre
    spiData[2] = data;                    // Valeur à écrire

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // Activer CS
    if (HAL_SPI_Transmit(&hspi3, spiData, 3, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); // Désactiver CS
        printf("Erreur de transmission SPI\r\n");
        return 0; // Sortir en cas d'erreur
    }
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); // Désactiver CS
    printf("TestWrite SPI\r\n");
}


/*
void MCP23S17_Write(uint8_t reg, uint8_t data) {
    uint8_t control_byte = 0x40; // Adresse périphérique en écriture
    uint8_t tx_buffer[2] = {reg, data};

    // Activer CS (NSS)
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // Modifier selon votre broche NSS
    HAL_SPI_Transmit(&hspi3, &control_byte, 1, HAL_MAX_DELAY); // Envoyer le byte de contrôle
    HAL_SPI_Transmit(&hspi3, tx_buffer, 2, HAL_MAX_DELAY);     // Envoyer l'adresse et la donnée
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);   // Désactiver CS (NSS)
    printf("TestWrite SPI\r\n");
}
*/
void Configure_LED() {

    // Configurer GPA0 comme sortie
    MCP23S17_Write(0x00, 0x00); // IODIRA : GPA0 en sortie (les autres en entrée)

    // Allumer la LED sur GPA0
    MCP23S17_Write(0x14, 0x01); // OLATA : Met GPA0 à 1
    printf("TestConfig SPI:\r\n");
}


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
h_shell_t h_shell;
HAL_StatusTypeDef status;
//SemaphoreHandle_t xSemaphore_UART;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void TAskFonction(void *argument)
{
	for (;;)
	{
		shell_run(&h_shell);

	}
}
/*
void TAskLed(void *argument)
{
	uint8_t spiData[4];
		spiData[0] = 0x40;
		spiData[1] = 0x00;
		spiData[2] = 0x12;
		spiData[3] = 0x01;
printf("TestStatus SPI: %i", status);

	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 1);
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 0);
	HAL_GPIO_WritePin(Vu_Reset_GPIO_Port,Vu_Reset_Pin, 0);
	status = HAL_SPI_Transmit(&hspi3, spiData, sizeof(spiData), 1000); // Transmettre via SPI
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 1);
}
*/




void chenillard(void) {
    // Supposons que vous avez 8 LEDs (A0 à A7 sur le MCP23S17)
    for (int i = 0; i < 8; i++) {
        // Allumer la LED correspondante (i)
        MCP23S17_Write(0x12, ~(1 << i)); // Éteindre toutes les autres LEDs
        MCP23S17_Write(0x13, ~(1 << i));
        printf("LED %d allumée\r\n", i + 1);
        HAL_Delay(100); // Attendre 100 ms

        // Éteindre la LED actuelle avant de passer à la suivante
        MCP23S17_Write(0x12, 0xFF); // Éteindre toutes les LEDs
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	// Code pour allumer la LED via SPI (en fonction du GPIO Expander)

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
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
	char msg[] = "USART2 Test OK\r\n";
	//printf("TestStatus SPI: %i\r\n", status);

	h_shell.drv.receive = drv_uart1_receive;
	h_shell.drv.transmit = drv_uart1_transmit;

	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	//shell_run(&h_shell);
	printf("Test1Status SPI: %i\r\n", status);

	/*
	uint8_t spiData[4];
		spiData[0] = 0x40;
		spiData[1] = 0x00;
		spiData[2] = 0x12;
		spiData[3] = 0x01;
		HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 1);
			HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 0);
			HAL_GPIO_WritePin(Vu_Reset_GPIO_Port,Vu_Reset_Pin, 0);
			status = HAL_SPI_Transmit(&hspi3, spiData, sizeof(spiData), 1000); // Transmettre via SPI
			HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, 1);
			*/
	    // MCP23S17_Init(); // Initialiser le MCP23S17
	//Configure_LED(); // Configurer et allumer la LED
	    // Exemple d'écriture sur le GPIOA (allumer toutes les LED)
	    //MCP23S17_Write(MCP23S17_GPIOA, 0xFF); // Mettre à HIGH toutes les sorties



	//MCP23S17_Init();
	//MCP23S17_Write(0x00, 0x12); // Configure IODIRA

	//MCP23S17_Write(0x12, 0x01); // Écrivez pour allumer la LED sur GPA0
	// Créer une tâche FreeRTOS
	//xTaskCreate(TAskFonction, "Task1", 128, NULL, osPriorityNormal, NULL);
	//xTaskCreate(TAskLed, "Task1", 128, NULL, osPriorityNormal, NULL);
	// Lancer le scheduler
	//vTaskStartScheduler();
	//  xSemaphore_UART = xSemaphoreCreateBinary();

	// Créer la tâche du shell
	   //xTaskCreate(Task_Shell, "Shell Task", 128, NULL, 2, NULL);

	// Démarrer le scheduler
	//vTaskStartScheduler();


/*
	uint8_t spiData[4];
	spiData[0] = 0x40;
	spiData[1] = 0x00;
	spiData[2] = 0x12;
	spiData[3] = 0x01;
	*/
	//xTaskCreate(TAskLed, "Task1", 128, NULL, 1, NULL);

	MCP23S17_Init();
	//MCP23S17_Write(0x13, 0x00);
	//MCP23S17_Write(0x12, 0x00);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  //MX_FREERTOS_Init();

  /* Start scheduler */
 // osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


		  //HAL_Delay(1000); // Attendre 1 seconde
          printf("Test1Status:\r\n");

          chenillard(); // Appeler la fonction chenillard


          //MCP23S17_Write(0x15, 0x00);
		 // MCP23S17_Write(0x14, 0x00);
		  //MCP23S17_Write(0x13, 0x00);
		  //MCP23S17_Write(0x12, 0x00);

          //MCP23S17_Write(0x12, 0xFF);
		      //  HAL_Delay(1000); // Attendre 1 seconde
		        //MCP23S17_Write(MCP23S17_GPIOA, 0xFF); // Allumer toutes les LED
/*
		        // Faire clignoter la LED
		printf("Test2Status SPI: %i\r\n", status);
		               MCP23S17_Write(0x14, 0x01); // Allumer GPA0
		               printf("Test2eteindre SPI:\r\n");
		               HAL_Delay(500);
		              // printf("Test2eteindre SPI:\r\n");
		               //MCP23S17_Write(0x14, 0x00); // Éteindre GPA0
		               //HAL_Delay(500);		                */
		/*
		        /*
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // Inverse l'état de la LED
	  HAL_Delay(500);

	  HAL_UART_Transmit(&huart2, (uint8_t *)msg, sizeof(msg) - 1, HAL_MAX_DELAY);
	  HAL_Delay(1000);  // Envoi toutes les secondes
	  */

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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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
