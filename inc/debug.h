/**
  ******************************************************************************
  * @file    debug.h
  * @author  Dinow
  * @version V0.0.1
  * @date    2015-11-15
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

#ifndef __DEBUG_H_
#define __DEBUG_H_

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "type.h"
#include "config.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#ifdef DEBUG
#define DEBUG_printf(CONDITION, args...)	do												\
									{														\
										if(CONDITION)										\
										{													\
											if(xSemaphoreTake(xPrintMutex, portMAX_DELAY))	\
											{												\
												printf( args );								\
												xSemaphoreGive(xPrintMutex);				\
											}												\
										}													\
									}while(0)
#else
#define DEBUG_printf(CONDITION, args...)	((void)0)
#endif

/* Exported functions ------------------------------------------------------- */
void DEBUG_Init(void);
void DEBUG_SendData(uint16_t Data);
void DEBUG_SleepMode(FunctionalState newState);
extern SemaphoreHandle_t xPrintMutex;

#endif /* __DEBUG_H_ */
