#include "components/MCP23S17.h"



HAL_StatusTypeDef MCP23S17_WriteRegister(uint8_t reg, uint8_t data){
	uint8_t pData[3];

	pData[0] = MCP23S17_ADDR ;
	pData[1] = reg;
	pData[2] = data;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&MCP23S17_SPI, pData, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	return ret;

}

void MCP23S17_ToggleAllPins(SPI_HandleTypeDef *hspi)
{
    uint8_t tx_data[3];

    // Pull CS pin low to start transmission
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

    // Write to OLATA - Set all PORTA pins high
    tx_data[0] = MCP23S17_OPCODE;      // Opcode for write
    tx_data[1] = MCP23S17_OLATA;       // Register address
    tx_data[2] = 0xFF;                 // Data - all pins high
    HAL_SPI_Transmit(hspi, tx_data, 3, 100);

    // Write to OLATB - Set all PORTB pins high
    tx_data[1] = MCP23S17_OLATB;       // Change register to OLATB
    HAL_SPI_Transmit(hspi, tx_data, 3, 100);

    // Pull CS pin high to end transmission
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
}

void MCP23S17_Init_And_Test(SPI_HandleTypeDef *hspi)
{
    uint8_t tx_data[3];

    // Configure all pins as outputs
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    tx_data[0] = MCP23S17_OPCODE;
    tx_data[1] = IODIRA_Addr;
    tx_data[2] = 0x00;  // All pins as outputs
    HAL_SPI_Transmit(hspi, tx_data, 3, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

    // Configure PORTB as outputs
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    tx_data[1] = IODIRB_Addr;
    HAL_SPI_Transmit(hspi, tx_data, 3, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

    // Toggle outputs on/off every second
    while(1) {
    	MCP23S17_ToggleAllPins(hspi);  // Turn all on
        HAL_Delay(1000);

        // Turn all off
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
        tx_data[1] = MCP23S17_OLATA;
        tx_data[2] = 0x00;
        HAL_SPI_Transmit(hspi, tx_data, 3, 100);
        tx_data[1] = MCP23S17_OLATB;
        HAL_SPI_Transmit(hspi, tx_data, 3, 100);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

        HAL_Delay(1000);
    }
}


