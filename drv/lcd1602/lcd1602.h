/**
  ******************************************************************************
  * @file    lcd1602.h
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-14
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

#ifndef __INC_DRIVERS_LCD1602_H_
#define __INC_DRIVERS_LCD1602_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void LCD_Init(void);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_LeftToRightMode(FunctionalState newState);
void LCD_AutoScroll(FunctionalState newState);
void LCD_Display(FunctionalState newState);
void LCD_Cursor(FunctionalState newState);
void LCD_Blink(FunctionalState newState);
void LCD_ScrollRight(void);
void LCD_ScrollLeft(void);
void LCD_CursorMoveRight(void);
void LCD_CursorMoveLeft(void);
void LCD_SetLoc(uint8_t x, uint8_t y);
void LCD_WriteData(unsigned char data);
void LCD_Print(char *str);
void LCD_Sleep(FunctionalState newState);

void LCD_Test(void);

#endif /* __INC_DRIVERS_LCD1602_H_ */
