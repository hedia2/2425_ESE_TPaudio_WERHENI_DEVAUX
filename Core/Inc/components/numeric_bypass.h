/*
 * numeric_bypass.h
 *
 *  Created on: Dec 9, 2024
 *      Author: dov911
 */


#ifndef NUMERIC_BYPASS_H
#define NUMERIC_BYPASS_H

#include "main.h"
#include "adc.h"
#include "dac.h"


/* Defines */
#define BUFFER_SIZE 1024  // Taille du buffer circulaire
#define ADC_RESOLUTION 12 // Résolution ADC en bits
#define DAC_RESOLUTION 12 // Résolution DAC en bits

/* Structure pour le bypass */
typedef struct {
    ADC_HandleTypeDef* hadc;
    DAC_HandleTypeDef* hdac;
    uint32_t dac_channel;
    uint32_t buffer_size;
    uint16_t* input_buffer;
    uint16_t* output_buffer;
    uint8_t is_running;
} Bypass_HandleTypeDef;

/* Prototypes des fonctions publiques */
HAL_StatusTypeDef Bypass_Init(ADC_HandleTypeDef* hadc, DAC_HandleTypeDef* hdac, uint32_t dac_channel);
HAL_StatusTypeDef Bypass_Start(void);
HAL_StatusTypeDef Bypass_Stop(void);
HAL_StatusTypeDef Bypass_Process(void);
uint8_t Bypass_IsRunning(void);

#endif /* NUMERIC_BYPASS_H */

