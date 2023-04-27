#ifndef POZYX_H_
#define POZYX_H_

#include "i2c.h"
#include "stm32l452xx.h"

class Pozyx {
 public:
  Pozyx(I2C_HandleTypeDef* hi2c);

  int reg_read(uint8_t reg_address, uint8_t* pData, int size);

  int get_who_am_i(uint8_t* whoami);
  int get_firmware_version(uint8_t* firmware);

 private:
  ~Pozyx();

  I2C_HandleTypeDef* hi2c;

  int i2c_write_read(uint8_t* write_data, int write_len, uint8_t* read_data, int read_len);
};

#endif