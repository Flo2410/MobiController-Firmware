#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "stm32l4xx.h"

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim);     // Input Capture Interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);  // Timer overflow
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);               // Externial interrupt
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*INTERRUPTS_H_*/