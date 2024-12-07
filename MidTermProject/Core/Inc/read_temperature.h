/*
 * read_temperature.h
 *
 *  Created on: Dec 5, 2024
 *      Author: utente
 */

#ifndef INC_READ_TEMPERATURE_H_
#define INC_READ_TEMPERATURE_H_

#include "adc.h"

#define RS 10000
#define BETA 3950
#define T0 298.15
#define R0 10000

void read_temperature(uint16_t *d_out, float *voltage, float *temp);

#endif /* INC_READ_TEMPERATURE_H_ */
