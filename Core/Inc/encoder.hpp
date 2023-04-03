#ifndef __ENCODER_H
#define __ENCODER_H

#include "etl/vector.h"
#include "stm32l4xx.h"

class Encoder {
 public:
  inline static etl::vector<Encoder *, 4> all_sensors;  // MAX Sensors = 4

  static void handle_interrupts(uint16_t GPIO_Pin);

  uint16_t get_counter(void);

  Encoder(GPIO_TypeDef *gpio_a_port, uint16_t gpio_a_pin, GPIO_TypeDef *gpio_b_port, uint16_t gpio_b_pin);

 private:
  GPIO_TypeDef *gpio_a_port;
  uint16_t gpio_a_pin;
  GPIO_TypeDef *gpio_b_port;
  uint16_t gpio_b_pin;

  uint16_t counter = 0;

  void handle_interrupt(uint16_t GPIO_Pin);

  ~Encoder();
};

#endif /* __ENCODER_H */
