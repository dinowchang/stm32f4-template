/**
  ******************************************************************************
  * @file    soil-moisture-test.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-16
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "util.h"
#include "soil-moisture.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t SOIL_OpenCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	SOIL_Open();
	sprintf(pcWriteBuffer, "VCC of soil moisture sensor is enabled\n");
	return pdFALSE;
}

static const CLI_Command_Definition_t xSoilOpen =
{
	"soil_open",
	"soil_open:\n    Enable VCC of soil moisture sensor\n",
	SOIL_OpenCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t SOIL_ReadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint16_t data;
	data = SOIL_Read();
	sprintf(pcWriteBuffer, "Soil moisture = %d\n", data);
	return pdFALSE;
}

static const CLI_Command_Definition_t xSoilRead =
{
	"soil_read",
	"soil_read:\n    Read soil moisture sensor\n",
	SOIL_ReadCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t SOIL_CloseCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	SOIL_Close();
	sprintf(pcWriteBuffer, "VCC of soil moisture sensor is disabled\n");
	return pdFALSE;
}

static const CLI_Command_Definition_t xSoilClose =
{
	"soil_close",
	"soil_close:\n    Disable VCC of soil moisture sensor\n",
	SOIL_CloseCommand,
	0
};

void SOIL_Test(void)
{
	FreeRTOS_CLIRegisterCommand(&xSoilOpen);
	FreeRTOS_CLIRegisterCommand(&xSoilRead);
	FreeRTOS_CLIRegisterCommand(&xSoilClose);
}
