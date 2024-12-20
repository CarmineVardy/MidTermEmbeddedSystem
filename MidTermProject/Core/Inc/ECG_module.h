/*
 * ECG_module.h
 *
 *  Created on: Dec 10, 2024
 *      Author: utente
 */

#ifndef INC_ECG_MODULE_H_
#define INC_ECG_MODULE_H_

#include "adc.h"

#define MAX_ARRAY_DIM	100
#define TRESHOLDPEAKS	0.7

typedef struct {
    uint16_t* buffer;
    float sum;
    uint16_t index;
    uint16_t length;
    uint16_t filled;
} FilterECGParam;

typedef struct {
    uint16_t* buffer;
    uint16_t count;
    uint16_t length;
} ECGParam;

void ECG_init(FilterECGParam* FilterECGparam, ECGParam* ECGparam, uint16_t* buffer1, uint16_t* buffer2, uint16_t length1, uint16_t length2);
void filter_signal(FilterECGParam* FilterECGparam, ECGParam* ECGparam, uint16_t d_out);
HAL_StatusTypeDef elab_ECG(ECGParam* ECGparam, uint16_t samp_time);
HAL_StatusTypeDef find_peaks(ECGParam* ECGparam, uint16_t *peaks, uint8_t *num_peaks);
uint16_t find_max(uint16_t *buffer, uint16_t length);
uint16_t find_min(uint16_t *buffer, uint16_t length);
void reset_ECG(FilterECGParam* FilterECGparam, ECGParam* ECGparam);
void print_ProgressBar2(uint8_t progress);




#endif /* INC_ECG_MODULE_H_ */
