#ifndef CAN_LIB_H_
#define CAN_LIB_H_

#include "can.h"
#include "stm32l4xx.h"

class CAN_LIB {
 public:
  enum CAN_IDS {
    MOVE_LATERALLY = 0x00002A01
  };

  CAN_LIB(CAN_HandleTypeDef *can);

  HAL_StatusTypeDef send_data(const CAN_TxHeaderTypeDef *tx_header, const uint8_t data[]);
  HAL_StatusTypeDef send_data(uint32_t ide, uint32_t ext_id, uint32_t rtr, uint32_t dlc, const uint8_t data[]);
  HAL_StatusTypeDef send_extended(uint32_t ext_id, const uint8_t data[]);
  HAL_StatusTypeDef send_stop();
  HAL_StatusTypeDef drive(int16_t vx, int16_t vy, int16_t vphi);

 private:
  CAN_HandleTypeDef *can;

  static uint16_t twos_complement(int16_t num);

  ~CAN_LIB();
};

#endif
