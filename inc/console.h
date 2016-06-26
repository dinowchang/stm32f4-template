/**
  ******************************************************************************
  * @file    console.h
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-26
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

#ifndef __INC_CONSOLE_H_
#define __INC_CONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void CONSOLE_Init(void);
extern QueueHandle_t xConsoleQueue;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INC_CONSOLE_H_ */
