#include "Bno055.hpp"

#include "stm32l452xx.h"
#include "string.h"  // for memcpy

Bno055::Bno055(I2C_HandleTypeDef *hi2c_device) : hi2c(hi2c_device) {
  this->setup();
  this->set_operation_mode_ndof();
}

void Bno055::write_data(uint8_t reg, uint8_t data) {
  uint8_t txdata[2] = {reg, data};
  uint8_t status;
  status = HAL_I2C_Master_Transmit(this->hi2c, BNO055_I2C_ADDR << 1, txdata, sizeof(txdata), 10);
  if (status == HAL_OK) {
    return;
  }

  if (status == HAL_ERROR) {
    printf("HAL_I2C_Master_Transmit HAL_ERROR\r\n");
  } else if (status == HAL_TIMEOUT) {
    printf("HAL_I2C_Master_Transmit HAL_TIMEOUT\r\n");
  } else if (status == HAL_BUSY) {
    printf("HAL_I2C_Master_Transmit HAL_BUSY\r\n");
  } else {
    printf("Unknown status data %d", status);
  }

  uint32_t error = HAL_I2C_GetError(this->hi2c);
  if (error == HAL_I2C_ERROR_NONE) {
    return;
  } else if (error == HAL_I2C_ERROR_BERR) {
    printf("HAL_I2C_ERROR_BERR\r\n");
  } else if (error == HAL_I2C_ERROR_ARLO) {
    printf("HAL_I2C_ERROR_ARLO\r\n");
  } else if (error == HAL_I2C_ERROR_AF) {
    printf("HAL_I2C_ERROR_AF\r\n");
  } else if (error == HAL_I2C_ERROR_OVR) {
    printf("HAL_I2C_ERROR_OVR\r\n");
  } else if (error == HAL_I2C_ERROR_DMA) {
    printf("HAL_I2C_ERROR_DMA\r\n");
  } else if (error == HAL_I2C_ERROR_TIMEOUT) {
    printf("HAL_I2C_ERROR_TIMEOUT\r\n");
  }

  HAL_I2C_StateTypeDef state = HAL_I2C_GetState(this->hi2c);
  if (state == HAL_I2C_STATE_RESET) {
    printf("HAL_I2C_STATE_RESET\r\n");
  } else if (state == HAL_I2C_STATE_READY) {
    printf("HAL_I2C_STATE_RESET\r\n");
  } else if (state == HAL_I2C_STATE_BUSY) {
    printf("HAL_I2C_STATE_BUSY\r\n");
  } else if (state == HAL_I2C_STATE_BUSY_TX) {
    printf("HAL_I2C_STATE_BUSY_TX\r\n");
  } else if (state == HAL_I2C_STATE_BUSY_RX) {
    printf("HAL_I2C_STATE_BUSY_RX\r\n");
  } else if (state == HAL_I2C_STATE_LISTEN) {
    printf("HAL_I2C_STATE_LISTEN\r\n");
  } else if (state == HAL_I2C_STATE_BUSY_TX_LISTEN) {
    printf("HAL_I2C_STATE_BUSY_TX_LISTEN\r\n");
  } else if (state == HAL_I2C_STATE_BUSY_RX_LISTEN) {
    printf("HAL_I2C_STATE_BUSY_RX_LISTEN\r\n");
  } else if (state == HAL_I2C_STATE_ABORT) {
    printf("HAL_I2C_STATE_ABORT\r\n");
  } else if (state == HAL_I2C_STATE_TIMEOUT) {
    printf("HAL_I2C_STATE_TIMEOUT\r\n");
  } else if (state == HAL_I2C_STATE_ERROR) {
    printf("HAL_I2C_STATE_ERROR\r\n");
  }
}

void Bno055::read_data(uint8_t reg, uint8_t *data, uint8_t len) {
  HAL_I2C_Master_Transmit(this->hi2c, BNO055_I2C_ADDR << 1, &reg, 1, 100);
  HAL_I2C_Master_Receive(this->hi2c, BNO055_I2C_ADDR << 1, data, len, 100);
}

void Bno055::reset() {
  this->write_data(BNO055_SYS_TRIGGER, 0x20);
  HAL_Delay(700);
}

Bno055::OPMODE Bno055::get_operation_mode() {
  uint8_t mode;
  this->read_data(BNO055_OPR_MODE, &mode, 1);
  return static_cast<OPMODE>(mode);
}

void Bno055::set_operation_mode(OPMODE mode) {
  this->write_data(BNO055_OPR_MODE, mode);
  if (mode == OPMODE::OPERATION_MODE_CONFIG) {
    HAL_Delay(19);
  } else {
    HAL_Delay(7);
  }
}

void Bno055::set_operation_mode_config() {
  this->set_operation_mode(OPMODE::OPERATION_MODE_CONFIG);
}

void Bno055::set_operation_mode_ndof() {
  this->set_operation_mode(OPMODE::OPERATION_MODE_NDOF);
}

void Bno055::enable_external_crystal() {
  this->set_external_crystal_use(true);
}

void Bno055::disable_external_crystal() {
  this->set_external_crystal_use(false);
}

void Bno055::setup() {
  this->reset();

  uint8_t id = 0;
  this->read_data(BNO055_CHIP_ID, &id, 1);
  if (id != BNO055_ID) {
    printf("Can't find BNO055, id: 0x%02x. Please check your wiring.\r\n", id);
  }
  this->set_page(0);
  this->write_data(BNO055_SYS_TRIGGER, 0x0);

  // Select BNO055 config mode
  this->set_operation_mode_config();
  HAL_Delay(10);
}

int8_t Bno055::get_temp() {
  this->set_page(0);
  uint8_t t;
  this->read_data(BNO055_TEMP, &t, 1);
  return t;
}

int16_t Bno055::get_sw_revision() {
  this->set_page(0);
  uint8_t buffer[2];
  this->read_data(BNO055_SW_REV_ID_LSB, buffer, 2);
  return (int16_t)((buffer[1] << 8) | buffer[0]);
}

uint8_t Bno055::get_bootloader_revision() {
  this->set_page(0);
  uint8_t tmp;
  this->read_data(BNO055_BL_REV_ID, &tmp, 1);
  return tmp;
}

uint8_t Bno055::get_system_status() {
  this->set_page(0);
  uint8_t tmp;
  this->read_data(BNO055_SYS_STATUS, &tmp, 1);
  return tmp;
}

uint8_t Bno055::get_system_error() {
  this->set_page(0);
  uint8_t tmp;
  this->read_data(BNO055_SYS_ERR, &tmp, 1);
  return tmp;
}

Bno055::self_test_result_t Bno055::get_self_test_result() {
  this->set_page(0);
  uint8_t tmp;
  self_test_result_t res = {
      .mcuState = 0,
      .gyrState = 0,
      .magState = 0,
      .accState = 0};
  this->read_data(BNO055_ST_RESULT, &tmp, 1);
  res.mcuState = (tmp >> 3) & 0x01;
  res.gyrState = (tmp >> 2) & 0x01;
  res.magState = (tmp >> 1) & 0x01;
  res.accState = (tmp >> 0) & 0x01;
  return res;
}

Bno055::calibration_state_t Bno055::get_calibration_state() {
  this->set_page(0);
  calibration_state_t cal = {
      .sys = 0,
      .gyro = 0,
      .mag = 0,
      .accel = 0};
  uint8_t calState = 0;
  this->read_data(BNO055_CALIB_STAT, &calState, 1);
  cal.sys = (calState >> 6) & 0x03;
  cal.gyro = (calState >> 4) & 0x03;
  cal.accel = (calState >> 2) & 0x03;
  cal.mag = calState & 0x03;
  return cal;
}

Bno055::calibration_data_t Bno055::get_calibration_data() {
  calibration_data_t cal_data;
  uint8_t buffer[22];
  OPMODE operation_mode = this->get_operation_mode();
  this->set_operation_mode_config();
  this->set_page(0);

  this->read_data(BNO055_ACC_OFFSET_X_LSB, buffer, 22);

  // Assumes little endian processor
  memcpy(&cal_data.offset.accel, buffer, 6);
  // cal_data.offset.accel.x = ((uint16_t)buffer[0] << 8) | buffer[1];
  // cal_data.offset.accel.y = ((uint16_t)buffer[2] << 8) | buffer[3];
  // cal_data.offset.accel.z = ((uint16_t)buffer[4] << 8) | buffer[5];

  memcpy(&cal_data.offset.mag, buffer + 6, 6);
  // cal_data.offset.mag.x = ((uint16_t)buffer[6] << 8) | buffer[7];
  // cal_data.offset.mag.y = ((uint16_t)buffer[8] << 8) | buffer[9];
  // cal_data.offset.mag.z = ((uint16_t)buffer[10] << 8) | buffer[11];

  memcpy(&cal_data.offset.gyro, buffer + 12, 6);
  // cal_data.offset.gyro.x = ((uint16_t)buffer[12] << 8) | buffer[13];
  // cal_data.offset.gyro.y = ((uint16_t)buffer[14] << 8) | buffer[15];
  // cal_data.offset.gyro.z = ((uint16_t)buffer[16] << 8) | buffer[17];

  memcpy(&cal_data.radius.accel, buffer + 18, 2);
  // cal_data.radius.accel = ((uint16_t)buffer[18] << 8) | buffer[19];

  memcpy(&cal_data.radius.mag, buffer + 20, 2);
  // cal_data.radius.mag = ((uint16_t)buffer[20] << 8) | buffer[21];

  this->set_operation_mode(operation_mode);

  return cal_data;
}

void Bno055::set_calibration_data(calibration_data_t cal_data) {
  uint8_t buffer[22];
  OPMODE operation_mode = this->get_operation_mode();
  this->set_operation_mode_config();
  this->set_page(0);

  // Assumes litle endian processor
  memcpy(buffer, &cal_data.offset.accel, 6);
  memcpy(buffer + 6, &cal_data.offset.mag, 6);
  memcpy(buffer + 12, &cal_data.offset.gyro, 6);
  memcpy(buffer + 18, &cal_data.radius.accel, 2);
  memcpy(buffer + 20, &cal_data.radius.mag, 2);

  for (uint8_t i = 0; i < 22; i++) {
    // TODO(oliv4945): create multibytes write
    this->write_data(BNO055_ACC_OFFSET_X_LSB + i, buffer[i]);
  }

  this->set_operation_mode(operation_mode);
}

Bno055::vector_t Bno055::get_vector_accelerometer() {
  return this->get_vector(VECTOR_TYPE::VECTOR_ACCELEROMETER);
}

Bno055::vector_t Bno055::get_vector_magnetometer() {
  return this->get_vector(VECTOR_TYPE::VECTOR_MAGNETOMETER);
}

Bno055::vector_t Bno055::get_vector_gyroscope() {
  return this->get_vector(VECTOR_TYPE::VECTOR_GYROSCOPE);
}

Bno055::vector_t Bno055::get_vector_euler() {
  return this->get_vector(VECTOR_TYPE::VECTOR_EULER);
}

Bno055::vector_t Bno055::get_vector_linear_accel() {
  return this->get_vector(VECTOR_TYPE::VECTOR_LINEARACCEL);
}

Bno055::vector_t Bno055::get_vector_gravity() {
  return this->get_vector(VECTOR_TYPE::VECTOR_GRAVITY);
}

Bno055::vector_t Bno055::get_vector_quaternion() {
  return this->get_vector(VECTOR_TYPE::VECTOR_QUATERNION);
}

void Bno055::set_axis_map(axis_map_t axis) {
  uint8_t axis_remap = (axis.z << 4) | (axis.y << 2) | (axis.x);
  uint8_t axis_map_sign = (axis.x_sign << 2) | (axis.y_sign << 1) | (axis.z_sign);
  this->write_data(BNO055_AXIS_MAP_CONFIG, axis_remap);
  this->write_data(BNO055_AXIS_MAP_SIGN, axis_map_sign);
}

// Private

void Bno055::set_external_crystal_use(bool state) {
  this->set_page(0);
  uint8_t tmp = 0;
  this->read_data(BNO055_SYS_TRIGGER, &tmp, 1);
  tmp |= (state == true) ? 0x80 : 0x0;
  this->write_data(BNO055_SYS_TRIGGER, tmp);
  HAL_Delay(700);
}

Bno055::vector_t Bno055::get_vector(uint8_t vec) {
  set_page(0);
  uint8_t buffer[8];  // Quaternion need 8 bytes

  if (vec == VECTOR_QUATERNION)
    read_data(vec, buffer, 8);
  else
    read_data(vec, buffer, 6);

  double scale = 1;

  if (vec == VECTOR_MAGNETOMETER) {
    scale = this->mag_scale;
  } else if (vec == VECTOR_ACCELEROMETER ||
             vec == VECTOR_LINEARACCEL || vec == VECTOR_GRAVITY) {
    scale = this->accel_scale;
  } else if (vec == VECTOR_GYROSCOPE) {
    scale = this->angular_rate_scale;
  } else if (vec == VECTOR_EULER) {
    scale = this->euler_scale;
  } else if (vec == VECTOR_QUATERNION) {
    scale = this->qua_scale;
  }

  vector_t xyz = {.w = 0, .x = 0, .y = 0, .z = 0};
  if (vec == VECTOR_QUATERNION) {
    xyz.w = (int16_t)((buffer[1] << 8) | buffer[0]) / scale;
    xyz.x = (int16_t)((buffer[3] << 8) | buffer[2]) / scale;
    xyz.y = (int16_t)((buffer[5] << 8) | buffer[4]) / scale;
    xyz.z = (int16_t)((buffer[7] << 8) | buffer[6]) / scale;
  } else {
    xyz.x = (int16_t)((buffer[1] << 8) | buffer[0]) / scale;
    xyz.y = (int16_t)((buffer[3] << 8) | buffer[2]) / scale;
    xyz.z = (int16_t)((buffer[5] << 8) | buffer[4]) / scale;
  }

  return xyz;
}