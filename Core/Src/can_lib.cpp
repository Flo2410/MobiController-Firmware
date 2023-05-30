#include "can_lib.hpp"

#include "can.h"
#include "etl/algorithm.h"
#include "stm32l4xx.h"

CAN_LIB::CAN_LIB(CAN_HandleTypeDef *can) {
  this->can = can;
  HAL_CAN_Start(this->can);
}

HAL_StatusTypeDef CAN_LIB::send_data(const CAN_TxHeaderTypeDef *tx_header, const uint8_t data[]) {
  uint32_t tx_mailbox;
  return HAL_CAN_AddTxMessage(this->can, tx_header, data, &tx_mailbox);
}

HAL_StatusTypeDef CAN_LIB::send_data(uint32_t ide, uint32_t ext_id, uint32_t rtr, uint32_t dlc, const uint8_t data[]) {
  CAN_TxHeaderTypeDef tx_header;
  tx_header.IDE = ide;
  tx_header.ExtId = ext_id;
  tx_header.RTR = rtr;
  tx_header.DLC = dlc;

  return this->send_data(&tx_header, data);
}

HAL_StatusTypeDef CAN_LIB::send_extended(uint32_t ext_id, const uint8_t data[]) {
  return this->send_data(CAN_ID_EXT, ext_id, CAN_RTR_DATA, 8, data);
}

HAL_StatusTypeDef CAN_LIB::send_stop() {
  uint8_t data[8] = {};
  return this->send_extended(CAN_IDS::MOVE_LATERALLY, data);
}

/**
 * @brief Drive the robot
 * @param vx Velocity in x direction in mm/s
 * @param vy Velocity in x direction in mm/s
 * @param vphi Velocity in x direction in mrad/s
 * @retval HAL status
 */
HAL_StatusTypeDef CAN_LIB::drive(int16_t vx, int16_t vy, int16_t vphi) {
  vx = etl::clamp<int16_t>(vx, -3276, 3276);
  vy = etl::clamp<int16_t>(vy, -3276, 3276);
  vphi = etl::clamp<int16_t>(vphi, -3276, 3276);

  int16_t val_x = vx * 10;  // m/s * 10000 -> mm/s * 10
  uint16_t x_twos = twos_complement(val_x);
  uint8_t x_high = x_twos >> 8;
  uint8_t x_low = (uint8_t)x_twos;

  int16_t val_y = vy * 10;  // m/s * 10000 -> mm/s * 10
  uint16_t y_twos = twos_complement(val_y);
  uint8_t y_high = y_twos >> 8;
  uint8_t y_low = (uint8_t)y_twos;

  int16_t val_phi = vphi * 10;  // rad/s * 10000 -> mrad/s * 10
  uint16_t phi_twos = twos_complement(val_phi);
  uint8_t phi_high = phi_twos >> 8;
  uint8_t phi_low = (uint8_t)phi_twos;

  uint8_t data[8] = {
      x_low,
      x_high,
      y_low,
      y_high,
      phi_low,
      phi_high,
      0x00,
      0x00,
  };
  return this->send_extended(CAN_IDS::MOVE_LATERALLY, data);
}

uint16_t CAN_LIB::twos_complement(int16_t num) {
  return -(uint16_t)num;
}
