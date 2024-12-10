#ifndef MCP23S17_H_
#define MCP23S17_H_

#include "shell.h"
#include "spi.h"


#define IODIRA 0x00
#define IODIRB 0x01

#define CONFIG 0x00

#define MCP23S17_GPIOA   0x12
#define MCP23S17_GPIOB    0x13
#define MCP23S17_OPCODE   0x40


#define MCP23S17_ADDR 0x40
#define MCP23S17_SPI hspi3

void MCP23S17_Init();
HAL_StatusTypeDef MCP23S17_WriteRegister(uint8_t reg, uint8_t data);
//void MCP23S17_Init();
//void MCP23S17_Write(uint8_t reg, uint8_t data);
void chenillard();


#endif /* MCP23S17_H_ */
