#include "PowerManager.hpp"

#include "adc.h"
#include "gpio.h"

PowerManager::PowerManager() {
  // Turn off the power for the led strip and pozyx
  HAL_GPIO_WritePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin, GPIO_PIN_SET);

  // Calibrate ADC
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
}

PowerManager::~PowerManager() {
}

void PowerManager::set_power_pozyx(bool pwr) {
}

void PowerManager::set_power_led(bool pwr) {}

bool PowerManager::get_power_pozyx(void) {}
bool PowerManager::get_power_led(void) {}

float PowerManager::get_battery_voltage(void) {
  HAL_ADC_Start(&hadc1);  // Start ADC
  HAL_ADC_PollForConversion(&hadc1, 1);
  float U = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, HAL_ADC_GetValue(&hadc1), LL_ADC_RESOLUTION_12B) * 0.001;
  return U;
}