/*
 * ECG_module.c
 *
 *  Created on: Dec 15, 2024
 *      Author: utente
 */

#include "ECG_module.h"
#include <math.h>

void ECG_init_Filter(FilterECGParam* FilterECGparam, uint16_t* buffer, uint16_t length) {

	// Imposta il buffer di input nella struttura FilterECGParam
    FilterECGparam->buffer = buffer;

    // Inizializza i parametri del filtro
    FilterECGparam->sum = 0.0;    // La somma dei valori nel filtro (utilizzato nel calcolo)
    FilterECGparam->index = 0;    // Indice di posizione nel buffer (utilizzato nel filtro circolare)
    FilterECGparam->length = length;    // Lunghezza del buffer
    FilterECGparam->filled = 0;   // Numero di elementi validi nel buffer

    // Inizializza il buffer impostando tutti i valori a 0
    for (uint16_t i = 0; i < length; i++) {
        buffer[i] = 0;
    }
}

void ECG_init_ECG(ECGParam* ECGparam, uint16_t* buffer, uint16_t length) {

	// Imposta il buffer di input nella struttura ECGParam
    ECGparam->buffer = buffer;

    // Inizializza i parametri relativi all'elaborazione dell'ECG
    ECGparam->count = 0;         // Numero di campioni elaborati
    ECGparam->length = length;  // Lunghezza del buffer
    ECGparam->HR = 0;            // Frequenza cardiaca (in battiti per minuto, bpm)
    ECGparam->HRV = 0.0;         // Variabilità della frequenza cardiaca (HRV)
}

void filter_signal(FilterECGParam* FilterECGparam, ECGParam* ECGparam, uint16_t d_out) {

	// Calcolo del valore di tensione in ingresso in base al dato di uscita d_out
    double voltage = ((double)d_out) * VREF / LEVELS;

    // Aggiornamento della somma per la media mobile
    FilterECGparam->sum += voltage - FilterECGparam->buffer[FilterECGparam->index];
    FilterECGparam->buffer[FilterECGparam->index] = voltage;

    // Aggiornamento dell'indice del buffer in modo circolare
    FilterECGparam->index = (FilterECGparam->index + 1) % FilterECGparam->length;

    // Aumento del contatore degli elementi validi nel filtro
    if (FilterECGparam->filled < FilterECGparam->length) {
        FilterECGparam->filled++;
    }

    // Calcolo del valore medio e memorizzazione nel buffer ECG
    ECGparam->buffer[ECGparam->count] = (uint16_t)(FilterECGparam->sum / FilterECGparam->filled);

    // Incremento del contatore dei campioni elaborati
    ECGparam->count++;
}

HAL_StatusTypeDef elab_ECG(ECGParam* ECGparam, uint16_t samp_freq) {
    uint16_t peaks[MAX_ARRAY_DIM];
    uint8_t num_peaks = 0;

    // Trova i picchi nel segnale ECG
    if (find_peaks(ECGparam, peaks, &num_peaks) != HAL_OK) {
        return HAL_ERROR;
    }

    if (num_peaks < 2) {
        return HAL_ERROR;
    }

    float intervals[MAX_ARRAY_DIM - 1];
    uint8_t validIntervals = 0;
    float meanInterval = 0.0;
    float stdDeviation = 0.0;

    // Calcola gli intervalli tra i picchi consecutivi
    for (int i = 1; i < num_peaks; i++) {
        float interval = (float)(peaks[i] - peaks[i - 1]);

        // Aggiungi l'intervallo se non è un outlier
        if (interval > 20 && interval < 500) {  // Soglie di intervallo (modificabili)
            intervals[validIntervals++] = interval;
        }
    }

    // Se non ci sono intervalli validi, ritorna errore
    if (validIntervals < 2) {
        return HAL_ERROR;
    }

    // Calcola la media degli intervalli
    for (int i = 0; i < validIntervals; i++) {
        meanInterval += intervals[i];
    }
    meanInterval /= validIntervals;

    // Calcola la deviazione standard
    for (int i = 0; i < validIntervals; i++) {
        stdDeviation += (intervals[i] - meanInterval) * (intervals[i] - meanInterval);
    }
    stdDeviation = sqrt(stdDeviation / validIntervals);

    // Filtra gli intervalli che sono troppo distanti dalla media
    uint8_t filteredIntervals = 0;
    for (int i = 0; i < validIntervals; i++) {
        if (fabs(intervals[i] - meanInterval) <= 2 * stdDeviation) {  // Soglia a 2 deviazioni standard
            intervals[filteredIntervals++] = intervals[i];
        }
    }

    // Se dopo il filtro non ci sono intervalli validi, ritorna errore
    if (filteredIntervals < 2) {
        return HAL_ERROR;
    }

    // Calcola la media degli intervalli filtrati
    float totalIntervals = 0.0;
    for (int i = 0; i < filteredIntervals; i++) {
        totalIntervals += intervals[i];
    }
    float avgInterval = totalIntervals / filteredIntervals;

    // Calcola la frequenza cardiaca (HR)
    uint8_t HR = round(60.0 / (avgInterval/samp_freq));

    // Calcola la variabilità della frequenza cardiaca (HRV) come deviazione standard degli intervalli RR
    float HRV = sqrt(stdDeviation);

    // Restituisci i valori HR e HRV, che saranno stampati nel main
    ECGparam->HR = HR;
    ECGparam->HRV = HRV;

    return HAL_OK;
}



HAL_StatusTypeDef find_peaks(ECGParam* ECGparam, uint16_t *peaks, uint8_t *num_peaks) {

    if (ECGparam->count < 3) {
        *num_peaks = 0;
        return HAL_ERROR;
    }

    uint16_t max_value = find_max(ECGparam->buffer, ECGparam->count);
    uint16_t min_value = find_min(ECGparam->buffer, ECGparam->count);
    uint16_t threshold = (uint16_t)(TRESHOLDPEAKS * (max_value - min_value)) + min_value;

    uint8_t count = 0;
    uint8_t aboveThresholdState = 0; // 0 = sotto soglia, 1 = sopra soglia

    for (uint16_t i = 1; i < ECGparam->count - 1; i++) {
        int16_t derivative = (int16_t)(ECGparam->buffer[i] - ECGparam->buffer[i - 1]);

        // Controllo fronte di salita e discesa
        if (aboveThresholdState == 0 && derivative > 0 && ECGparam->buffer[i] >= threshold && ECGparam->buffer[i - 1] < threshold) {
            aboveThresholdState = 1;
        } else if (aboveThresholdState == 1 && ECGparam->buffer[i] < threshold) {
            aboveThresholdState = 0;
        }

        // Controllo picco locale
        if (ECGparam->buffer[i] > threshold &&
            ECGparam->buffer[i] > ECGparam->buffer[i - 1] &&
            ECGparam->buffer[i] > ECGparam->buffer[i + 1]) {

            // Assicurarsi che il picco sia valido considerando anche il fronte di salita
            if (aboveThresholdState == 1) {
                peaks[count] = i;
                count++;

                if (count >= MAX_ARRAY_DIM) {
                    break;
                }

                // Prevenire duplicazione di picchi nella stessa area
                aboveThresholdState = 0;
            }
        }
    }

    *num_peaks = count;
    return HAL_OK;
}

uint16_t find_max(uint16_t *buffer, uint16_t length)
{
    // Se la lunghezza dell'array è zero, restituisce 0 (nessun dato)
    if (length == 0) {
        return 0;
    }

    // Inizializza il massimo con il primo valore dell'array
    uint16_t max = buffer[0];

    // Scorre l'array e aggiorna il massimo se un valore maggiore è trovato
    for (uint16_t i = 1; i < length; i++) {
        if (buffer[i] > max) {
            max = buffer[i];
        }
    }

    return max;  // Restituisce il valore massimo trovato
}

uint16_t find_min(uint16_t *buffer, uint16_t length)
{
    // Se la lunghezza dell'array è zero, restituisce 0 (nessun dato)
    if (length == 0) {
        return 0;
    }

    // Inizializza il minimo con il primo valore dell'array
    uint16_t min = buffer[0];

    // Scorre l'array e aggiorna il minimo se un valore minore è trovato
    for (uint16_t i = 1; i < length; i++) {
        if (buffer[i] < min) {
            min = buffer[i];
        }
    }

    return min;  // Restituisce il valore minimo trovato
}

void reset_ECG(FilterECGParam* FilterECGparam, ECGParam* ECGparam)
{
    // Reset dei parametri del filtro ECG
    FilterECGparam->sum = 0.0;           // Azzeramento della somma dei valori filtrati
    FilterECGparam->index = 0;           // Ripristino dell'indice del buffer
    FilterECGparam->filled = 0;          // Azzeramento del conteggio dei valori
    for (uint16_t i = 0; i < FilterECGparam->length; i++) {
        FilterECGparam->buffer[i] = 0;   // Azzeramento di tutti i valori nel buffer
    }

    // Reset dei dati ECG
    ECGparam->count = 0;                 // Azzeramento del conteggio dei campioni ECG
    for (uint16_t i = 0; i < ECGparam->length; i++) {
        ECGparam->buffer[i] = 0;         // Azzeramento di tutti i valori nel buffer ECG
    }
}



