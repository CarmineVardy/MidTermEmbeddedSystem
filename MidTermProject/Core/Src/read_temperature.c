/*
 * read_temperature.c
 *
 *  Created on: Dec 5, 2024
 *      Author: utente
 */

#include "read_temperature.h"
#include <math.h>
#include <stdio.h>

void init_TemperatureParams(TempParam *tempParam){

	tempParam->currentTemperature = 0.0;
	tempParam->previousTemperature = 0.0;
	tempParam->stableTemperature = 0.0;
	tempParam->isStable = 0;
	tempParam->stabilityCounter = 0;
	tempParam->variationDetected = 0;

}

void read_Temperature(TempParam *tempParam, uint16_t d_out) {
    double voltage, resistance;
    voltage = ((double)d_out) * VREF / LEVELS;
    resistance = (-RS * (voltage)) / (voltage - VREF);

    tempParam->currentTemperature = (BETA) / (log(resistance / R0) + BETA / T0) - 273.15;

    // Verifica della stabilità
    if (fabs(tempParam->currentTemperature - tempParam->previousTemperature) > STABILITY_THRESHOLD) {
        if (!tempParam->variationDetected) {
            tempParam->variationDetected = 1;
            printf("\rCalcolo della temperatura, attendere...\n");
        }

        tempParam->stabilityCounter = 0;
        tempParam->isStable = 0;
    } else {
        if (tempParam->variationDetected) {
            tempParam->stabilityCounter++;

            if (tempParam->stabilityCounter >= STABILITY_SAMPLES) {
                if (fabs(tempParam->currentTemperature - tempParam->stableTemperature) > STABILITY_THRESHOLD) {
                    tempParam->stableTemperature = tempParam->currentTemperature;
                    printf("\rTemperatura stabile: %.1f \xB0 C \n", tempParam->stableTemperature);
                }
                tempParam->variationDetected = 0;
                tempParam->isStable = 1;

                // Non stampare la progress bar se la temperatura è stabile
                return;
            }

            uint8_t progress = (tempParam->stabilityCounter * 100) / STABILITY_SAMPLES;
            print_ProgressBar1(progress);
        }
    }

    tempParam->previousTemperature = tempParam->currentTemperature;
}


void print_ProgressBar1(uint8_t progress) {
    printf("\r[");
    for (int i = 0; i < 20; i++) {
        printf(i < (progress / 5) ? "#" : "-");
    }
    printf("] %d%%", progress);
    fflush(stdout);
}

void reset_TemperatureParams(TempParam *tempParam){

	tempParam->currentTemperature = 0.0;
	tempParam->previousTemperature = 0.0;
	tempParam->stableTemperature = 0.0;
	tempParam->isStable = 0;
	tempParam->stabilityCounter = 0;
	tempParam->variationDetected = 0;

}



