/*
 * drv_uart1.c
 *
 *  Created on: 7 nov. 2022
 *      Author: laurentf
 */

#include "drv_uart1.h"

#include "usart.h"
#include "gpio.h"

uint8_t drv_uart2_receive(char * pData, uint16_t size)
{
	HAL_UART_Receive(&huart2, (uint8_t*)(pData), size, HAL_MAX_DELAY);

	return 0;
}

uint8_t drv_uart2_transmit(const char * pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)pData, size, HAL_MAX_DELAY);

	return 0;
}
