/**
  ******************************************************************************
  * @file    blink.c
  * @author  Dinow
  * @version V0.0.1
  * @date    2015-11-23
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BLINK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void LED_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

__INLINE void LED_On(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

__INLINE void LED_Off(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}


/**
 *
 * @param pvParameters
 */
static void Blink_Task( void *pvParameters )
{
	TickType_t xLastFlashTime;

	LED_Init();

	xLastFlashTime = xTaskGetTickCount();

	while(1)
	{
		LED_On();
		vTaskDelayUntil( &xLastFlashTime, 500 );
		LED_Off();
		vTaskDelayUntil( &xLastFlashTime, 4500 );

	}
}

void Blink_Init(void)
{
	xTaskCreate( Blink_Task, "LED", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL );
}
