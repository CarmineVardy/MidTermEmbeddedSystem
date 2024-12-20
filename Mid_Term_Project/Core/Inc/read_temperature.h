/*
 * read_temperature.h
 *
 *  Created on: Dec 14, 2024
 *      Author: utente
 */

#ifndef INC_READ_TEMPERATURE_H_
#define INC_READ_TEMPERATURE_H_

#include "adc.h"

// Definizioni dei parametri per il calcolo della temperatura
#define RS 10000           // Resistenza in serie al termistore (valore nominale in ohm)
#define BETA 3950          // Valore BETA del termistore, utilizzato nella formula di Steinhart-Hart
#define T0 298.15          // Temperatura nominale del termistore (25°C in Kelvin)
#define R0 10000           // Resistenza nominale del termistore a T0 (in ohm)

#define STABILITY_SAMPLES 20  // Numero di campioni per determinare la stabilità della temperatura
#define STABILITY_THRESHOLD 1.0  // Soglia di variazione per considerare la temperatura stabile/instabile

typedef struct {
    // Struttura per mantenere i dati relativi alla temperatura e alla stabilità
    float currentTemperature;   // Temperatura corrente letta
    float previousTemperature;  // Temperatura letta precedentemente (utile per il confronto)
    float stableTemperature;    // Temperatura stabile quando il sistema è in stato di equilibrio
    uint8_t isStable;           // Flag che indica se la temperatura è stabile (1 se stabile, 0 se instabile)
    uint8_t stabilityCounter;   // Contatore che tiene traccia dei campioni stabili consecutivi
    uint8_t variationDetected;  // Flag che segnala se è stata rilevata una variazione nella temperatura
} TempParam;


// Funzione per inizializzare i parametri della struttura TempParam
void init_TemperatureParams(TempParam *tempParam);

// Funzione che calcola la temperatura a partire dalla lettura dell'ADC (d_out)
double calculate_Temperature(uint16_t d_out);

// Funzione principale per leggere la temperatura e verificarne la stabilità
void read_Temperature(TempParam *tempParam, uint16_t d_out);

// Funzione per verificare se la temperatura è stabile (basata sulla variazione della temperatura)
void check_Stability(TempParam *tempParam);

// Funzione per azzerare i parametri della struttura TempParam e riportarla allo stato iniziale
void reset_TemperatureParams(TempParam *tempParam);


#endif /* INC_READ_TEMPERATURE_H_ */
