/**
  ******************************************************************************
  * @file    console.c
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "config.h"
#include "type.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "gitversion.h"
#include "console.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define CONSOLE_TASK_PRIORITY					( tskIDLE_PRIORITY + 2UL )
#define CONSOLE_TASK_STACK						( 2048/4 ) 							// 2048 bytes
#define CONSOLE_QUEUE_LENGTH					16
#define MAX_INPUT_SIZE							64

#define DBG_CONSOLE								TRUE

#define CHAR_ASCII_DEL							( 0x7F ) 							// DEL acts as a backspace.
#define CHAR_ASCII_ESC							( 0x1B ) 							// ESCAPE Key

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
QueueHandle_t xConsoleQueue;
static const char * const pcWelcomeMessage = "FreeRTOS command server.\nType Help to view a list of registered commands.\n\n>";
static const char * const pcEndOfOutputMessage = "\n[Press ENTER to execute the previous command again]\n[Press UP ARROW to show the previous command]\n>";

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Displays a table showing the state of each FreeRTOS task
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char * const pcHeader =
			"     State   Priority  Stack    #\r\n************************************************\r\n";
	BaseType_t xSpacePadding;

	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy(pcWriteBuffer, "Task");
	pcWriteBuffer += strlen(pcWriteBuffer);

	/* Minus three for the null terminator and half the number of characters in
	 "Task" so the column lines up with the centre of the heading. */
	configASSERT(configMAX_TASK_NAME_LEN > 3);
	for (xSpacePadding = strlen("Task"); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3); xSpacePadding++)
	{
		/* Add a space to align columns after the task's name. */
		*pcWriteBuffer = ' ';
		pcWriteBuffer++;

		/* Ensure always terminated. */
		*pcWriteBuffer = 0x00;
	}
	strcpy(pcWriteBuffer, pcHeader);
	vTaskList(pcWriteBuffer + strlen(pcHeader));

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

static const CLI_Command_Definition_t xTaskStats =
{
	"task-stats", /* The command string to type. */
	"task-stats:\n    Displays a table showing the state of each FreeRTOS task\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/**
 * @brief Console task
 * @param pvParameters
 */
static void CONSOLE_Task( void *pvParameters )
{
	char data;
	uint8_t inputIndex = 0;
	uint8_t controlModeIndex = 0;
	static char inputString[MAX_INPUT_SIZE], lastInputString[MAX_INPUT_SIZE];
	char *outputString;
	BaseType_t ret;

	outputString = FreeRTOS_CLIGetOutputBuffer();
	DEBUG_printf(DBG_CONSOLE, "\nBuild on %s at %s. ", __DATE__, __TIME__);
	DEBUG_printf(DBG_CONSOLE, "Git Version : %s\n", GIT_VERSION);
	DEBUG_printf(DBG_CONSOLE, "%s", pcWelcomeMessage);
	fflush(stdout);

	while (1)
	{
		xQueueReceive(xConsoleQueue, &data, portMAX_DELAY);

		if (data == '\r')
		{
			DEBUG_printf(DBG_CONSOLE, "\n");

			// See if the command is empty, indicating that the last command is to be executed again.
			if (inputIndex == 0)
			{
				strcpy(inputString, lastInputString);
			}

			do
			{
				// Get the next output string from the command interpreter.
				ret = FreeRTOS_CLIProcessCommand(inputString, outputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

				// Write the generated string to the UART.
				DEBUG_printf(DBG_CONSOLE, "%s\n", outputString);

			} while (ret != pdFALSE);

			strcpy(lastInputString, inputString);
			inputIndex = 0;
			memset(inputString, 0x00, MAX_INPUT_SIZE);

			DEBUG_printf(DBG_CONSOLE, "%s", pcEndOfOutputMessage);
			fflush(stdout);

		}
		else if (data == CHAR_ASCII_ESC)
		{
			controlModeIndex = 1;
		}
		else if (controlModeIndex != 0)
		{
			if( controlModeIndex == 1 )
			{
				if (data == '[')
					controlModeIndex = 2;
				else
					controlModeIndex = 0;
			}
			else if( controlModeIndex == 2 )
			{
				if (data == 'A') // UP ARROR KEY: Show last command
				{
					while(inputIndex > 0)
					{
						inputIndex--;
						inputString[inputIndex] = '\0';
						DEBUG_printf(DBG_CONSOLE, "\b \b");
						fflush(stdout);
					}
					strcpy(inputString, lastInputString);
					DEBUG_printf(DBG_CONSOLE, "%s", inputString);
					inputIndex = strlen(inputString);
					fflush(stdout);
				}
				controlModeIndex = 0;
			}
			else
				controlModeIndex = 0;

		}
		else if ((data == '\b') || (data == CHAR_ASCII_DEL))
		{
			// Backspace was pressed.  Erase the last character in the string - if any
			if (inputIndex > 0)
			{
				inputIndex--;
				inputString[inputIndex] = '\0';
				DEBUG_printf(DBG_CONSOLE, "\b \b");
				fflush(stdout);
			}
		}
		else if ((data >= ' ') && (data <= '~'))
		{
			if (inputIndex < MAX_INPUT_SIZE)
			{
				inputString[inputIndex] = data;
				inputIndex++;
				DEBUG_printf(DBG_CONSOLE, "%c", data);
				fflush(stdout);
			}
		}
		else
		{
			// Ignore other character
		}
	}
}

/**
 * @brief Initialize and create console task
 */
void CONSOLE_Init(void)
{
	FreeRTOS_CLIRegisterCommand( &xTaskStats );

	xConsoleQueue = xQueueCreate(CONSOLE_QUEUE_LENGTH, sizeof(char));

	//assert(xConsoleQueue != NULL);

	// Create that task that handles the console itself.
	xTaskCreate( 	CONSOLE_Task,				/* The task that implements the command console. */
					"CONSOLE",					/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CONSOLE_TASK_STACK,			/* The size of the stack allocated to the task. */
					NULL,						/* The parameter is not used, so NULL is passed. */
					CONSOLE_TASK_PRIORITY,		/* The priority allocated to the task. */
					NULL );						/* A handle is not required, so just pass NULL. */
}
