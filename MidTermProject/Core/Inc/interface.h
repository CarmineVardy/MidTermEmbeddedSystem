/*
 * interface.h
 *
 *  Created on: Dec 10, 2024
 *      Author: utente
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_


// Funzione per inizializzare l'interfaccia
void init_interface();

// Funzione per aggiornare la temperatura
void update_temperature(float new_temperature);

// Funzione per aggiornare il battito cardiaco
void update_heart_rate(int new_heart_rate);

// Funzione per aggiornare il numero di passi
void update_steps(int new_step_count);

// Funzione per aggiungere una riga con un calcolo (esempio: operazione tra battito e passi)
void update_calculation(int heart_rate, int step_count);

// Funzione per cambiare lo stato dell'attività (fermo, camminando, correndo)
void update_activity_status(const char *status);

// Funzione per cancellare lo schermo
void clear_screen();

#endif /* INC_INTERFACE_H_ */




