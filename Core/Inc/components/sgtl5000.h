#ifndef  SGTL5000_H_
#define SGTL5000_H_

#include "shell.h"
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "stm32l4xx_hal_i2c.h"

#define SGTL5000_I2C_ADDR	 0x14

#define CHIP_LINREG_CTRL     0x0026
#define CHIP_REF_CTRL        0x0028
#define CHIP_LINE_OUT_CTRL   0x002C
#define CHIP_SHORT_CTRL      0x003C
#define CHIP_ANA_CTRL        0x0024
#define CHIP_ANA_POWER       0x0030
#define CHIP_DIG_POWER       0x0002
#define CHIP_LINE_OUT_VOL    0x002E
#define CHIP_CLK_CTRL        0x0004
#define CHIP_I2S_CTRL        0x0006
#define CHIP_ADCDAC_CTRL     0x000E
#define CHIP_DAC_VOL         0x0010


#define LINREG_VAL           0x006C
#define ANA_POWER_VAL        0x4260
#define REF_VAL              0x004E
#define LINE_OUT_VAL         0x0322
#define SHORT_VAL            0x1106
#define DIG_POWER_VAL        0x0073


HAL_StatusTypeDef SGTL5000_Init(void);
static HAL_StatusTypeDef SGTL5000_WriteReg(uint16_t reg, uint16_t value);

#endif /* SGTL5000_H_ */
