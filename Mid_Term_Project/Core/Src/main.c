/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "interface.h"
#include "button.h"
#include "led.h"
#include "switch_channel.h"
#include "activity_tracking.h"
#include "read_temperature.h"
#include "ECG_module.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Intervalli di campionamento in millisecondi dei 3 task
#define STEP_SAMPLING_INTERVAL 10  // Intervallo per il contapassi (1000 ms)
#define ECG_SAMPLING_INTERVAL 5      // Intervallo per il campionamento ECG (5 ms)
#define TEMP_SAMPLING_INTERVAL 500   // Intervallo per il campionamento temperatura (100 ms)

//FOR ACTIVITY TRACKING
#define STEP_BUFFER_SIZE 5

//FOR TEMP

//FOR ECG
#define SAMPLING_FREQUENCY_ECG 200 //Frequenza per l'ECG
#define SAMPLING_TIME_ECG 0.005 //Tempo per ECG in s (1.0/SAMPLING_FREQUENCY_ECG)

#define ECG_BUF_LENGHT 6 //Lunghezza buffer per il filtro (SAMPLING_FREQUENCY_ECG/50)

#define ELAB_ECG_BUF_SIZE 1000 //Lunghezza finestra con valori ECG (ECG_SAMPLING_INTERVAL*SAMPLING_FREQUENCY_ECG)


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void handle_step_counter(ADC_HandleTypeDef *hadc, UserActivity *userActivity, StepBuffer *stepBuffer, LedArray *leds);
void handle_temperature(ADC_HandleTypeDef *hadc, TempParam *temp);
void handle_temperature_release(TempParam *temp);
void handle_ECG(ADC_HandleTypeDef *hadc,FilterECGParam *filter, ECGParam *ECGparam, uint8_t *message);
void handle_ECG_button_release(FilterECGParam *filter, ECGParam *ECGparam, uint8_t *message);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  //clear_screen();
  //init_interface();

  // Variabili globali per gestire i tempi di campionamento
  uint32_t last_step_time = 0;
  uint32_t last_ecg_time = 0;
  uint32_t last_temp_time = 0;
  uint32_t current_time = 0;

  uint8_t message_ECG = 0;

  /*Dichiarazione e inizializzazione push button (utilizzo di button.h)*/
  buttonConfig temp_Button, ECG_Button; // Dichiarazione delle variabili per configurare i pulsanti
  button_init(&temp_Button, PUSH_BUTTON_1_Pin, GPIOB, GPIO_PIN_SET); // Inizializzazione del pulsante per la temperatura (con etichetta, porta e stato iniziale)
  button_init(&ECG_Button, PUSH_BUTTON_2_Pin, GPIOA, GPIO_PIN_SET);  // Inizializzazione del pulsante per il monitoraggio ECG (con etichetta, porta e stato iniziale)
  GPIO_PinState b_state;  // Dichiarazione delle variabili per memorizzare lo stato attuale dei pulsanti

  /*Dichiarazione e inizializzazione led (utilizzo di led.h)*/
  LedArray leds; // Creazione della struttura che contiene tutti e tre i LED
  led_init(&leds.green_Led, GREEN_LED_Pin, GPIOA, GPIO_PIN_RESET);   // LED verde
  led_init(&leds.yellow_Led, YELLOW_LED_Pin, GPIOB, GPIO_PIN_RESET); // LED giallo
  led_init(&leds.red_Led, RED_LED_Pin, GPIOC, GPIO_PIN_RESET);      // LED rosso

  /*Dichiarazione e inizializzazione acticity tracking (utilizzo di activity_tracking.h)*/
  UserActivity userActivity;
  StepBuffer stepBuffer;
  uint32_t STEP_buf[STEP_BUFFER_SIZE];
  UserActivity_init(&userActivity, &leds); // Inizializzazione della struttura UserActivity4
  StepBuffer_init(&stepBuffer, STEP_buf, STEP_BUFFER_SIZE); // Inizializzazione della struttura StepBuffer

  /* Dichiarazione e inizializzazione struttura termistore */
  TempParam tempParam;
  init_TemperatureParams(&tempParam);

  /* Dichiarazione e inizializzazione struttura ECG */
  FilterECGParam filterECG;
  ECGParam ECGparam;
  uint16_t ECG_buf[ECG_BUF_LENGHT], elab_ECG_buf[ELAB_ECG_BUF_SIZE];

  ECG_init_Filter(&filterECG, ECG_buf, ECG_BUF_LENGHT);
  ECG_init_ECG(&ECGparam, elab_ECG_buf, ELAB_ECG_BUF_SIZE);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  current_time = HAL_GetTick();  // Ottieni il tempo corrente

	  // Gestione del contapassi: solo se è il momento giusto per campionare
	  if ((current_time - last_step_time) >= STEP_SAMPLING_INTERVAL) {
	      handle_step_counter(&hadc1, &userActivity, &stepBuffer, &leds);  // Chiamata alla funzione che gestisce i passi
	      last_step_time = current_time;  // Aggiorna l'ultimo tempo di campionamento
	  }

	  // Gestione del bottone della temperatura
	  b_state = read_button(&temp_Button);  // Leggi lo stato del bottone temperatura

	  if (b_state == GPIO_PIN_RESET) {  // Pulsante temperatura premuto


		  if ((current_time - last_temp_time) >= TEMP_SAMPLING_INTERVAL) {

			  handle_temperature(&hadc1, &tempParam);  // Chiamata alla funzione che gestisce la temperatura
			  last_temp_time = current_time;  // Aggiorna l'ultimo tempo di campionamento temperatura
	      }

	  } else if (temp_Button.previous_state == GPIO_PIN_RESET && b_state == GPIO_PIN_SET) {
	      // Il pulsante è stato rilasciato (passato da RESET a SET)
	      handle_temperature_release(&tempParam);  // Chiamata per gestire il rilascio del bottone temperatura
	  }

	  // Gestione del bottone ECG
	  b_state = read_button(&ECG_Button);  // Leggi lo stato del bottone ECG

	  if (b_state == GPIO_PIN_RESET) {  // Pulsante ECG premuto

		  if ((current_time - last_ecg_time) >= ECG_SAMPLING_INTERVAL) {


			  handle_ECG(&hadc1,&filterECG, &ECGparam, &message_ECG);  // Chiamata alla funzione che gestisce l'ECG


			  last_ecg_time = current_time;  // Aggiorna l'ultimo tempo di campionamento ECG
	      }

	  } else if (ECG_Button.previous_state == GPIO_PIN_RESET && b_state == GPIO_PIN_SET) {
	      // Il pulsante ECG è stato rilasciato
		  handle_ECG_button_release(&filterECG, &ECGparam,&message_ECG);  // Funzione per gestire il rilascio del bottone ECG

	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

void handle_step_counter(ADC_HandleTypeDef *hadc, UserActivity *userActivity, StepBuffer *stepBuffer, LedArray *leds) {

	uint16_t data_out_1, data_out_2;

    // Lettura dei sensori di forza
    if (switch_channel_and_read(hadc, ADC_CHANNEL_1, &data_out_1) != HAL_OK ||
        switch_channel_and_read(hadc, ADC_CHANNEL_4, &data_out_2) != HAL_OK) {
    	update_activity_message("Error with ADC");
    } else {
        read_forceSensor(&data_out_1, &userActivity->left_foot);
        read_forceSensor(&data_out_2, &userActivity->right_foot);
        update_step_count(userActivity, HAL_GetTick(), stepBuffer, leds);
    }
}

void handle_temperature(ADC_HandleTypeDef *hadc, TempParam *temp) {

	uint16_t data_out;

    // Lettura dei sensori di forza
    if (switch_channel_and_read(hadc, ADC_CHANNEL_0, &data_out) != HAL_OK) {
    	update_temperature_message("Error with ADC");
    } else {
    	read_Temperature(temp, data_out);
    }
}

void handle_temperature_release(TempParam *temp){

	reset_TemperatureParams(temp);
	clear_temp_interface();
}

void handle_ECG(ADC_HandleTypeDef *hadc, FilterECGParam *filter, ECGParam *ECGparam, uint8_t *message) {

	uint16_t data_out;

    // Aggiorna il messaggio solo una volta
    if (!(*message)) {
        //update_ECG_message("calculating HR..analyzing HRV...hold steady..");
        *message = 1; // Imposta il flag a 1 dopo l'aggiornamento
    }

    // Cambia il canale ADC, leggi il valore e verifica il successo dell'operazione
    if (switch_channel_and_read(hadc, ADC_CHANNEL_8, &data_out) != HAL_OK) {
    	//update_ECG_message("Error with ADC");
        return; // Uscita anticipata in caso di errore
    }

    // Filtra il segnale grezzo utilizzando i parametri forniti
    filter_signal(filter, ECGparam, data_out);

    //printf("\rdata:%d\n", ECGparam->buffer[ECGparam->count-1]);

    // Calcola il progresso della raccolta dei dati e aggiorna l'interfaccia
    uint8_t progress = (ECGparam->count * 100) / ECGparam->length;
    //update_ECG_progress(progress);

    // Verifica se sono stati raccolti tutti i campioni necessari
    if (ECGparam->count >= ECGparam->length) {

        // Elabora il segnale ECG per calcolare HR e HRV
        if (elab_ECG(ECGparam, SAMPLING_FREQUENCY_ECG) != HAL_OK) {
        	update_ECG_message("Error ECG elaboration, Retry..");
        } else {
            // Aggiorna i valori di HR e HRV nell'interfaccia utente
            //update_ECG(ECGparam->HR, ECGparam->HRV);
        	printf("\rBPM:%d\n", ECGparam->HR);
        }
        // Resetta il contatore per consentire una nuova acquisizione
        ECGparam->count = 0;

    }
}


void handle_ECG_button_release(FilterECGParam *filter, ECGParam *ECGparam, uint8_t *message) {
    // Resetta i parametri del filtro e della struttura ECG
    reset_ECG(filter, ECGparam);

    // Pulisce l'interfaccia ECG per indicare che l'elaborazione è terminata o interrotta
    //clear_ECG_interface();

    *message = 0;

}





/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
