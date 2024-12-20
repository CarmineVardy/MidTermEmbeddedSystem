/*
 * read_temperature.h
 *
 *  Created on: Dec 5, 2024
 *      Author: utente
 */

#ifndef INC_READ_TEMPERATURE_H_
#define INC_READ_TEMPERATURE_H_

#include "adc.h"

#define RS 10000 // Resistenza in serie al termistore
#define BETA 3950 // Valore BETA del termistore
#define T0 298.15 // Temperatura nominale (25°C in Kelvin)
#define R0 10000 // Resistenza nominale del termistore a T0

#define STABILITY_SAMPLES 20
#define STABILITY_THRESHOLD 1.0 // Valore meno rigido per ADC rumorosi

typedef struct {
	float currentTemperature; // Temperatura attuale
	float previousTemperature; // Temperatura precedente
	float stableTemperature; // Temperatura stabile
	uint8_t isStable; // Flag che indica se il valore è stabile
	uint8_t stabilityCounter;     // Contatore per letture stabili consecutive
	uint8_t variationDetected; // Se rilevata una possibile variazione
} TempParam;

void init_TemperatureParams(TempParam *tempParam);
void read_Temperature(TempParam *tempParam, uint16_t d_out);
void print_ProgressBar(uint8_t progress);
void print_StableTemperature(TempParam *tempParam);
void reset_TemperatureParams(TempParam *tempParam);
void print_ProgressBar1(uint8_t progress);

#endif /* INC_READ_TEMPERATURE_H_ */
