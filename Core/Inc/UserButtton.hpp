#ifndef USER_BUTTON_H_
#define USER_BUTTON_H_

#include "etl/vector.h"
#include "gpio.h"
#include "stm32l452xx.h"

class UserButtton {
 public:
  enum class MODE {
    INTERNAL = 0,
    EXTERNAL = 1,
  };

  inline static etl::vector<UserButtton *, 1> all_buttons;  // MAX = 1

  static void handle_interrupts(uint16_t GPIO_Pin);

  void set_mode(MODE mode);
  MODE get_mode(void);

  UserButtton(GPIO_TypeDef *gpio_port, uint16_t pin);
  ~UserButtton();

 private:
  GPIO_TypeDef *btn_gpio_port;
  uint16_t btn_pin;
  bool state = false;
  MODE mode = MODE::INTERNAL;

  uint32_t last_state_change = 0;

  void handle_interrupt(uint16_t GPIO_Pin);
};

#endif