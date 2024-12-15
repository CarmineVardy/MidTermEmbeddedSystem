#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Per sleep

// Variabili globali per tracciare se i messaggi sono già stati visualizzati
static int temp_message_displayed = 0;
static int hr_message_displayed = 0;

// Funzione per cancellare lo schermo (per Linux/MacOS)
void clear_screen() {
    printf("\033[H\033[J");
}

// Funzione per inizializzare l'interfaccia dello smartwatch
void init_interface() {

	printf("+-------------------------------+\r\n");
    printf("|          SMARTWATCH           |\r\n");
    printf("+-------------------------------+\r\n");
    printf("| Temp: 0.0 \xB0 C                 |\r\n");
    printf("|-------------------------------|\r\n");
    printf("| Heart Rate: 0 bpm             |\r\n");
    printf("| HR Variability: 0 ms          |\r\n");
    printf("+-------------------------------+\r\n");
    printf("| Status: Active                |\r\n");
    printf("+-------------------------------+\r\n");
    printf("| Gruppo 1:                     |\r\n");
    printf("| Carmine Vardaro               |\r\n");
    printf("| Alessia Parente               |\r\n");
    printf("| Antonio D'Arienzo             |\r\n");
    printf("+-------------------------------+\r\n");
}

// Funzione per aggiornare la temperatura
void update_temperature(float new_temperature) {
    printf("\033[A");  // Sposta il cursore di una riga in alto
    printf("\033[K");  // Cancella la riga corrente
    printf("| Temp: %.1f C                    |\n", new_temperature);
}

// Funzione per aggiornare il battito cardiaco e la variabilità
void update_heart_rate_and_variability(int new_heart_rate, int new_hr_variability) {
    printf("\033[A\033[K");  // Sposta il cursore e cancella la riga precedente
    printf("| Heart Rate: %d bpm              |\n", new_heart_rate);
    printf("\033[A\033[K");  // Sposta ancora una riga in alto
    printf("| HR Variability: %d ms           |\n", new_hr_variability);
}

// Funzione che aggiorna la progress bar e mostra il messaggio sopra, solo una volta
void update_progress_bar_with_message(uint8_t progress, const char* message, int* message_displayed) {
    if (*message_displayed == 0) {
        // Mostra il messaggio sopra la progress bar solo se non è stato già visualizzato
        printf("\033[A\033[K");  // Sposta il cursore di una riga in alto e cancella la riga
        printf("+-------------------------------+\n");
        printf("| %s           |\n", message);  // Mostra il messaggio
        printf("+-------------------------------+\n");
        *message_displayed = 1;  // Imposta la variabile per indicare che il messaggio è stato visualizzato
    }

    // Mostra la progress bar
    print_ProgressBar(progress);

    // Se il progresso è al 100%, cancella la progress bar
    if (progress == 100) {
        clear_progress_bar();
    }
}

// Funzione per gestire la progress bar e il messaggio per la temperatura
void update_temperature_with_progress(float new_temperature, uint8_t progress) {
    printf("\033[A");  // Sposta il cursore di una riga in alto
    printf("\033[K");  // Cancella la riga corrente
    update_temperature(new_temperature);  // Aggiorna la temperatura

    // Mostra il messaggio di calcolo della temperatura e la progress bar, se non già visualizzato
    update_progress_bar_with_message(progress, "Sto calcolando la temperatura", &temp_message_displayed);
}

// Funzione per gestire la progress bar e il messaggio per HR e HRV
void update_heart_rate_and_variability_with_progress(int new_heart_rate, int new_hr_variability, uint8_t progress) {
    printf("\033[A\033[K");  // Sposta il cursore e cancella la riga precedente
    update_heart_rate_and_variability(new_heart_rate, new_hr_variability);  // Aggiorna HR e HRV

    // Mostra il messaggio di calcolo dell'HR e HRV e la progress bar, se non già visualizzato
    update_progress_bar_with_message(progress, "Sto calcolando il tuo HR, HRV, non muoverti...", &hr_message_displayed);
}

// Funzione per la progress bar
void print_ProgressBar(uint8_t progress) {
    printf("\r[");
    for (int i = 0; i < 20; i++) {
        printf(i < (progress / 5) ? "#" : "-");
    }
    printf("] %d%%", progress);
    fflush(stdout);
}

// Funzione che aggiorna la progress bar e la rimuove quando non è più necessaria
void update_progress_bar(uint8_t progress) {
    print_ProgressBar(progress);

    // Se il progresso è al 100%, cancella la progress bar
    if (progress == 100) {
        clear_progress_bar();
    }
}

// Funzione per rimuovere la progress bar
void clear_progress_bar() {
    printf("\033[A\033[K");  // Sposta il cursore di una riga sopra e cancella la riga
}
