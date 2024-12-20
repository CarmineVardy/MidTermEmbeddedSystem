/*
 * read_eeg.h
 *
 *  Created on: Nov 24, 2024
 *      Author: Martino Giaquinto
 */

#ifndef INC_READ_ECG_H_
#define INC_READ_ECG_H_

#include "adc.h"

#define	VREF	3340
#define	LEVELS	4096

#define MAX_ARRAY_DIM	100
#define TRESHOLDPEAKS	0.75


typedef struct {
    uint16_t* buffer;   // Circular buffer
    float sum;       // Buffer sum
    uint16_t index;     // Current index
    uint16_t length;    // Buffer length
    uint16_t filled; 	//counts the buffer filled positions
} FilterParam;

HAL_StatusTypeDef read_ECG(uint16_t *d_out, float *voltage);
uint16_t filter_signal(FilterParam* filter, uint16_t new_value);
HAL_StatusTypeDef filter_init(FilterParam* filter, uint16_t* buffer, uint16_t length);
uint16_t find_max(uint16_t *buffer, uint16_t length);
HAL_StatusTypeDef find_peaks(uint16_t *buffer, uint16_t length, uint16_t *peaks, uint8_t *num_peaks);
HAL_StatusTypeDef elab_ECG(uint16_t *ECG_window, uint16_t samp_freq, uint16_t length, uint8_t *HR);

#endif /* INC_READ_ECG_H_ */
