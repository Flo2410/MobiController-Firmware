#include "PowerManager.hpp"

#include "MobiController.hpp"
#include "adc.h"
#include "gpio.h"

PowerManager::PowerManager() {
  // Turn off the power for the led strip and pozyx
  this->set_power_pozyx(false);
  this->set_power_led(false);

  // Calibrate ADC
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
}

PowerManager::~PowerManager() {
}

void PowerManager::set_power_pozyx(bool pwr) {
  HAL_GPIO_WritePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin, static_cast<GPIO_PinState>(!pwr));
}

void PowerManager::set_power_led(bool pwr) {
  HAL_GPIO_WritePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin, static_cast<GPIO_PinState>(!pwr));
}

bool PowerManager::get_power_pozyx() {
  auto pin_state = HAL_GPIO_ReadPin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin);
  return static_cast<bool>(!pin_state);
}

bool PowerManager::get_power_led() {
  auto pin_state = HAL_GPIO_ReadPin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin);
  return static_cast<bool>(!pin_state);
}

void PowerManager::toggle_power_pozyx() {
  HAL_GPIO_TogglePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin);
}

void PowerManager::toggle_power_led() {
  HAL_GPIO_TogglePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin);
}

float PowerManager::get_battery_voltage() {
  HAL_ADC_Start(&hadc1);  // Start ADC
  HAL_ADC_PollForConversion(&hadc1, 1);
  return __LL_ADC_CALC_DATA_TO_VOLTAGE(3310UL, HAL_ADC_GetValue(&hadc1), LL_ADC_RESOLUTION_12B) * (14 / 3.3) * 0.001;
}

bool PowerManager::check_for_battery_warning() {
  float U = this->get_battery_voltage();

  // Check if a battery is connected
  if (U == 0) {
    MobiController::debug_print("There is no battery connected, ignoring low battery voltage\n");
    return false;
  }

  // Check if the battery voltage is bellow 11 V
  if (U <= 11.1) {
    this->battery_warning_triggerd = true;
    return true;
  }
  return false;
}

bool PowerManager::is_battery_warning_triggered() {
  return this->battery_warning_triggerd;
}