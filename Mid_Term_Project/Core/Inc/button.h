/*
 * button.h
 *
 *  Created on: Dec 12, 2024
 *      Author: utente
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "gpio.h"

#define COUNT_LIMIT 2  // Valore soglia per il debounce

typedef struct {
    uint16_t GPIO_user_label;    // Etichetta del pulsante
    GPIO_TypeDef* GPIO_Port;     // Porta GPIO a cui è connesso il pulsante
    GPIO_PinState temp_state;    // Stato temporaneo del pulsante
    GPIO_PinState stable_state;  // Stato stabile (debounced)
    GPIO_PinState previous_state;// Stato precedente del pulsante
    uint8_t count;               // Contatore per il debounce
} buttonConfig;

/* Inizializza la configurazione del pulsante */
void button_init(buttonConfig* button, uint16_t user_label, GPIO_TypeDef* port, GPIO_PinState init_state);

/* Legge lo stato stabile del pulsante applicando il debounce */
GPIO_PinState read_button(buttonConfig* button);

#endif /* INC_BUTTON_H_ */
