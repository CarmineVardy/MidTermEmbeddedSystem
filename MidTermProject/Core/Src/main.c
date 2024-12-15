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
#include "button.h"
#include "activity_tracking.h"
#include "led.h"
#include "switch_channel.h"
#include "read_temperature.h"
#include "ECG_module.h"
#include "interface.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//FOR ECG
#define ELAB_ECG_TIME 5 // Tempo di elaborazione ECG in secondi

#define SAMPLING_FREQUENCY_ECG 200 //Frequenza per l'ECG
#define SAMPLING_TIME_ECG 0.005 //Tempo per ECG in s (1.0/SAMPLING_FREQUENCY_ECG)

#define ECG_BUF_LENGHT 4 //Lunghezza buffer per il filtro (SAMPLING_FREQUENCY_ECG/50)

#define ELAB_ECG_BUF_SIZE 1000 //Lunghezza finestra con valori ECG (ELAB_ECG_TIME*SAMPLING_FREQUENCY_ECG)

//FOR TEMP
#define SAMPLING_TIME_TEMP 0.5 //Tempo per temperatura in s

//FOR STEP_COUNTER
#define SAMPLING_TIME_STEP_COUNT 0.1 //Tempo per temperatura in s


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void print_ProgressBar1(uint8_t progress);
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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  init_interface();

  uint16_t data_out;

  uint32_t t1, t2;
  uint32_t time_counter = 0;  // Contatore per il tempo in millisecondi

  int singleConvMode;

  /*Dichiarazione e inizializzazione push button*/
  buttonConfig temp_Button, ECG_Button;

  button_init(&temp_Button, PUSH_BUTTON_1_Pin, GPIOB, GPIO_PIN_SET);
  button_init(&ECG_Button, PUSH_BUTTON_2_Pin, GPIOA, GPIO_PIN_SET);

  GPIO_PinState b_state_temp, b_state_ECG;

  /*Dichiarazione e inizializzazione led */
  ledConfig green_Led, yellow_Led, red_Led;

  led_init(&green_Led, GREEN_LED_Pin, GPIOA, GPIO_PIN_RESET);
  led_init(&yellow_Led, YELLOW_LED_Pin, GPIOB, GPIO_PIN_RESET);
  led_init(&red_Led, RED_LED_Pin, GPIOC, GPIO_PIN_RESET);

  /* Dichiarazione e inizializzazione struttura termistore */
  TempParam tempParam;
  init_TemperatureParams(&tempParam);

  /* Dichiarazione e inizializzazione struttura ECG */
  FilterECGParam filterECG;
  ECGParam ECGparam;
  uint16_t ECG_buf[ECG_BUF_LENGHT], elab_ECG_buf[ELAB_ECG_BUF_SIZE];
<<<<<<< Updated upstream
  UserActivity userActivity;
  StepBuffer stepBuffer;
  StepBuffer_init(&stepBuffer);
  UserActivity_init(&userActivity);
  ECG_init(&filterECG, &ECGparam, ECG_buf, elab_ECG_buf, ECG_BUF_LENGHT, ELAB_ECG_BUF_SIZE);
=======

  ECG_init_Filter(&filterECG, ECG_buf, ECG_BUF_LENGHT);
  ECG_init_ECG(&ECGparam, elab_ECG_buf, ELAB_ECG_BUF_SIZE);
>>>>>>> Stashed changes

  while (1)
  {

	  t1 = HAL_GetTick();

<<<<<<< Updated upstream

  	  //SENSORE DI FORZA
	  switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_1, ADC_SAMPLETIME_3CYCLES, singleConvMode);
	  read_forceSensor(&data_out, &userActivity.right_sensor);
	  switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_3CYCLES, singleConvMode);
	  read_forceSensor(&data_out, &userActivity.left_sensor);
	  step_counter(&userActivity, &stepBuffer );
	  detect_activity(&userActivity,&stepBuffer,&green_Led, &yellow_Led, &red_Led);


	  printf("\rSTEPS: %d  FSR1: %f    FSR2: %f\n", userActivity->steps ,userActivity->right_sensor,userActivity->left_sensor );
	  printf("\rSTATE: %s \n", userState_to_string(userActivity.state));
);

	  // VECCHIO
	  /*float R1,R2;
	  switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_1, ADC_SAMPLETIME_3CYCLES, singleConvMode);
	  read_forceSensor(&data_out, &R1);
	  switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_3CYCLES, singleConvMode);
	  read_forceSensor(&data_out, &R2);
	  printf("\rFORCE1: %f", R1);*/



=======
	  time_counter+=5; // Incrementa il contatore ogni millisecondo
>>>>>>> Stashed changes

	  /* Gestione bottone per temperatura */
	  b_state_temp = read_button(&temp_Button);

	  if (b_state_temp == GPIO_PIN_RESET) {
	      // Campionamento temperatura ogni 1000ms (1 secondo)
	      if (time_counter % (uint32_t) (SAMPLING_TIME_TEMP * 1000) == 0) { // Ogni 1000ms, campiona la temperatura
	          singleConvMode = 0;
	          if (switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_0, ADC_SAMPLETIME_3CYCLES, singleConvMode) != HAL_OK) {
	              printf("\rError with ADC \n");
	          } else {
	              read_Temperature(&tempParam, data_out);
	              if (!tempParam.isStable) {
	            	  //uint8_t progress = (tempParam.stabilityCounter * 100) / STABILITY_SAMPLES;

	            	  //print_ProgressBar1(progress);

	            	  //update_progress_bar(progress);  // Mostra il progresso
	              } else {
	            	  //printf("\rTemp: %.1f \n",tempParam.stableTemperature );
	            	  //tempParam.isStable = 0;
	                  //update_temperature(tempParam.stableTemperature);  // Temperatura stabile
	              }
	          }
	      }
	  } else if (temp_Button.previous_state == GPIO_PIN_RESET && b_state_temp == GPIO_PIN_SET) {
	      reset_TemperatureParams(&tempParam);
	      //clear_progress_bar();
	  }


	  /* Gestione bottone per ECG/HRV */
	  b_state_ECG = read_button(&ECG_Button);

	  if (b_state_ECG == GPIO_PIN_RESET) {
	      // Campionamento ECG ogni 5ms
	      if (time_counter % (uint32_t) (SAMPLING_TIME_ECG * 1000) == 0) { // Ogni 5ms, campiona ECG
	          singleConvMode = 1;
	          if (switch_channel_and_read(&data_out, &hadc1, ADC_CHANNEL_8, ADC_SAMPLETIME_3CYCLES, singleConvMode) != HAL_OK) {
	              printf("\rError with ADC \n");
	          } else {
	              filter_signal(&filterECG, &ECGparam, data_out);
                  uint8_t progress = (ECGparam.count * 100) / ECGparam.length;

                  print_ProgressBar1(progress);
                  //update_progress_bar(progress);  // Mostra il progresso
	              if (ECGparam.count == ECGparam.length) {
	                  if (elab_ECG(&ECGparam, SAMPLING_FREQUENCY_ECG) != HAL_OK) {
	                      printf("\rError with ECG elaboration, Retry.. \n");
	                  } else {
	                	  printf("\rHR: %d, HRV: %f\n",ECGparam.HR, ECGparam.HRV );
	                	  //update_heart_rate_and_variability(ECGparam.HR, ECGparam.HRV);  // Aggiorna HR e HRV
	                  }
	                  ECGparam.count = 0; // Reset dopo l'elaborazione
	              }
	          }
	      }
	  } else if (ECG_Button.previous_state == GPIO_PIN_RESET && b_state_ECG == GPIO_PIN_SET) {
	      reset_ECG(&filterECG, &ECGparam);
	      //clear_progress_bar();
	  }



	  /* Ciclo di delay per la gestione del contapassi */
	  if (time_counter % (uint32_t)(SAMPLING_TIME_STEP_COUNT * 1000) == 0) {
	      // Qui si implementa il contatore dei passi
	  }

	  // Riavvia il contatore a zero ogni secondo, se necessario
	  if (time_counter >= 1000) {
	      time_counter = 0;
	  }

	  t2 = HAL_GetTick();
	  if (t2 - t1 < 5) {
	      // Aspetta il tempo mancante per arrivare a 5 millisecondi
	      HAL_Delay(5 - (t2 - t1));
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

void print_ProgressBar1(uint8_t progress) {
    printf("\r[");
    for (int i = 0; i < 20; i++) {
        printf(i < (progress / 5) ? "#" : "-");
    }
    printf("] %d%%", progress);
    fflush(stdout);
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
