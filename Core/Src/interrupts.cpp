#include "interrupts.hpp"

#include "adc.h"
#include "encoder.hpp"
#include "gpio.h"
#include "hcsr04.hpp"
#include "led_strip.hpp"
#include "stdio.h"
#include "stm32l4xx.h"

bool mosfet_power = false;

// Input Capture Interrupt
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
  // Ulraschall sensor interrupt
  HCSR04::handle_interrupts(htim);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  Encoder::handle_interrupts(GPIO_Pin);

  if (GPIO_Pin == USER_BTN_Pin) {
    printf("User BTN pressed!\n");

    mosfet_power = HAL_GPIO_ReadPin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin);

    if (mosfet_power) {
      printf("Mosfet OFF\n");
      HAL_GPIO_WritePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin, GPIO_PIN_RESET);
      // mosfet_power = false;
    } else {
      printf("Mosfet ON\n");
      HAL_GPIO_WritePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin, GPIO_PIN_SET);
      // mosfet_power = true;
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  HCSR04::handle_period_elapsed_interrupts(htim);
  LED_STRIP::handle_timer_interrupt(htim);
}

// ADC
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
//   // Read & Update The ADC Result
//   uint16_t adc_res = HAL_ADC_GetValue(&hadc1);
//   printf("%d\n", adc_res);
//   HAL_ADC_Start_IT(&hadc1);
// }