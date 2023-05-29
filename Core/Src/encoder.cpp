#include "encoder.hpp"

#include "stdio.h"
#include "stm32l4xx.h"

Encoder::Encoder(GPIO_TypeDef *gpio_a_port, uint16_t gpio_a_pin, GPIO_TypeDef *gpio_b_port, uint16_t gpio_b_pin) {
  this->gpio_a_port = gpio_a_port;
  this->gpio_a_pin = gpio_a_pin;
  this->gpio_b_port = gpio_b_port;
  this->gpio_b_pin = gpio_b_pin;
  all_sensors.push_back(this);
}

void Encoder::handle_interrupts(uint16_t GPIO_Pin) {
  for (auto it_sensor = Encoder::all_sensors.begin(); it_sensor != Encoder::all_sensors.end(); ++it_sensor) {
    (*it_sensor)->handle_interrupt(GPIO_Pin);
  }
}

void Encoder::handle_interrupt(uint16_t GPIO_Pin) {
  // Check if the interrupt was triggered by "my" pin.
  if (GPIO_Pin != this->gpio_a_pin) return;

  // Get the state of the B pin
  GPIO_PinState state_b = HAL_GPIO_ReadPin(this->gpio_b_port, this->gpio_b_pin);

  // Check B pin state and adjust the counter accordingly.
  if (state_b == GPIO_PIN_SET)
    this->counter++;
  else if (state_b == GPIO_PIN_RESET)
    this->counter--;
}

uint16_t Encoder::get_counter(void) {
  return this->counter;
}

Encoder::~Encoder() {
  etl::remove(all_sensors.begin(), all_sensors.end(), this);
}