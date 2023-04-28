#include "Pozyx.hpp"

#include "Pozyx_definitions.h"
#include "i2c.h"

int ceil(float x) {
  int i = static_cast<int>(x);
  if (i >= x) {
    return i;
  } else {
    return i + 1;
  }
}

Pozyx::Pozyx(I2C_HandleTypeDef* hi2c) : hi2c(hi2c) {
}

/**
 * Reads a number of bytes from the specified pozyx register address using I2C
 */
int Pozyx::reg_read(uint8_t reg_address, uint8_t* pData, int size) {
  // BUFFER_LENGTH is defined in wire.h, it limits the maximum amount of bytes that can be transmitted/received with i2c in one go
  // because of this, we may have to split up the i2c reads in smaller chunks

  if (!IS_REG_READABLE(reg_address))
    return POZYX_FAILURE;

  int n_runs = ceil((float)size / BUFFER_LENGTH);
  int i;
  int status = 1;
  uint8_t reg;

  for (i = 0; i < n_runs; i++) {
    int offset = i * BUFFER_LENGTH;
    reg = reg_address + offset;

    if (i + 1 != n_runs) {
      status &= this->i2c_write_read(&reg, 1, pData + offset, BUFFER_LENGTH);
    } else {
      status &= this->i2c_write_read(&reg, 1, pData + offset, size - offset);
    }
  }

  return status;
}

int Pozyx::get_who_am_i(uint8_t* whoami) {
  return this->reg_read(POZYX_WHO_AM_I, whoami, 1);
}

int Pozyx::get_firmware_version(uint8_t* version) {
  return this->reg_read(POZYX_FIRMWARE_VER, version, 1);
}

int Pozyx::get_harware_version(uint8_t* version) {
  return this->reg_read(POZYX_HARDWARE_VER, version, 1);
}

int Pozyx::get_network_id(uint16_t* id) {
  return this->reg_read(POZYX_NETWORK_ID, (uint8_t*)id, 2);
}

int Pozyx::get_position(vector_t* pos) {
  coordinates_t coords;
  auto status = this->reg_read(POZYX_POS_X, (uint8_t*)&coords, sizeof(coordinates_t));
  pos->w = 0;
  pos->x = static_cast<float>(coords.x);
  pos->y = static_cast<float>(coords.y);
  pos->z = static_cast<float>(coords.z);

  return status;
}

int Pozyx::get_euler(vector_t* euler) {
  // the raw data are three 16bit values
  int16_t raw_data[3] = {0, 0, 0};
  int status = this->reg_read(POZYX_EUL_HEADING, (uint8_t*)&raw_data, 3 * sizeof(int16_t));

  // convert the raw data from to euler angles in degrees
  euler->w = 0;
  euler->z = raw_data[0] / POZYX_EULER_DIV_DEG;
  euler->x = raw_data[1] / POZYX_EULER_DIV_DEG;
  euler->y = raw_data[2] / POZYX_EULER_DIV_DEG;

  return status;
}

int Pozyx::get_quaternions(vector_t* quaternions) {
  // the raw data are four 16bit values
  int16_t raw_data[4] = {0, 0, 0, 0};
  int status = this->reg_read(POZYX_QUAT_W, (uint8_t*)&raw_data, 4 * sizeof(int16_t));

  // convert the raw data from to a quaternion
  quaternions->w = raw_data[0] / POZYX_QUAT_DIV;
  quaternions->x = raw_data[1] / POZYX_QUAT_DIV;
  quaternions->y = raw_data[2] / POZYX_QUAT_DIV;
  quaternions->z = raw_data[3] / POZYX_QUAT_DIV;

  return status;
}

Pozyx::~Pozyx() {
}

/**
 * Call a register function using I2C with given parameters
 */
int Pozyx::i2c_write_read(uint8_t* write_data, int write_len, uint8_t* read_data, int read_len) {
  if (HAL_I2C_Master_Transmit(this->hi2c, POZYX_I2C_ADDRESS << 1, write_data, write_len, HAL_MAX_DELAY) != HAL_OK) {
    return POZYX_FAILURE;
  }

  if (HAL_I2C_Mem_Read(this->hi2c, POZYX_I2C_ADDRESS << 1, static_cast<uint16_t>(*write_data), I2C_MEMADD_SIZE_8BIT, read_data, read_len, HAL_MAX_DELAY) != HAL_OK) {
    return POZYX_FAILURE;
  }

  return POZYX_SUCCESS;
}
