/*
 * interface.h
 *
 *  Created on: Dec 12, 2024
 *      Author: utente
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#include <stdint.h>

void init_interface(); //Funzione per l'inizializzazione dell'interfaccia
void clear_screen(); //Funzione per la pulizia dell'interfaccia
void update_steps(int steps);
void update_activity(const char* activity);
void update_activity_message(const char *message);
void update_temperature(float temp);
void update_temperature_progress(uint8_t progress);
void clear_temp_interface();
void update_temperature_message(const char *message);
void update_ECG(uint8_t HR, float HRV);
void update_ECG_progress(uint8_t progress);
void update_ECG_message(const char *message);
void clear_ECG_interface();

#endif /* INC_INTERFACE_H_ */
