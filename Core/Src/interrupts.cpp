#include "interrupts.hpp"

#include "UserButtton.hpp"
#include "adc.h"
#include "encoder.hpp"
#include "gpio.h"
#include "hcsr04.hpp"
#include "led_strip.hpp"
#include "stdio.h"
#include "stm32l4xx.h"

// Input Capture Interrupt
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
  // Ulraschall sensor interrupt
  HCSR04::handle_interrupts(htim);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  Encoder::handle_interrupts(GPIO_Pin);
  UserButtton::handle_interrupts(GPIO_Pin);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  HCSR04::handle_period_elapsed_interrupts(htim);
  LED_STRIP::handle_timer_interrupt(htim);
}
