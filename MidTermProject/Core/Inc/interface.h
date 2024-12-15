/*
 * interface.h
 *
 *  Created on: Dec 10, 2024
 *      Author: utente
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#include <stdint.h>

void clear_screen();
void init_interface();
void update_temperature(float new_temperature);
void update_heart_rate_and_variability(int new_heart_rate, int new_hr_variability);
void update_progress_bar_with_message(uint8_t progress, const char* message, int* message_displayed);
void update_temperature_with_progress(float new_temperature, uint8_t progress);
void update_heart_rate_and_variability_with_progress(int new_heart_rate, int new_hr_variability, uint8_t progress);
void print_ProgressBar(uint8_t progress);
void update_progress_bar(uint8_t progress);
void clear_progress_bar();

#endif /* INC_INTERFACE_H_ */




