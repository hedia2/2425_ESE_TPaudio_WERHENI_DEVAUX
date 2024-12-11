#include "components/wave_generator.h"
#include "components/sgtl5000.h"
#include "sai.h"

#include <string.h>


// Definitions
#define SAMPLE_RATE 48000
#define AUDIO_PER_FRAME 1
#define BITS_PER_SAMPLE 16

int16_t saiTxBuffer[SAI_BUFFER_SIZE];
uint32_t triangleWave[TRIANGLE_POINTS];

extern SAI_HandleTypeDef hsai_BlockA2;
extern SAI_HandleTypeDef hsai_BlockB2;
extern h_sgtl5000_t h_sgtl5000;


void Triangle_Wave_Start(void)
{
	for(int i = 0; i < TRIANGLE_POINTS/2; i++)
	{

		triangleWave[i] = (i * 32767) / (TRIANGLE_POINTS/2);
		triangleWave[i + TRIANGLE_POINTS/2] = (32767 - ((i * 32767) / (TRIANGLE_POINTS/2)));
	}

	for(int i = 0; i < TRIANGLE_POINTS; i++)
	{
		saiTxBuffer[i*2] = (int16_t)triangleWave[i];
		saiTxBuffer[i*2 + 1] = (int16_t)triangleWave[i];
	}

	if(HAL_SAI_Transmit_DMA(&hsai_BlockA2, (uint8_t*)saiTxBuffer, SAI_BUFFER_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}

void Triangle_Wave_Stop(void)
{
	HAL_SAI_DMAStop(&hsai_BlockA2);

	uint16_t mask = 0x000C; // Mute
	sgtl5000_WriteReg(&h_sgtl5000, SGTL5000_CHIP_ADCDAC_CTRL, mask);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
	if(hsai->Instance == SAI2_Block_A)
	{
		HAL_SAI_Transmit_DMA(hsai, (uint8_t*)saiTxBuffer, SAI_BUFFER_SIZE);
	}
}
