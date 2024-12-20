/*
 * led.h
 *
 *  Created on: Dec 12, 2024
 *      Author: utente
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "gpio.h"

// Struttura per configurare il LED
typedef struct {
    uint16_t GPIO_user_label;  // Etichetta del pin GPIO associato al LED
    GPIO_TypeDef* GPIO_Port;   // Porta GPIO su cui è collegato il LED
    GPIO_PinState state;       // Stato corrente del LED (acceso/spento)
} ledConfig;

typedef struct {
    ledConfig green_Led;
    ledConfig yellow_Led;
    ledConfig red_Led;
} LedArray;


// Funzione per inizializzare il LED con un'etichetta, una porta e uno stato iniziale
void led_init (ledConfig* led, uint16_t user_label, GPIO_TypeDef* port, GPIO_PinState init_state);
void turnON_led(ledConfig* led); // Accende il LED specificato
void turnOFF_led(ledConfig* led);// Spegne il LED specificato
void control_led(LedArray* leds, ledConfig* led_to_turn_on); // Controlla e accende il LED specificato dall'array di LED




#endif /* INC_LED_H_ */
