/**
  ******************************************************************************
  * @file    lcd1602.c
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "type.h"
#include "util.h"
#include "lcd1602.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SUPPORT_LCD_TEST_COMMAND			0 // debug command for FreeRTOS-CLI

#define LCD_NIBBLE_DELAY					5
#define LCD_INSTRUCTION_DELAY				40
#define LCD_INSTRUCTION_LONG_DELAY			(uint32_t)1600

// LCD Pins
#define LCD_GPIO_CLOCK_PORT					RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC
#define LCD_PIN_DB4_PORT					GPIOB
#define LCD_PIN_DB4_NUM						GPIO_Pin_5
#define LCD_PIN_DB5_PORT					GPIOB
#define LCD_PIN_DB5_NUM						GPIO_Pin_4
#define LCD_PIN_DB6_PORT					GPIOB
#define LCD_PIN_DB6_NUM						GPIO_Pin_10
#define LCD_PIN_DB7_PORT					GPIOA
#define LCD_PIN_DB7_NUM						GPIO_Pin_8
#define LCD_PIN_RS_PORT						GPIOA
#define LCD_PIN_RS_NUM						GPIO_Pin_9
#define LCD_PIN_EN_PORT						GPIOC
#define LCD_PIN_EN_NUM						GPIO_Pin_7
#define LCD_PIN_BL_PORT						GPIOB
#define LCD_PIN_BL_NUM						GPIO_Pin_6
#define LCD_PIN_PW_PORT						GPIOB
#define LCD_PIN_PW_NUM						GPIO_Pin_7

#define LCD_PIN_DB4							LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_DB5							LCD_PIN_DB5_PORT, LCD_PIN_DB5_NUM
#define LCD_PIN_DB6							LCD_PIN_DB6_PORT, LCD_PIN_DB6_NUM
#define LCD_PIN_DB7							LCD_PIN_DB7_PORT, LCD_PIN_DB7_NUM
#define LCD_PIN_RS							LCD_PIN_RS_PORT, LCD_PIN_RS_NUM
#define LCD_PIN_EN							LCD_PIN_EN_PORT, LCD_PIN_EN_NUM
#define LCD_PIN_BL							LCD_PIN_BL_PORT, LCD_PIN_BL_NUM
#define LCD_PIN_PW							LCD_PIN_PW_PORT, LCD_PIN_PW_NUM

// HD44780 Commands
#define HD44780_CLEAR						0x01
#define HD44780_HOME						0x02
#define HD44780_ENTRY_MODE					0x04
#define HD44780_DISPLAY_CONTROL				0x08
#define HD44780_DISPLAY_CURSOR_SHIFT		0x10
#define HD44780_FUNCTION_SET				0x20
#define HD44780_CGRAM_SET					0x40
#define HD44780_DDRAM_SET					0x80

// Bits definition of entry mode
#define HD44780_EM_INCREMENT				0x02
#define HD44780_EM_SHIFT_DISPLAY			0x01

// Bits definition of display on/off control
#define HD44780_DISPLAY_ON					0x04
#define HD44780_CURSOR_ON					0x02
#define HD44780_CURSOR_BLINK				0x01

//Bits definition of cursor/display shift
#define HD44780_SHIFT_DISPLAY				0x08
#define HD44780_SHIFT_RIGHT					0x04

//Bits definition of function set
#define HD44780_8_BIT						0x10
#define HD44780_FONT5x10					0x04
#define HD44780_TWO_LINE					0x08


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static unsigned char m_EntryMode;
static unsigned char m_DisplayCtl;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Send nibble to LCD in 4bit mode
 * @param	nibble to send
 */
static void LCD_WriteNibble(unsigned char nibble)
{
	GPIO_SetBits(LCD_PIN_EN);
	GPIO_WriteBit(LCD_PIN_DB4, (BitAction) (nibble & BIT0));
	GPIO_WriteBit(LCD_PIN_DB5, (BitAction) (nibble & BIT1));
	GPIO_WriteBit(LCD_PIN_DB6, (BitAction) (nibble & BIT2));
	GPIO_WriteBit(LCD_PIN_DB7, (BitAction) (nibble & BIT3));

	udelay(LCD_NIBBLE_DELAY);
	GPIO_ResetBits(LCD_PIN_EN);
}

/**
 * @brief	Write instruction
 * @param	command
 */
static void LCD_WriteInstruction(unsigned char cmd)
{
	GPIO_ResetBits(LCD_PIN_RS);
	LCD_WriteNibble(cmd >> 4);
	LCD_WriteNibble(cmd & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief	Write data
 * @param	data
 */
void LCD_WriteData(unsigned char data)
{
	GPIO_SetBits(LCD_PIN_RS);
	LCD_WriteNibble(data >> 4);
	LCD_WriteNibble(data & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief	Clears display and returns cursor to the home position (address 0).
 */
void LCD_Clear(void)
{
	LCD_WriteInstruction(HD44780_CLEAR);
	udelay(LCD_INSTRUCTION_LONG_DELAY);
}

/**
 * @brief	Returns cursor to home position.
 * 			Also returns display being shifted to the original position. DDRAM content remains unchanged.
 */
void LCD_Home(void)
{
	LCD_WriteInstruction(HD44780_HOME);
	udelay(LCD_INSTRUCTION_LONG_DELAY);
}

/**
 * @brief	Cursor move direction when write data
 * @param	newState
 */
void LCD_LeftToRightMode(FunctionalState newState)
{
	if( newState != DISABLE )
		m_EntryMode |= HD44780_EM_INCREMENT;
	else
		m_EntryMode &= ~HD44780_EM_INCREMENT;

	LCD_WriteInstruction(m_EntryMode);
}

/**
 * @brief	Scroll display when write data
 * @param	newState
 */
void LCD_AutoScroll(FunctionalState newState)
{
	if( newState != DISABLE )
		m_EntryMode |= HD44780_EM_SHIFT_DISPLAY;
	else
		m_EntryMode &= ~HD44780_EM_SHIFT_DISPLAY;

	LCD_WriteInstruction(m_EntryMode);
}

/**
 * @brief	Control display on/off
 * @param	newState
 */
void LCD_Display(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_DISPLAY_ON;
	else
		m_DisplayCtl &= ~HD44780_DISPLAY_ON;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 * @brief	Control cursor on/off
 * @param	newState
 */
void LCD_Cursor(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_CURSOR_ON;
	else
		m_DisplayCtl &= ~HD44780_CURSOR_ON;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 * @brief	Control blink cursor on/off
 * @param	newState
 */
void LCD_Blink(FunctionalState newState)
{
	if( newState != DISABLE )
		m_DisplayCtl |= HD44780_CURSOR_BLINK;
	else
		m_DisplayCtl &= ~HD44780_CURSOR_BLINK;

	LCD_WriteInstruction(m_DisplayCtl);
}

/**
 *
 */
void LCD_ScrollRight(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
}

/**
 *
 */
void LCD_ScrollLeft(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY);
}

/**
 *
 */
void LCD_CursorMoveRight(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_RIGHT);
}

/**
 *
 */
void LCD_CursorMoveLeft(void)
{
	LCD_WriteInstruction(HD44780_DISPLAY_CURSOR_SHIFT);
}

/**
 *
 * @param x
 * @param y
 */
void LCD_SetLoc(uint8_t x, uint8_t y)
{
	LCD_WriteInstruction(HD44780_DDRAM_SET | ((y & 0x01) << 6) | (x & 0x3f));
}

/**
 * @brief	print a string
 * @param 	str : string to print
 */
void LCD_Print(char *str)
{
	while(*str) LCD_WriteData(*str++);
}

/**
 * @brief Configure flow of 4bits mode after power on
 */
static void LCD_SetTo4BitsMode(void)
{
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x02);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief	Initialize flow after power on
 */
void LCD_PowerOnFlow(void)
{
	udelay(40000);

	m_EntryMode = HD44780_ENTRY_MODE;
	m_DisplayCtl = HD44780_DISPLAY_CONTROL;

	LCD_SetTo4BitsMode();
	LCD_WriteInstruction(HD44780_FUNCTION_SET | HD44780_TWO_LINE); // set LCD to font 5x7, 2 line
	LCD_Display(DISABLE);
	LCD_Clear();
	LCD_LeftToRightMode(ENABLE);
	LCD_Display(ENABLE);
}

/**
 * @brief	Control backlight and display on/off
 * @param	newState
 */
void LCD_Sleep(FunctionalState newState)
{
	if( newState != DISABLE )
	{
		LCD_Display(DISABLE);
		GPIO_ResetBits(LCD_PIN_BL);

		GPIO_SetBits(LCD_PIN_PW);

		// Set all gpio to analog mode for power consumption
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;

		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB4_NUM;
		GPIO_Init(LCD_PIN_DB4_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB5_NUM;
		GPIO_Init(LCD_PIN_DB5_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB6_NUM;
		GPIO_Init(LCD_PIN_DB6_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB7_NUM;
		GPIO_Init(LCD_PIN_DB7_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_RS_NUM;
		GPIO_Init(LCD_PIN_RS_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_EN_NUM;
		GPIO_Init(LCD_PIN_EN_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_BL_NUM;
		GPIO_Init(LCD_PIN_BL_PORT, &GPIO_InitStructure);

	}
	else
	{
		// Re-configure all gpio
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_StructInit(&GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB4_NUM;
		GPIO_Init(LCD_PIN_DB4_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB5_NUM;
		GPIO_Init(LCD_PIN_DB5_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB6_NUM;
		GPIO_Init(LCD_PIN_DB6_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB7_NUM;
		GPIO_Init(LCD_PIN_DB7_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_RS_NUM;
		GPIO_Init(LCD_PIN_RS_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_EN_NUM;
		GPIO_Init(LCD_PIN_EN_PORT, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LCD_PIN_BL_NUM;
		GPIO_Init(LCD_PIN_BL_PORT, &GPIO_InitStructure);

		GPIO_ResetBits(LCD_PIN_RS);
		GPIO_ResetBits(LCD_PIN_EN);
		GPIO_SetBits(LCD_PIN_BL);

		GPIO_ResetBits(LCD_PIN_PW);
		GPIO_SetBits(LCD_PIN_BL);
		LCD_PowerOnFlow();
	}
}

/**
 * @brief Configure gpio and initialize LCD
 */
void LCD_Init(void)
{
	RCC_AHB1PeriphClockCmd(LCD_GPIO_CLOCK_PORT, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB4_NUM;
	GPIO_Init(LCD_PIN_DB4_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB5_NUM;
	GPIO_Init(LCD_PIN_DB5_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB6_NUM;
	GPIO_Init(LCD_PIN_DB6_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB7_NUM;
	GPIO_Init(LCD_PIN_DB7_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_RS_NUM;
	GPIO_Init(LCD_PIN_RS_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_EN_NUM;
	GPIO_Init(LCD_PIN_EN_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_BL_NUM;
	GPIO_Init(LCD_PIN_BL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_PW_NUM;
	GPIO_Init(LCD_PIN_PW_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(LCD_PIN_PW);
	GPIO_ResetBits(LCD_PIN_RS);
	GPIO_ResetBits(LCD_PIN_EN);
	GPIO_SetBits(LCD_PIN_BL);

	LCD_PowerOnFlow();

#if SUPPORT_LCD_TEST_COMMAND
	LCD_Test();
#endif

}
