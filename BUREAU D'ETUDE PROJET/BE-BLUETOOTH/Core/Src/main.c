/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "ipcc.h"
#include "memorymap.h"
#include "rf.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "lcd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_MOISTURE 2500
#define MAX_LUX 3000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t lux=0 ;
uint16_t moisture=0;
uint16_t raw_data[2];

// Variables pour l'affichage des lcd

uint16_t prev_mois=0, prev_lux = 0 ;
 static uint32_t last = 0;
 static uint32_t last_arrosage=0 ;
 static uint32_t com=0 ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t convCompleted=0 ;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  convCompleted = 1 ;
}


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
  /* Config code for STM32_WPAN (HSE Tuning must be done before system clock configuration) */
  MX_APPE_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* IPCC initialisation */
  MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_RF_Init();
  /* USER CODE BEGIN 2 */
   rgb_lcd lcd;
   lcd_init(&hi2c1,&lcd);
  /* USER CODE END 2 */

  /* Init code for STM32_WPAN */
  MX_APPE_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) raw_data, 2);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  uint8_t aff_mois, aff_lux ;

  while (1)
  {

    /* USER CODE END WHILE */
    MX_APPE_Process();

    /* USER CODE BEGIN 3 */

    if (convCompleted) {

        moisture = raw_data[0];
        lux      = raw_data[1];

        // On affiche que si il y'a un grand changement
        // Eviter que le ADC affiche tout le temps

       if ((moisture-prev_mois)>10 || (prev_mois - moisture )>10 )
            {

        	prev_mois=moisture ;
            }

       if((lux-prev_lux)>200 || (prev_lux - lux)>200) {

        	 prev_lux=lux ;
       }

       // Sortie PWM pour la commande
       // Sortie sur PA8
       // on fix l'interval d'arrossage à 1000 ticks

       if (get_command()){
    	   __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 200); //20% de la tension
       }
       else{
        if ( HAL_GetTick()-last_arrosage > 1000 ){
            //Lorsque le sol est très sec
        	// On alimente à 40% de la tension
        	if ( moisture >= 0 && moisture < 300 )
        	    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 400); // 40% de la tension
        	// Lorsque le sol est un peu humide
        	//On alimente à 20% de la tension
        	if ( moisture >= 300 && moisture <700)
        		  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 200);
        	//Lorque le sol est humide
        	// On cesse d'alimenter le moteur
        	if( moisture >=700)
        		  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        	last_arrosage=HAL_GetTick();

        }


       }



      // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 250);

       // Command depuis le bluetooth



        set_lux(lux ) ;
        set_mois(moisture) ;

        convCompleted = 0;

       // Affichage après 1000 ticks

        if (HAL_GetTick() - last > 1000) {
        char buffer[17];
        char buffer_l[17];
        lcd_position(&hi2c1, 0, 0);
        aff_mois =(prev_mois*100)/MAX_MOISTURE;
        aff_lux=  (prev_lux*100)/MAX_LUX ;

              snprintf(buffer, sizeof(buffer), "SM: %d", aff_mois);

               lcd_print(&hi2c1, buffer);
               lcd_print(&hi2c1, "% ");
               snprintf(buffer, sizeof(buffer), "LUX: %d",aff_lux);
               lcd_print(&hi2c1, buffer);
               lcd_print(&hi2c1, "%");

        last=HAL_GetTick();

        /* snprintf(buffer, sizeof(buffer), "MOISTURE : %d ", aff_mois);
               //HAL_UART_Transmit(&huart1,buffer , 27, 10);
               lcd_print(&hi2c1, buffer);
               lcd_print(&hi2c1, "%");
               lcd_position(&hi2c1, 0, 1);
               snprintf(buffer_l, sizeof(buffer_l), "LUMIERE: %d ",aff_lux);
               lcd_print(&hi2c1, buffer_l);
               lcd_print(&hi2c1, "%");*/

        }

    }

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

  /** Macro to configure the PLL multiplication factor
  */
  __HAL_RCC_PLL_PLLM_CONFIG(RCC_PLLM_DIV2);

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_MSI);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMHIGH);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS|RCC_PERIPHCLK_RFWAKEUP;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_LSE;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE0;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/* USER CODE BEGIN 4 */

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
