#include "components/sgtl5000.h"
#include "main.h"
#include "stm32l4xx_hal_i2c.h"

HAL_StatusTypeDef dump_status;


HAL_StatusTypeDef sgtl5000_WriteReg(h_sgtl5000_t * h_sgtl5000, uint16_t reg, uint16_t value)
{
    uint8_t data[2];
    data[0] = (value >> 8) & 0xFF;    // MSB of value
    data[1] = value & 0xFF;           // LSB of value

    return HAL_I2C_Mem_Write(h_sgtl5000->hi2c,
                            h_sgtl5000->device_address,
                            reg,
                            I2C_MEMADD_SIZE_16BIT,
                            data,
                            2,
                            HAL_MAX_DELAY);
}

HAL_StatusTypeDef sgtl5000_init(h_sgtl5000_t * h_sgtl5000)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint16_t mask;

	/* Chip Powerup and Supply Configurations */

	//--------------- Power Supply Configuration----------------
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// Configure VDDD level to 1.8V (bits 3:0)
	// Write CHIP_LINREG_CTRL 0x????
	// OK, pas touche!
	// Power up internal linear regulator (Set bit 9)
	// Write CHIP_ANA_POWER 0x7260
	// Pas touche non plus

	// NOTE: This next Write call is needed ONLY if VDDD is
	// externally driven
	// Turn off startup power supplies to save power (Clear bit 12 and 13)
	// Write CHIP_ANA_POWER 0x4260
	mask = (1 << 12) | (1 << 13);
	sgtl5000_i2c_clear_bit(h_sgtl5000, SGTL5000_CHIP_ANA_POWER, mask);

	// NOTE: The next Write calls is needed only if both VDDA and
	// VDDIO power supplies are less than 3.1V.
	// Enable the internal oscillator for the charge pump (Set bit 11)
	// Write CHIP_CLK_TOP_CTRL 0x0800
	// Enable charge pump (Set bit 11)
	// Write CHIP_ANA_POWER 0x4A60
	// VDDA and VDDIO = 3.3V so not necessary

	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1 V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	// Write CHIP_LINREG_CTRL 0x006C
	// VDDA and VDDIO = 3.3V so it IS necessary
	mask = (1 << 5) | (1 << 6);
	sgtl5000_i2c_set_bit(h_sgtl5000, SGTL5000_CHIP_LINREG_CTRL, mask);

	//---- Reference Voltage and Bias Current Configuration----
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8 V. VDDA/2 = 0.9 V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	// Write CHIP_REF_CTRL 0x004E
	mask = 0x01FF;	// VAG_VAL = 1.575V, BIAS_CTRL = -50%, SMALL_POP = 1
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_REF_CTRL, mask);

	// Set LINEOUT reference voltage to VDDIO/2 (1.65 V) (bits 5:0)
	// and bias current (bits 11:8) to the recommended value of 0.36 mA
	// for 10 kOhm load with 1.0 nF capacitance
	// Write CHIP_LINE_OUT_CTRL 0x0322
//	mask = 0x0322;	// LO_VAGCNTRL = 1.65V, OUT_CURRENT = 0.36mA (?)
	mask = 0x031E;
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_LINE_OUT_CTRL, mask);

	//------------Other Analog Block Configurations--------------
	// Configure slow ramp up rate to minimize pop (bit 0)
	// Write CHIP_REF_CTRL 0x004F
	// Déjà fait

	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75 mA
	// Write CHIP_SHORT_CTRL 0x1106
	mask = 0x1106;	// MODE_CM = 2, MODE_LR = 1, LVLADJC = 200mA, LVLADJL = 75mA, LVLADJR = 50mA
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_SHORT_CTRL, mask);

	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	// Write CHIP_ANA_CTRL 0x0133
	mask = 0x0004;	// Unmute all + SELECT_ADC = LINEIN
//	mask = 0x0000;	// Unmute all + SELECT_ADC = MIC
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ANA_CTRL, mask);

	//------------Power up Inputs/Outputs/Digital Blocks---------
	// Power up LINEOUT, HP, ADC, DAC
	// Write CHIP_ANA_POWER 0x6AFF
	mask = 0x6AFF;	// LINEOUT_POWERUP, ADC_POWERUP, CAPLESS_HEADPHONE_POWERUP, DAC_POWERUP, HEADPHONE_POWERUP, REFTOP_POWERUP, ADC_MONO = stereo
	// VAG_POWERUP, VCOAMP_POWERUP = 0, LINREG_D_POWERUP, PLL_POWERUP = 0, VDDC_CHRGPMP_POWERUP, STARTUP_POWERUP = 0, LINREG_SIMPLE_POWERUP,
	// DAC_MONO = stereo
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ANA_POWER, mask);
	// Power up desired digital blocks
	// I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
	// ADC (bit 6) are powered on
	// Write CHIP_DIG_POWER 0x0073
	mask = 0x0073;	// I2S_IN_POWERUP, I2S_OUT_POWERUP, DAP_POWERUP, DAC_POWERUP, ADC_POWERUP
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_DIG_POWER, mask);

	//----------------Set LINEOUT Volume Level-------------------
	// Set the LINEOUT volume level based on voltage reference (VAG)
	// values using this formula
	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9 V and
	// 1.65 V respectively, the // left LO vol (bits 12:8) and right LO
	// volume (bits 4:0) value should be set // to 5
	// Write CHIP_LINE_OUT_VOL 0x0505
	mask = 0x1111;	// TODO recalculer
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_LINE_OUT_VOL, mask);

	/* System MCLK and Sample Clock */

	// Configure SYS_FS clock to 48 kHz
	// Configure MCLK_FREQ to 256*Fs
	// Modify CHIP_CLK_CTRL->SYS_FS 0x0002 // bits 3:2
	// Modify CHIP_CLK_CTRL->MCLK_FREQ 0x0000 // bits 1:0
	mask = 0x0004;	// SYS_FS = 48kHz
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_CLK_CTRL, mask);
	// Configure the I2S clocks in master mode
	// NOTE: I2S LRCLK is same as the system sample clock
	// Modify CHIP_I2S_CTRL->MS 0x0001 // bit 7
	// Non, on reste en slave!
	mask = 0x0130;	// DLEN = 16 bits
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_I2S_CTRL, mask);

	/* PLL Configuration */
	// Pas utilisé

	/* Input/Output Routing */
	// Laissons tout par défaut pour l'instant
//	mask = 0x0000;	// ADC -> DAC
//	sgtl5000_i2c_write_register(h_sgtl5000, SGTL5000_CHIP_SSS_CTRL, mask);

	/* Le reste */
	mask = 0x0000;	// Unmute
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ADCDAC_CTRL, mask);

	mask = 0x3C3C;
//	mask = 0x4747;
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_DAC_VOL, mask);

	mask = 0x0251;	// BIAS_RESISTOR = 2, BIAS_VOLT = 5, GAIN = 1
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_MIC_CTRL, mask);

//	for (int i = 0 ; register_map[i] != SGTL5000_DAP_COEF_WR_A2_LSB ; i++)
//	{
//		uint16_t reg = 0;
//		sgtl5000_i2c_read_register(h_sgtl5000, register_map[i], &reg);
//		printf("%02d: [0x%04x] = 0x%04x\r\n", i, register_map[i], reg);
//	}

	return ret;
}


HAL_StatusTypeDef sgtl5000_initADCDMA(h_sgtl5000_t * h_sgtl5000)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint16_t mask;

	/* Chip Powerup and Supply Configurations */

	//--------------- Power Supply Configuration----------------
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// Configure VDDD level to 1.8V (bits 3:0)
	// Write CHIP_LINREG_CTRL 0x????
	// OK, pas touche!
	// Power up internal linear regulator (Set bit 9)
	// Write CHIP_ANA_POWER 0x7260
	// Pas touche non plus

	// NOTE: This next Write call is needed ONLY if VDDD is
	// externally driven
	// Turn off startup power supplies to save power (Clear bit 12 and 13)
	// Write CHIP_ANA_POWER 0x4260
	mask = (1 << 12) | (1 << 13);
	sgtl5000_i2c_clear_bit(h_sgtl5000, SGTL5000_CHIP_ANA_POWER, mask);

	// NOTE: The next Write calls is needed only if both VDDA and
	// VDDIO power supplies are less than 3.1V.
	// Enable the internal oscillator for the charge pump (Set bit 11)
	// Write CHIP_CLK_TOP_CTRL 0x0800
	// Enable charge pump (Set bit 11)
	// Write CHIP_ANA_POWER 0x4A60
	// VDDA and VDDIO = 3.3V so not necessary

	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1 V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	// Write CHIP_LINREG_CTRL 0x006C
	// VDDA and VDDIO = 3.3V so it IS necessary
	mask = (1 << 5) | (1 << 6);
	sgtl5000_i2c_set_bit(h_sgtl5000, SGTL5000_CHIP_LINREG_CTRL, mask);

	//---- Reference Voltage and Bias Current Configuration----
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8 V. VDDA/2 = 0.9 V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	// Write CHIP_REF_CTRL 0x004E
	mask = 0x01FF;	// VAG_VAL = 1.575V, BIAS_CTRL = -50%, SMALL_POP = 1
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_REF_CTRL, mask);

	// Set LINEOUT reference voltage to VDDIO/2 (1.65 V) (bits 5:0)
	// and bias current (bits 11:8) to the recommended value of 0.36 mA
	// for 10 kOhm load with 1.0 nF capacitance
	// Write CHIP_LINE_OUT_CTRL 0x0322
//	mask = 0x0322;	// LO_VAGCNTRL = 1.65V, OUT_CURRENT = 0.36mA (?)
	mask = 0x031E;
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_LINE_OUT_CTRL, mask);

	//------------Other Analog Block Configurations--------------
	// Configure slow ramp up rate to minimize pop (bit 0)
	// Write CHIP_REF_CTRL 0x004F
	// Déjà fait

	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75 mA
	// Write CHIP_SHORT_CTRL 0x1106
	mask = 0x1106;	// MODE_CM = 2, MODE_LR = 1, LVLADJC = 200mA, LVLADJL = 75mA, LVLADJR = 50mA
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_SHORT_CTRL, mask);

	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	// Write CHIP_ANA_CTRL 0x0133
	mask = 0x0004;	// Unmute all + SELECT_ADC = LINEIN
//	mask = 0x0000;	// Unmute all + SELECT_ADC = MIC
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ANA_CTRL, mask);

	//------------Power up Inputs/Outputs/Digital Blocks---------
	// Power up LINEOUT, HP, ADC, DAC
	// Write CHIP_ANA_POWER 0x6AFF
	mask = 0x6AFF;	// LINEOUT_POWERUP, ADC_POWERUP, CAPLESS_HEADPHONE_POWERUP, DAC_POWERUP, HEADPHONE_POWERUP, REFTOP_POWERUP, ADC_MONO = stereo
	// VAG_POWERUP, VCOAMP_POWERUP = 0, LINREG_D_POWERUP, PLL_POWERUP = 0, VDDC_CHRGPMP_POWERUP, STARTUP_POWERUP = 0, LINREG_SIMPLE_POWERUP,
	// DAC_MONO = stereo
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ANA_POWER, mask);
	// Power up desired digital blocks
	// I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
	// ADC (bit 6) are powered on
	// Write CHIP_DIG_POWER 0x0073
	mask = 0x0073;	// I2S_IN_POWERUP, I2S_OUT_POWERUP, DAP_POWERUP, DAC_POWERUP, ADC_POWERUP
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_DIG_POWER, mask);

	//----------------Set LINEOUT Volume Level-------------------
	// Set the LINEOUT volume level based on voltage reference (VAG)
	// values using this formula
	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9 V and
	// 1.65 V respectively, the // left LO vol (bits 12:8) and right LO
	// volume (bits 4:0) value should be set // to 5
	// Write CHIP_LINE_OUT_VOL 0x0505
	mask = 0x1111;	// TODO recalculer
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_LINE_OUT_VOL, mask);

	/* System MCLK and Sample Clock */

	// Configure SYS_FS clock to 48 kHz
	// Configure MCLK_FREQ to 256*Fs
	// Modify CHIP_CLK_CTRL->SYS_FS 0x0002 // bits 3:2
	// Modify CHIP_CLK_CTRL->MCLK_FREQ 0x0000 // bits 1:0
	mask = 0x0004;	// SYS_FS = 48kHz
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_CLK_CTRL, mask);
	// Configure the I2S clocks in master mode
	// NOTE: I2S LRCLK is same as the system sample clock
	// Modify CHIP_I2S_CTRL->MS 0x0001 // bit 7
	// Non, on reste en slave!
	mask = 0x0130;	// DLEN = 16 bits
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_I2S_CTRL, mask);

	/* PLL Configuration */
	// Pas utilisé

	/* Input/Output Routing */
	// Laissons tout par défaut pour l'instant
	mask = 0x0000;	// ADC -> DAC
 	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_SSS_CTRL, mask);

	/* Le reste */
	mask = 0x0000;	// Unmute
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_ADCDAC_CTRL, mask);

	mask = 0x3C3C;
//	mask = 0x4747;
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_DAC_VOL, mask);

	mask = 0x0251;	// BIAS_RESISTOR = 2, BIAS_VOLT = 5, GAIN = 1
	sgtl5000_WriteReg(h_sgtl5000, SGTL5000_CHIP_MIC_CTRL, mask);

//	for (int i = 0 ; register_map[i] != SGTL5000_DAP_COEF_WR_A2_LSB ; i++)
//	{
//		uint16_t reg = 0;
//		sgtl5000_i2c_read_register(h_sgtl5000, register_map[i], &reg);
//		printf("%02d: [0x%04x] = 0x%04x\r\n", i, register_map[i], reg);
//	}

	return ret;
}





static HAL_StatusTypeDef sgtl5000_i2c_read_register(h_sgtl5000_t *h_sgtl5000, uint16_t reg, uint16_t *value)
{
    HAL_StatusTypeDef status;
    uint8_t reg_addr[2];
    uint8_t data[2];

    reg_addr[0] = (uint8_t)((reg & 0xFF00) >> 8);
    reg_addr[1] = (uint8_t)(reg & 0x00FF);

    // Write register address
    status = HAL_I2C_Master_Transmit(h_sgtl5000->hi2c, h_sgtl5000->device_address, reg_addr, 2, HAL_MAX_DELAY);
    if (status != HAL_OK)
        return status;

    // Read register value
    status = HAL_I2C_Master_Receive(h_sgtl5000->hi2c, h_sgtl5000->device_address, data, 2, HAL_MAX_DELAY);
    if (status != HAL_OK)
        return status;

    *value = (uint16_t)((data[0] << 8) | data[1]);
    return HAL_OK;
}

static HAL_StatusTypeDef sgtl5000_i2c_set_bit(h_sgtl5000_t *h_sgtl5000, uint16_t reg, uint16_t bits_mask)
{
    uint16_t reg_value;
    HAL_StatusTypeDef status;

    // Read current value - Fixed: use read function instead of write
    status = sgtl5000_i2c_read_register(h_sgtl5000, reg, &reg_value);
    if (status != HAL_OK)
        return status;

    // Set bits
    reg_value |= bits_mask;

    // Write back
    return sgtl5000_WriteReg(h_sgtl5000, reg, reg_value);
}

static HAL_StatusTypeDef sgtl5000_i2c_clear_bit(h_sgtl5000_t *h_sgtl5000, uint16_t reg, uint16_t bits_mask)
{
    uint16_t reg_value;
    HAL_StatusTypeDef status;

    // Read current value - Fixed: use read function instead of write
    status = sgtl5000_i2c_read_register(h_sgtl5000, reg, &reg_value);
    if (status != HAL_OK)
        return status;

    // Clear bits
    reg_value &= ~bits_mask;

    // Write back
    return sgtl5000_WriteReg(h_sgtl5000, reg, reg_value);
}

