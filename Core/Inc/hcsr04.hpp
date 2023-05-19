#ifndef __HCSR04_H
#define __HCSR04_H

#include "etl/vector.h"
#include "stm32l4xx.h"
#include "tim.h"

class HCSR04 {
 public:
  inline static etl::vector<HCSR04 *, 6> all_sensors;  // MAX Sensors = 6
  static void handle_interrupts(TIM_HandleTypeDef *htim);
  static void handle_period_elapsed_interrupts(TIM_HandleTypeDef *htim);

  HCSR04(TIM_HandleTypeDef *htim, TIM_Channel_Struct channel, GPIO_TypeDef *gpio_trig_port, uint16_t gpio_trig_pin);

  void read(void);
  float get_distance(void);

 private:
  TIM_HandleTypeDef *htim;
  TIM_Channel_Struct channel;
  GPIO_TypeDef *gpio_trig_port;
  uint16_t gpio_trig_pin;

  uint32_t val_1 = 0;
  uint32_t val_2 = 0;
  uint8_t overflow_count = 0;
  uint32_t difference = 0;
  bool is_first_captured = false;  // is the first value captured ?

  void handle_interrupt(TIM_HandleTypeDef *htim);
  void handle_period_elapsed_interrupt(TIM_HandleTypeDef *htim);

  ~HCSR04();
};

// https://controllerstech.com/hcsr04-ultrasonic-sensor-and-stm32/

#endif /* __HCSR04_H */
