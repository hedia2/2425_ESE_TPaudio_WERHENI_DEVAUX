#ifndef MCP23S17_H_
#define MCP23S17_H_

#include "shell.h"
#include "spi.h"


// BANK =1
#define IODIRA_Addr 0x00
#define IODIRB_Addr 0x10
#define GPINTENA_Addr 0x01

#define MCP23S17_OLATA    0x14    // Address of OLATA register
#define MCP23S17_OLATB    0x15    // Address of OLATB register
#define MCP23S17_OPCODE   0x40    // Default opcode (0100 0000)


#define MCP23S17_ADDR 0x40
#define MCP23S17_SPI hspi3

void MCP23S17_ToggleAllPins(SPI_HandleTypeDef *hspi);
void MCP23S17_Init_And_Test(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef MCP23S17_WriteRegister(uint8_t reg, uint8_t data);


#endif /* MCP23S17_H_ */
