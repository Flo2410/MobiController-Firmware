#include "bh1750.hpp"

#include "stdio.h"
#include "stm32l4xx.h"

BH1750::BH1750(I2C_HandleTypeDef *hi2c, uint16_t address, MODE mode) : hi2c(hi2c), address(address) {
  HAL_StatusTypeDef status = this->reset();

  if (status != HAL_OK)
    printf("Error initializing BH1750 with error: %d\n", status);

  this->set_mtreg(BH1750::DEFAULT_MTREG);  // Set default value;
  this->set_mode(mode);
}

/**
 * @brief  Reset all registers to default value.
 * @retval HAL status
 */
HAL_StatusTypeDef BH1750::reset(void) {
  uint8_t data = ACTION::RESET;
  return HAL_I2C_Master_Transmit(this->hi2c, this->address, &data, 1, 10);
}

/**
 * @brief Set the power state.
 * @param state Power state which should be set. false -> sleep, low power. true -> running
 * @retval HAL status
 */
HAL_StatusTypeDef BH1750::power_state(bool state) {
  uint8_t data = (state ? 1 : 0);
  return HAL_I2C_Master_Transmit(this->hi2c, this->address, &data, 1, 10);
}

/**
 * @brief Set the mode of converting. Look into BH1750::MODE enum.
 * @retval HAL status
 */
HAL_StatusTypeDef BH1750::set_mode(BH1750::MODE mode) {
  if (!((mode >> 4) || (mode >> 5))) return HAL_ERROR;
  if ((mode & 0x0F) > 3) return HAL_ERROR;

  this->mode = mode;
  return HAL_I2C_Master_Transmit(this->hi2c, this->address, (uint8_t *)&this->mode, 1, 10);
}

/**
 * @brief Set the Measurement Time register. It allows to increase or decrease the sensitivity.
 * @param mtreg The value the MT register should be set to.
 * @retval HAL status
 */
HAL_StatusTypeDef BH1750::set_mtreg(uint8_t mtreg) {
  HAL_StatusTypeDef ret_code;

  if (mtreg < 31 || mtreg > 254)
    return HAL_ERROR;

  this->mtreg = mtreg;

  uint8_t data[2];
  data[0] = (0x40 | (mtreg >> 5));
  data[1] = (0x60 | (mtreg & 0x1F));

  ret_code = HAL_I2C_Master_Transmit(this->hi2c, this->address, &data[0], 1, 10);
  if (HAL_OK != ret_code) return ret_code;

  ret_code = HAL_I2C_Master_Transmit(this->hi2c, this->address, &data[1], 1, 10);
  return ret_code;
}

//
//	Trigger the conversion in manual modes.
//	For low resolution conversion time is typical 16 ms,
//	for high resolution 120 ms. You need to wait until read the measurement value.
//	There is no need to exit low power mode for manual conversion. It makes automatically.
//

// /**
//  * @brief Trigger the conversion in manual mode
// */
// HAL_StatusTypeDef BH1750_TriggerManualConversion(void) {
//   if (BH1750_OK == BH1750_SetMode(Bh1750_Mode)) {
//     return BH1750_OK;
//   }
//   return BH1750_ERROR;
// }

/**
 * @brief Read the converted value and calculate the result.
 * @retval light level
 */
float BH1750::read_light(void) {
  float tmp_result;
  uint8_t data[2];

  if (HAL_OK == HAL_I2C_Master_Receive(this->hi2c, this->address, data, 2, 10)) {
    tmp_result = (data[0] << 8) | (data[1]);

    if (this->mtreg != DEFAULT_MTREG) {
      tmp_result *= (float)((uint8_t)DEFAULT_MTREG / (float)this->mtreg);
    }

    if (this->mode == ONETIME_HIGH_RES_MODE_2 || this->mode == CONTINUOUS_HIGH_RES_MODE_2) {
      tmp_result /= 2.0;
    }

    return tmp_result / (float)CONVERSION_FACTOR;
  }
  return -1;
}