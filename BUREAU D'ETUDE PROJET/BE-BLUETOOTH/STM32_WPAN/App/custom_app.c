/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
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
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "data.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* BEG14_SERVICE */
  uint8_t               Mycharnotify_Notification_Status;
  uint8_t               Mybutton_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */

  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[512];
uint8_t NotifyCharData[512];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */
char state=0 ;
static uint8_t TimerID;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* BEG14_SERVICE */
static void Custom_Mycharnotify_Update_Char(void);
static void Custom_Mycharnotify_Send_Notification(void);
static void Custom_Mybutton_Update_Char(void);
static void Custom_Mybutton_Send_Notification(void);

/* USER CODE BEGIN PFP */
static void myTimerCallback(void)
{
    // Lance la tâche myTask
    UTIL_SEQ_SetTask(1 << CFG_TASK_MY_TASK, CFG_SCH_PRIO_0);
}

void myTask(void){



	    uint16_t a=get_lux() ;
	    uint16_t b=get_mois();

	       UpdateCharData[0] = a >>8; //MSB
	       UpdateCharData[1] = a &0xFF ; //LSB
	       UpdateCharData[2] = b >> 8; //MSB
	       UpdateCharData[3] = b & 0xFF; //LSB
	       Custom_Mycharnotify_Update_Char();

}
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* BEG14_SERVICE */
    case CUSTOM_STM_MYCHARWRITE_WRITE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYCHARWRITE_WRITE_EVT */

      /* USER CODE END CUSTOM_STM_MYCHARWRITE_WRITE_EVT */
      break;

    case CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_MYBUTTON_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYBUTTON_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYBUTTON_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_MYBUTTON_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYBUTTON_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYBUTTON_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */
	  UTIL_SEQ_RegTask(1 << CFG_TASK_MY_TASK, UTIL_SEQ_RFU, myTask);
	  HW_TS_Create(CFG_TIM_PROC_ID_ISR, &TimerID, hw_ts_Repeated, myTimerCallback);

	  // Démarre le timer (500ms = 500 * 1000 / 32768 ticks)
	  HW_TS_Start(TimerID, 16384); // 500ms en ticks (32768 ticks = 1 seconde)

  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* BEG14_SERVICE */
__USED void Custom_Mycharnotify_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Mycharnotify_UC_1*/
     updateflag=1 ;
  /* USER CODE END Mycharnotify_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_MYCHARNOTIFY, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Mycharnotify_UC_Last*/

  /* USER CODE END Mycharnotify_UC_Last*/
  return;
}

void Custom_Mycharnotify_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Mycharnotify_NS_1*/
  updateflag=1 ;
  /* USER CODE END Mycharnotify_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_MYCHARNOTIFY, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Mycharnotify_NS_Last*/

  /* USER CODE END Mycharnotify_NS_Last*/

  return;
}

__USED void Custom_Mybutton_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Mybutton_UC_1*/
  updateflag=1 ;
  /* USER CODE END Mybutton_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_MYBUTTON, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Mybutton_UC_Last*/

  /* USER CODE END Mybutton_UC_Last*/
  return;
}

void Custom_Mybutton_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Mybutton_NS_1*/
  updateflag=1 ;
  /* USER CODE END Mybutton_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_MYBUTTON, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Mybutton_NS_Last*/

  /* USER CODE END Mybutton_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
