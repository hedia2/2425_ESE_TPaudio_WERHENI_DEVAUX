#include "components/sgtl5000.h"
#include "main.h"
#include "stm32l4xx_hal_i2c.h"


static HAL_StatusTypeDef SGTL5000_WriteReg(uint16_t reg, uint16_t value)
{
	uint8_t data[2];
	data[0] = (value >> 8) & 0xFF;
	data[1] = value & 0xFF;

	uint8_t reg_addr[2];
	reg_addr[0] = (reg >> 8) & 0xFF;
	reg_addr[1] = reg & 0xFF;

	return HAL_I2C_Mem_Write(&hi2c2, SGTL5000_I2C_ADDR,
			((uint16_t)reg_addr[0] << 8) | reg_addr[1],
			I2C_MEMADD_SIZE_16BIT, data, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SGTL5000_Init(void)
{
	HAL_StatusTypeDef status;

	status = SGTL5000_WriteReg(CHIP_ANA_POWER, ANA_POWER_VAL);
	if(status != HAL_OK) return status;
	//voir datasheet recall ana_power Write CHIP_ANA_POWER 0x4A60

	status = SGTL5000_WriteReg(CHIP_LINREG_CTRL, LINREG_VAL);
	if(status != HAL_OK) return status;


	status = SGTL5000_WriteReg(CHIP_REF_CTRL, REF_VAL);
	if(status != HAL_OK) return status;

	status = SGTL5000_WriteReg(CHIP_LINE_OUT_CTRL, LINE_OUT_VAL);
	if(status != HAL_OK) return status;

	//------------Other Analog Block Configurations--------------
	status = SGTL5000_WriteReg(CHIP_REF_CTRL, 0x004F);
	if(status != HAL_OK) return status;


	status = SGTL5000_WriteReg(CHIP_SHORT_CTRL, SHORT_VAL);
	if(status != HAL_OK) return status;

	status = SGTL5000_WriteReg(CHIP_ANA_CTRL, 0x0133);
	if(status != HAL_OK) return status;

	status = SGTL5000_WriteReg(CHIP_ANA_POWER, 0x6AFF);
	if(status != HAL_OK) return status;

	status = SGTL5000_WriteReg(CHIP_DIG_POWER, DIG_POWER_VAL);
	if(status != HAL_OK) return status;

	status = SGTL5000_WriteReg(CHIP_LINE_OUT_CTRL, 0x0505);
	if(status != HAL_OK) return status;

	HAL_Delay(10);

	return HAL_OK;
}
