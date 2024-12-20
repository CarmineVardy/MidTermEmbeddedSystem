/*
 * ECG_module.c
 *
 *  Created on: Dec 10, 2024
 *      Author: utente
 */

#include "ECG_module.h"
#include <math.h>
#include <stdio.h>

void ECG_init(FilterECGParam* FilterECGparam, ECGParam* ECGparam, uint16_t* buffer1, uint16_t* buffer2, uint16_t length1, uint16_t length2){

	FilterECGparam->buffer = buffer1;
	FilterECGparam->sum = 0.0;
	FilterECGparam->index = 0;
	FilterECGparam->length = length1;
	FilterECGparam->filled = 0;

	for (uint16_t i = 0; i < length1; i++)
	{
		buffer1[i] = 0;
	}

	ECGparam->buffer = buffer2;
	ECGparam->count = 0;
	ECGparam->length = length2;

}

void filter_signal(FilterECGParam* FilterECGparam, ECGParam* ECGparam, uint16_t d_out)
{
	double voltage;
	voltage = ((double)d_out) * VREF / LEVELS;

	FilterECGparam->sum = FilterECGparam->sum - FilterECGparam->buffer[FilterECGparam->index] + voltage;
	FilterECGparam->buffer[FilterECGparam->index] = voltage;
	FilterECGparam->index = (FilterECGparam->index + 1) % FilterECGparam->length;

	if (FilterECGparam->filled < FilterECGparam->length)
	{
		FilterECGparam->filled++;
	}

	ECGparam->buffer[ECGparam->count] = FilterECGparam->sum / FilterECGparam->filled;
	ECGparam->count++;

	uint8_t progress = (ECGparam->count * 100) / ECGparam->length;
	print_ProgressBar2(progress);

}

HAL_StatusTypeDef elab_ECG(ECGParam* ECGparam, uint16_t samp_time){

	uint16_t peaks[MAX_ARRAY_DIM];
	uint8_t num_peaks = 0;

	if(find_peaks(ECGparam, peaks, &num_peaks) != HAL_OK){
		return HAL_ERROR;
	}

	if (num_peaks < 2) {
		return HAL_ERROR;
	}

	float totalIntervals = 0.0;
	float avgInterval = 0.0;
	uint8_t HR;

	for (int i = 1; i < num_peaks; i++) {
		totalIntervals += (float) (peaks[i] - peaks[i - 1]);
	}

	avgInterval = totalIntervals / (num_peaks - 1);


	HR = round(60.0 / (avgInterval*samp_time));

	printf("\rHR: %d\n", HR);

	return HAL_OK;


}

HAL_StatusTypeDef find_peaks(ECGParam* ECGparam, uint16_t *peaks, uint8_t *num_peaks){

	if (ECGparam->count < 3) {
		*num_peaks = 0;
	    return HAL_ERROR;
	}

    uint16_t max_value = find_max(ECGparam->buffer, ECGparam->count);
    uint16_t min_value = find_min(ECGparam->buffer, ECGparam->count);
    uint16_t th = (uint16_t)(TRESHOLDPEAKS * (max_value-min_value)) + min_value;

    uint8_t count = 0;

    for (uint16_t i = 1; i < ECGparam->count - 1; i++) {
        if ((ECGparam->buffer[i] > th) && (ECGparam->buffer[i] > ECGparam->buffer[i-1]) && (ECGparam->buffer[i] > ECGparam->buffer[i+1])) {

        	peaks[count] = i;
            count++;

            if (count >= MAX_ARRAY_DIM)
            {
            	break;
            }
        }
     }

        *num_peaks = count;
        return HAL_OK;
}

uint16_t find_max(uint16_t *buffer, uint16_t length)
{
	if (length == 0) {
        return 0;
    }

    uint16_t max = buffer[0];
    for (uint16_t i = 1; i < length; i++) {
        if (buffer[i] > max) {
            max = buffer[i];
        }
    }
    return max;
}

uint16_t find_min(uint16_t *buffer, uint16_t length)
{

	if (length == 0) {
        return 0;
    }

    uint16_t min = buffer[0];
    for (uint16_t i = 1; i < length; i++) {
        if (buffer[i] < min) {
            min = buffer[i];
        }
    }
    return min;
}

void reset_ECG(FilterECGParam* FilterECGparam, ECGParam* ECGparam){

	FilterECGparam->sum = 0.0;
	FilterECGparam->index = 0;
	FilterECGparam->filled = 0;

	for (uint16_t i = 0; i < FilterECGparam->length; i++)
	{
		FilterECGparam->buffer[i] = 0;
	}

	ECGparam->count = 0;

	for (uint16_t i = 0; i < ECGparam->length; i++)
	{
		ECGparam->buffer[i] = 0;
	}

}

void print_ProgressBar2(uint8_t progress) {
    printf("\r[");
    for (int i = 0; i < 20; i++) {
        printf(i < (progress / 5) ? "#" : "-");
    }
    printf("] %d%%", progress);
    fflush(stdout);
}







