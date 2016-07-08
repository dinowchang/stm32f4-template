/**
  ******************************************************************************
  * @file    soil-moisture.h
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

#ifndef __SRC_DRIVERS_SOIL_MOISTURE_SOIL_MOISTURE_H_
#define __SRC_DRIVERS_SOIL_MOISTURE_SOIL_MOISTURE_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void SOIL_Init(void);
void SOIL_Open(void);
uint16_t SOIL_Read(void);
void SOIL_Close(void);

void SOIL_Test(void);

#endif /* __SRC_DRIVERS_SOIL_MOISTURE_SOIL_MOISTURE_H_ */
