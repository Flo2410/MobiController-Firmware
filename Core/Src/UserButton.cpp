#include "UserButtton.hpp"
#include "etl/algorithm.h"
#include "gpio.h"
#include "stdio.h"
#include "stm32l452xx.h"

void UserButtton::handle_interrupts(uint16_t GPIO_Pin) {
  for (auto it_sensor = UserButtton::all_buttons.begin(); it_sensor != UserButtton::all_buttons.end(); ++it_sensor) {
    (*it_sensor)->handle_interrupt(GPIO_Pin);
  }
}

void UserButtton::set_mode(MODE mode) {
  this->mode = mode;
}

UserButtton::MODE UserButtton::get_mode() {
  return this->mode;
}

UserButtton::UserButtton(GPIO_TypeDef *gpio_port, uint16_t pin) : btn_gpio_port(gpio_port), btn_pin(pin) {
  all_buttons.push_back(this);
}

UserButtton::~UserButtton() {
  etl::remove(all_buttons.begin(), all_buttons.end(), this);
}

void UserButtton::handle_interrupt(uint16_t GPIO_Pin) {
  if (GPIO_Pin != this->btn_pin) return;

  // Debounce
  uint32_t current_ms = HAL_GetTick();
  if (current_ms < (this->last_state_change + 1)) return;
  this->last_state_change = current_ms;

  // Update state
  this->state = HAL_GPIO_ReadPin(this->btn_gpio_port, this->btn_pin);

  if (this->get_mode() == MODE::INTERNAL) {
    if (this->state) {
      printf("Released\n");
    } else {
      printf("Pressed\n");
    }
  }
}
