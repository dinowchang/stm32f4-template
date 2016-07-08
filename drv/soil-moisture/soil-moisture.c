/**
  ******************************************************************************
  * @file    soil-moisture.c
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
#include "stm32f4xx.h"
#include "type.h"
#include "soil-moisture.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SUPPORT_SOIL_TEST_COMMAND			0

#define SOIL_GPIO_CLOCK_PORT				RCC_AHB1Periph_GPIOC
#define SOIL_PIN_VCC_PORT					GPIOC
#define SOIL_PIN_VCC_NUM					GPIO_Pin_6
#define SOIL_PIN_SEN_PORT					GPIOC
#define SOIL_PIN_SEN_NUM					GPIO_Pin_5

#define SOIL_ADC_CLOCK_PORT					RCC_APB2Periph_ADC2
#define SOIL_ADC_PORT						ADC2

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static bool m_SensorEnabled = FALSE;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Enable VCC pin and configure ADC
 */
void SOIL_Open(void)
{
	// Switch VCC pin to 3.3V output mode
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SOIL_PIN_VCC_NUM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SOIL_PIN_VCC_PORT, &GPIO_InitStructure);

	GPIO_SetBits(SOIL_PIN_VCC_PORT, SOIL_PIN_VCC_NUM);

	// configure ADC channel
	RCC_APB2PeriphClockCmd(SOIL_ADC_CLOCK_PORT, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_Init(SOIL_ADC_PORT, &ADC_InitStruct);
	ADC_RegularChannelConfig(SOIL_ADC_PORT, ADC_Channel_15, 1, ADC_SampleTime_84Cycles);

	ADC_Cmd(SOIL_ADC_PORT, ENABLE);

	ADC_SoftwareStartConv(SOIL_ADC_PORT); //Start ADC1 software conversion

	m_SensorEnabled = TRUE;
}

/**
 * @brief	measure moisture of soil
 * @return	ADC value of sensor
 */
uint16_t SOIL_Read(void)
{
	if (!m_SensorEnabled) return 0;

	ADC_ClearFlag(SOIL_ADC_PORT, ADC_FLAG_EOC); //Clear EOC flag
	while(ADC_GetFlagStatus(SOIL_ADC_PORT, ADC_FLAG_EOC) == RESET); //Wail for conversion complete

	return ADC_GetConversionValue(SOIL_ADC_PORT);
}

/**
 * @brief	Disable VCC pin and configure ADC
 */
void SOIL_Close(void)
{
	// Reset VCC output
	GPIO_ResetBits(SOIL_PIN_VCC_PORT, SOIL_PIN_VCC_NUM);

	// Switch VCC pin to analog mode for power saving
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SOIL_PIN_VCC_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(SOIL_PIN_VCC_PORT, &GPIO_InitStructure);

	ADC_Cmd(SOIL_ADC_PORT, DISABLE);
	RCC_APB2PeriphClockCmd(SOIL_ADC_CLOCK_PORT, DISABLE);

	m_SensorEnabled = FALSE;
}

/**
 * @brief	Configure gpio and adc
 */
void SOIL_Init(void)
{
	RCC_AHB1PeriphClockCmd(SOIL_GPIO_CLOCK_PORT, ENABLE);
	RCC_APB2PeriphClockCmd(SOIL_ADC_CLOCK_PORT, ENABLE);

	// Set VCC pin to input mode for power saving
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SOIL_PIN_VCC_NUM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SOIL_PIN_VCC_PORT, &GPIO_InitStructure);

	// Set sensor pin to analog mode
	GPIO_InitStructure.GPIO_Pin = SOIL_PIN_SEN_NUM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SOIL_PIN_SEN_PORT, &GPIO_InitStructure);

	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);

#if SUPPORT_SOIL_TEST_COMMAND
	SOIL_Test();
#endif

}
