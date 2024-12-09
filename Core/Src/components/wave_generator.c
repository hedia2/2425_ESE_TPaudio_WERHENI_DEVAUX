#include "components/wave_generator.h"
#include "sai.h"

int16_t saiTxBuffer[SAI_BUFFER_SIZE];
uint32_t triangleWave[TRIANGLE_POINTS];

// TODO: Passer le DMA en Half-word si ça marche pas
extern SAI_HandleTypeDef hsai_BlockA;
extern SAI_HandleTypeDef hsai_BlockB;

void Triangle_Wave_Start(void)
{
	for(int i = 0; i < TRIANGLE_POINTS/2; i++)
	    {

	        triangleWave[i] = (i * 32767) / (TRIANGLE_POINTS/2);
	        triangleWave[i + TRIANGLE_POINTS/2] =
	            (32767 - ((i * 32767) / (TRIANGLE_POINTS/2)));
	    }


	    for(int i = 0; i < TRIANGLE_POINTS; i++)
	    {
	        saiTxBuffer[i*2] = triangleWave[i];
	        saiTxBuffer[i*2 + 1] = triangleWave[i];
	    }


	    if(HAL_SAI_Transmit_DMA(&hsai_BlockA, (uint8_t*)saiTxBuffer, SAI_BUFFER_SIZE) != HAL_OK)
	    {
	        Error_Handler();
	    }

}

void Triangle_Wave_Stop(void)
{

    HAL_SAI_DMAStop(&hsai_BlockA);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if(hsai->Instance == SAI1_Block_A)
    {
        HAL_SAI_Transmit_DMA(hsai, (uint8_t*)saiTxBuffer, SAI_BUFFER_SIZE);
    }
}



