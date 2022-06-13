/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H
#define __ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

TIM_HandleTypeDef htim2;

/* Exported functions ------------------------------------------------------- */
void Encoder_GPIO_Init();
void Encoder_Init_Tim2();

#endif