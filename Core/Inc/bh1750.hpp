#ifndef BH1750_H_
#define BH1750_H_

#include "stm32l4xx.h"

class BH1750 {
 public:
  static const uint16_t DEFAULT_ADDRESS = (0x23 << 1);
  static const uint8_t DEFAULT_MTREG = 69;
  static constexpr float CONVERSION_FACTOR = 1.2f;

  enum MODE {
    CONTINUOUS_HIGH_RES_MODE = 0x10,
    CONTINUOUS_HIGH_RES_MODE_2 = 0x11,
    CONTINUOUS_LOW_RES_MODE = 0x13,
    ONETIME_HIGH_RES_MODE = 0x20,
    ONETIME_HIGH_RES_MODE_2 = 0x21,
    ONETIME_LOW_RES_MODE = 0x23
  };

  enum ACTION {
    POWER_DOWN = 0x00,
    POWER_ON = 0x01,
    RESET = 0x07
  };

  BH1750(I2C_HandleTypeDef *hi2c, uint16_t address, MODE mode = CONTINUOUS_HIGH_RES_MODE_2);

  HAL_StatusTypeDef reset(void);
  HAL_StatusTypeDef power_state(bool state);
  HAL_StatusTypeDef set_mtreg(uint8_t mtreg);
  HAL_StatusTypeDef set_mode(MODE mode);
  // HAL_StatusTypeDef trigger_manual_conversion(void);
  float read_light(void);

 private:
  I2C_HandleTypeDef *hi2c;  // Handler to I2C interface
  uint16_t address;
  MODE mode;      // Current sensor mode
  uint8_t mtreg;  // Current MT register value

  ~BH1750();
};

#endif  // BH1750_H_