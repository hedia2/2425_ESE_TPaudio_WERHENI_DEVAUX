#include "components/MCP23S17.h"




HAL_StatusTypeDef MCP23S17_WriteRegister(uint8_t reg, uint8_t data){
	uint8_t pData[3];

	pData[0] = MCP23S17_ADDR;
	pData[1] = reg;
	pData[2] = data;

	HAL_GPIO_WritePin(CS_Vu_GPIO_Port, CS_Vu_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&MCP23S17_SPI, pData, 3, HAL_MAX_DELAY);
	HAL_Delay(10);
	HAL_GPIO_WritePin(CS_Vu_GPIO_Port, CS_Vu_Pin, GPIO_PIN_SET);
	return ret;
}

void MCP23S17_Init()
{
	HAL_GPIO_WritePin(RESET_Vu_GPIO_Port, RESET_Vu_Pin, GPIO_PIN_SET);
	MCP23S17_WriteRegister(IODIRA, CONFIG);
	MCP23S17_WriteRegister(IODIRB, CONFIG);
}
