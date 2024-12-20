/*
 * activity_tracking.h
 *
 *  Created on: Dec 12, 2024
 *      Author: utente
 */
#ifndef INC_ACTIVITY_TRACKING_H_
#define INC_ACTIVITY_TRACKING_H_

#include "main.h"
#include <stdint.h>
#include "led.h"

#define STEP_THRESHOLD 100       // Soglia per determinare il contatto col suolo
#define RUNNING_THRESHOLD 500    // Soglia per lo stato "RUNNING"
#define MIN_WALKING_STEPS 3 // Numero minimo di passi consecutivi per confermare lo stato WALKING
#define INACTIVITY_TIMEOUT 2000  // 2 secondi di inattività per confermare lo stato RESTING


typedef enum {
    RESTING,
    WALKING,
    RUNNING
} UserState;

typedef struct {
    float data;                // Dato del sensore
    uint8_t foot_on_ground;    // Stato del piede (1 = a terra, 0 = sollevato)
    uint8_t prev_foot_on_ground; // Stato precedente
} FSR_Sensor;

typedef struct {
    uint32_t steps;                 // Numero totale di passi
    uint32_t last_step_time;        // Tempo dell'ultimo passo rilevato (in ms)
    uint32_t consecutive_steps;     // Passi consecutivi da RESTING
    FSR_Sensor left_foot;           // Sensore del piede sinistro
    FSR_Sensor right_foot;          // Sensore del piede destro
    UserState state;                // Stato dell'utente
} UserActivity;


typedef struct {
	uint32_t* buffer; // Buffer con tempi tra i passi
	uint32_t total_step_time; 	 	// Somma dei tempi tra i passi
	uint16_t index;    // Indice corrente nel buffer
	uint16_t length;   // Lunghezza del buffer
	uint16_t filled;   // Numero di valori effettivamente presenti nel buffer
} StepBuffer;


void init_fsr_sensor(FSR_Sensor *sensor); // Inizializza il sensore FSR impostando i valori di base.
void UserActivity_init(UserActivity *user_activity, LedArray *ledArray); // Inizializza l'attività dell'utente, compreso il sensore FSR e lo stato.
void StepBuffer_init(StepBuffer *buffer, uint32_t* steps, uint16_t length); // Inizializza il buffer per i passi con la lunghezza specificata e il vettore di passi.


void read_forceSensor(uint16_t *d_out, FSR_Sensor *sensor); // Legge i dati dal sensore FSR e aggiorna i parametri del sensore.
void update_step_count(UserActivity *user_activity, uint32_t current_time, StepBuffer *step_buffer, LedArray *ledArray); // Aggiorna il conteggio dei passi e lo stato dell'attività dell'utente.
void determine_activity(UserActivity *user_activity, uint32_t current_time, StepBuffer *step_buffer, LedArray *ledArray); // Determina lo stato dell'attività dell'utente (resto, camminata, corsa) in base al tempo e ai dati dei passi.

void add_step_time(StepBuffer *buffer, uint32_t step_time); // Aggiunge il tempo di un passo al buffer dei passi.
void reset_step_buffer(StepBuffer *buffer); // Resetta il buffer dei passi azzerando il totale e gli indici.



const char* userState_to_string(UserState state); // Converte lo stato dell'utente in una stringa leggibile.


void set_walking_state(UserActivity *user_activity, LedArray *ledArray); // Imposta lo stato dell'utente su "camminata" e accende il LED giallo.
void set_running_state(UserActivity *user_activity, LedArray *ledArray); // Imposta lo stato dell'utente su "corsa" e accende il LED verde.
void set_resting_state(UserActivity *user_activity, LedArray *ledArray); // Imposta lo stato dell'utente su "riposo" e accende il LED rosso.



#endif /* INC_ACTIVITY_TRACKING_H_ */



