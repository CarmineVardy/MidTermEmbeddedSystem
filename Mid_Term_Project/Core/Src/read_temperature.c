/*
 * read_temperature.c
 *
 *  Created on: Dec 14, 2024
 *      Author: utente
 */

#include "read_temperature.h"
#include "interface.h"
#include "math.h"

void init_TemperatureParams(TempParam *tempParam) {

	// Inizializza la temperatura corrente a 0.0 (assumendo che la lettura non sia ancora avvenuta)
    tempParam->currentTemperature = 0.0;

    // La temperatura precedente è anch'essa inizialmente 0.0
    tempParam->previousTemperature = 0.0;

    // La temperatura stabile è inizialmente 0.0
    tempParam->stableTemperature = 0.0;

    // Inizializza lo stato di stabilità a 0 (non stabile)
    tempParam->isStable = 0;

    // Conta il numero di campioni stabiliti per rilevare la stabilità
    tempParam->stabilityCounter = 0;

    // Inizializza la variabile di rilevamento variazioni a 0 (nessuna variazione rilevata)
    tempParam->variationDetected = 0;
}


double calculate_Temperature(uint16_t d_out) {

	// Calcola la tensione in ingresso a partire dal valore ADC (d_out)
    // VREF è la tensione di riferimento e LEVELS è il numero di livelli del convertitore ADC
    double voltage = ((double)d_out) * VREF / LEVELS;

    // Calcola la resistenza in base alla formula del termistore (legge il valore di tensione)
    // RS è il valore di resistenza noto, e VREF è la tensione di riferimento
    double resistance = (-RS * (voltage)) / (voltage - VREF);

    // Calcola la temperatura a partire dalla resistenza usando l'equazione di Steinhart-Hart
    // BETA, R0 e T0 sono parametri specifici del termistore
    // La formula restituisce la temperatura in gradi Celsius, quindi si sottrae 273.15 per ottenere il risultato in °C
    return (BETA) / (log(resistance / R0) + BETA / T0) - 273.15;
}


void read_Temperature(TempParam *tempParam, uint16_t d_out) {

    // Calcola la temperatura attuale utilizzando il valore ADC (d_out)
    // La funzione 'calculate_TemperatureFromVoltage' calcola la temperatura in gradi Celsius
    tempParam->currentTemperature = calculate_Temperature(d_out);

    // Verifica la stabilità della temperatura corrente rispetto a quella precedente
    // La funzione 'check_Stability' gestisce il rilevamento di eventuali variazioni di temperatura
    check_Stability(tempParam);


    // Salva la temperatura corrente come temperatura precedente per il prossimo ciclo di lettura
    // Questo è necessario per il confronto nelle future letture e per la gestione della stabilità
    tempParam->previousTemperature = tempParam->currentTemperature;
}


void check_Stability(TempParam *tempParam) {
    // Calcola la differenza tra la temperatura corrente e quella precedente
    double tempDifference = fabs(tempParam->currentTemperature - tempParam->previousTemperature);

    // Se la differenza supera la soglia di stabilità
    if (tempDifference > STABILITY_THRESHOLD) {
        // Rileva una variazione significativa
        tempParam->variationDetected = 1;
        tempParam->isStable = 0;  // Segna che la temperatura non è stabile

        // Reset del contatore di stabilità perché la temperatura è cambiata
        tempParam->stabilityCounter = 0;

        // Aggiorna l'interfaccia per mostrare che la temperatura non è stabile
        update_temperature_message("calculating temperature..possible variation..");
    } else {
        if (tempParam->variationDetected) {
            // Incrementa il contatore di stabilità
            tempParam->stabilityCounter++;

            uint8_t progress = (tempParam->stabilityCounter * 100) / STABILITY_SAMPLES;
            update_temperature_progress(progress);

            // Verifica se il numero di campioni stabili ha raggiunto la soglia
            if (tempParam->stabilityCounter >= STABILITY_SAMPLES) {
                // La temperatura è considerata stabile
                tempParam->isStable = 1;
                tempParam->variationDetected = 0;

                // Aggiorna il valore stabile solo se è cambiato significativamente
                if (fabs(tempParam->currentTemperature - tempParam->stableTemperature) > STABILITY_THRESHOLD) {
                    tempParam->stableTemperature = tempParam->currentTemperature;

                    // Stampa il valore stabile aggiornato
                    update_temperature(tempParam->stableTemperature);

                }
                // Azzera l'interfaccia per indicare che il valore è stabile
                clear_temp_interface();
            }
        }
    }
}


void reset_TemperatureParams(TempParam *tempParam) {

    // Azzera la temperatura corrente
    tempParam->currentTemperature = 0.0;

    // Azzera la temperatura precedente
    tempParam->previousTemperature = 0.0;

    // Azzera la temperatura stabile
    tempParam->stableTemperature = 0.0;

    // Segna che la temperatura non è stabile (lo stato iniziale)
    tempParam->isStable = 0;

    // Reset del contatore di stabilità
    tempParam->stabilityCounter = 0;

    // Reset del flag che rileva variazioni nella temperatura
    tempParam->variationDetected = 0;
}

