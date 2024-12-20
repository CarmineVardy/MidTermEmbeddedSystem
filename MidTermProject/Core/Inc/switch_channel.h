/*
 * switch_channel.h
 *
 *  Created on: Dec 5, 2024
 *      Author: anton
 */

#ifndef INC_SWITCH_CHANNEL_H_
#define INC_SWITCH_CHANNEL_H_

#include "adc.h"

HAL_StatusTypeDef channel_config(ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time, int singleConvMode);
HAL_StatusTypeDef switch_channel_and_read(uint16_t *d_out, ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time, int singleConvMode);

#endif /* INC_SWITCH_CHANNEL_H_ */
