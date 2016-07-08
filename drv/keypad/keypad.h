/**
  ******************************************************************************
  * @file    keypad.h
  * @author  Dinow
  * @version V0.0.1
  * @date    2016-6-14
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */
#ifndef __INC_DRIVERS_KEYPAD_H_
#define __INC_DRIVERS_KEYPAD_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
	Key_None = 0,
	Key_Left,
	Key_Right,
	Key_Up,
	Key_Down,
	Key_Select
} Key_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void KEY_Init(void);
void KEY_Enable(void);
void KEY_Disable(void);
Key_t KEY_GetKey(void);
void KEY_SetIntrMode(FunctionalState newState);

void KEY_Test(void);

#endif /* __INC_DRIVERS_KEYPAD_H_ */
