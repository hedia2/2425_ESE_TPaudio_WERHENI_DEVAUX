/*
 * numeric_bypass.c
 *
 *  Created on: Dec 9, 2024
 *      Author: dov911
 */

#include "components/numeric_bypass.h"

static uint16_t adc_buffer[BUFFER_SIZE];
static uint16_t dac_buffer[BUFFER_SIZE];
static Bypass_HandleTypeDef hbypass = {0};
static uint32_t buffer_index = 0;



HAL_StatusTypeDef Bypass_Init(ADC_HandleTypeDef* hadc, DAC_HandleTypeDef* hdac, uint32_t dac_channel)
{
    if(hadc == NULL || hdac == NULL)
    {
        return HAL_ERROR;
    }

    hbypass.hadc = hadc;
    hbypass.hdac = hdac;
    hbypass.dac_channel = dac_channel;
    hbypass.buffer_size = BUFFER_SIZE;
    hbypass.input_buffer = adc_buffer;
    hbypass.output_buffer = dac_buffer;
    hbypass.is_running = 0;

    memset(adc_buffer, 0, sizeof(adc_buffer));
    memset(dac_buffer, 0, sizeof(dac_buffer));

    return HAL_OK;
}

HAL_StatusTypeDef Bypass_Start(void)
{
    if(hbypass.is_running)
    {
        return HAL_ERROR;
    }

    if(HAL_ADC_Start_DMA(hbypass.hadc, (uint32_t*)hbypass.input_buffer,
                         hbypass.buffer_size) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if(HAL_DAC_Start(hbypass.hdac, hbypass.dac_channel) != HAL_OK)
    {
        HAL_ADC_Stop_DMA(hbypass.hadc);
        return HAL_ERROR;
    }

    hbypass.is_running = 1;
    return HAL_OK;
}

HAL_StatusTypeDef Bypass_Stop(void)
{
    if(!hbypass.is_running)
    {
        return HAL_ERROR;
    }

    HAL_ADC_Stop_DMA(hbypass.hadc);

    HAL_DAC_Stop(hbypass.hdac, hbypass.dac_channel);

    hbypass.is_running = 0;
    return HAL_OK;
}

static void ProcessHalfBuffer(uint32_t offset)
{
    uint32_t half_size = hbypass.buffer_size / 2;

    for(uint32_t i = 0; i < half_size; i++)
    {
        uint32_t index = offset + i;
        hbypass.output_buffer[index] = ProcessSample(hbypass.input_buffer[index]);

        HAL_DAC_SetValue(hbypass.hdac, hbypass.dac_channel,
                        DAC_ALIGN_12B_R, hbypass.output_buffer[index]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc == hbypass.hadc && hbypass.is_running)
    {
        ProcessHalfBuffer(hbypass.buffer_size / 2);
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc == hbypass.hadc && hbypass.is_running)
    {
    	 ProcessHalfBuffer(0);
    }
}

uint8_t Bypass_IsRunning(void)
{
    return hbypass.is_running;
}





