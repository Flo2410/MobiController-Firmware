#include "hcsr04.hpp"

#include "etl/string.h"
#include "stm32l4xx.h"
#include "tim.h"
#include "usbd_cdc_if.h"

HCSR04::HCSR04(TIM_HandleTypeDef *htim, TIM_Channel_Struct channel, GPIO_TypeDef *gpio_trig_port, uint16_t gpio_trig_pin) {
  this->htim = htim;
  this->channel = channel;
  this->gpio_trig_port = gpio_trig_port;
  this->gpio_trig_pin = gpio_trig_pin;
  all_sensors.push_back(this);

  // HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_IC_Start_IT(this->htim, this->channel.tim_channel);
}

HCSR04::~HCSR04() {
  etl::remove(all_sensors.begin(), all_sensors.end(), this);
}

void HCSR04::read() {
  HAL_GPIO_WritePin(this->gpio_trig_port, this->gpio_trig_pin, GPIO_PIN_SET);    // pull the TRIG pin HIGH
  HAL_Delay(0.01);                                                               // wait for 10 us
  HAL_GPIO_WritePin(this->gpio_trig_port, this->gpio_trig_pin, GPIO_PIN_RESET);  // pull the TRIG pin low
  __HAL_TIM_ENABLE_IT(this->htim, this->channel.interrupt_channel);
}

float HCSR04::get_distance() {
  return this->difference * 0.034 / 2;
  // return this->difference / 58;
}

void HCSR04::handle_interrupts(TIM_HandleTypeDef *htim) {
  for (auto it_sensor = HCSR04::all_sensors.begin(); it_sensor != HCSR04::all_sensors.end(); ++it_sensor) {
    (*it_sensor)->handle_interrupt(htim);
  }
}

void HCSR04::handle_period_elapsed_interrupts(TIM_HandleTypeDef *htim) {
  for (auto it_sensor = HCSR04::all_sensors.begin(); it_sensor != HCSR04::all_sensors.end(); ++it_sensor) {
    (*it_sensor)->handle_period_elapsed_interrupt(htim);
  }
}

void HCSR04::handle_interrupt(TIM_HandleTypeDef *htim) {
  if (htim->Channel == this->channel.active_channel && this->htim == htim) {     // if the interrupt source is the correct channel
    if (!this->is_first_captured) {                                              // if the first value is not captured
      this->val_1 = HAL_TIM_ReadCapturedValue(htim, this->channel.tim_channel);  // read the first value
      this->is_first_captured = true;                                            // set the first captured as true
      this->overflow_count = 0;                                                  // reset overflow count
      // Now change the polarity to falling edge
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, this->channel.tim_channel, TIM_INPUTCHANNELPOLARITY_FALLING);
    }

    else {                                                                       // if the first is already captured
      this->val_2 = HAL_TIM_ReadCapturedValue(htim, this->channel.tim_channel);  // read second value

      this->val_2 += this->overflow_count * (this->htim->Instance->ARR + 1);  // handle overflow

      this->difference = this->val_2 - this->val_1;

      this->is_first_captured = false;  // set it back to false

      // set polarity to rising edge
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, this->channel.tim_channel, TIM_INPUTCHANNELPOLARITY_RISING);
      __HAL_TIM_DISABLE_IT(htim, this->channel.interrupt_channel);  // Disable the interrupt
    }
  }
}

void HCSR04::handle_period_elapsed_interrupt(TIM_HandleTypeDef *htim) {
  if (htim->Channel != this->channel.active_channel && this->htim != htim) return;
  this->overflow_count++;
}