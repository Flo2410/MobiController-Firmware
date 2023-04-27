#include "MobiController.hpp"

#include "Bno055.hpp"
#include "PowerManager.hpp"
#include "UserButtton.hpp"
#include "bh1750.hpp"
#include "can.h"
#include "can_lib.hpp"
#include "cpp_main.hpp"
#include "encoder.hpp"
#include "etl/vector.h"
#include "hcsr04.hpp"
#include "i2c.h"
#include "led_strip.hpp"
#include "min.h"
#include "stdarg.h"
#include "stdio.h"
#include "usb_com_port.hpp"

void MobiController::debug_print(const char *format, ...) {
#ifdef MOBI_DEBUG
  va_list args;
  va_start(args, format);
  printf("MobiController: ");
  vprintf(format, args);
  va_end(args);
#endif
}

void MobiController::loop() {
  min_poll(&min_ctx, {}, 0);

  this->handle_periodic_update();
  this->handle_command_queue();
  this->handle_data_frame_queue();
}

void MobiController::queue_command(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload) {
  // Check that the recived id is a valid command.
  if (min_id < 0x20 || min_id > 0x2B) {
    debug_print("Recieved unkown command with ID: %#.2x!\n", min_id);
    this->send_status(STATUS::UNKOWN_COMMAND);
    return;
  }

  QueuedCommand cmd = {
      .min_id = static_cast<COMMANDS>(min_id),
      .payload = min_payload,
      .payload_length = len_payload};
  this->command_queue.push(cmd);
}

void MobiController::queue_data_frame(SubDevice sub_device) {
  this->data_frame_queue.push(sub_device);
}

void MobiController::queue_data_frame(DATA data, uint8_t sub_device_mask) {
  SubDevice sub_device = {
      .min_id = data,
      .sub_device_mask = sub_device_mask,
  };
  this->queue_data_frame(sub_device);
}

void MobiController::enable_periodic_update(SubDevice sub_device, uint16_t interval) {
  PeriodicUpdate periodic_update = {
      .sub_device = sub_device,
      .interval = interval,
  };
  this->periodic_updates.push_back(periodic_update);
}
void MobiController::enable_periodic_update(DATA data, uint16_t interval) {
  SubDevice sub_device = {
      .min_id = data,
      .sub_device_mask = 0,
  };
  this->enable_periodic_update(sub_device, interval);
}

void MobiController::disable_periodic_update(SubDevice sub_device) {
  for (PeriodicUpdate *it = this->periodic_updates.begin(); it < this->periodic_updates.end(); ++it) {
    if (it->sub_device.min_id == sub_device.min_id && it->sub_device.sub_device_mask == sub_device.sub_device_mask) this->periodic_updates.erase(it);
  }
}

void MobiController::disable_periodic_update(DATA data) {
  SubDevice sub_device = {
      .min_id = data,
      .sub_device_mask = 0,
  };
  this->disable_periodic_update(sub_device);
}

void MobiController::enable_periodic_update_if_disabled(SubDevice sub_device, uint16_t interval) {
  // First disable the periodic update, if enabled. Then enable it.
  // This is done to allow the change of the interval

  // Disable periodic update
  this->disable_periodic_update_if_enabled(sub_device);
  // enable periodic update
  this->enable_periodic_update(sub_device, interval);
}
void MobiController::enable_periodic_update_if_disabled(DATA data, uint16_t interval) {
  // First disable the periodic update, if enabled. Then enable it.
  // This is done to allow the change of the interval

  // Disable periodic update
  this->disable_periodic_update_if_enabled(data);
  // enable periodic update
  this->enable_periodic_update(data, interval);
}

void MobiController::disable_periodic_update_if_enabled(SubDevice sub_device) {
  if (this->is_periodic_update_enabled(sub_device))
    this->disable_periodic_update(sub_device);
}

void MobiController::disable_periodic_update_if_enabled(DATA data) {
  if (this->is_periodic_update_enabled(data))
    this->disable_periodic_update(data);
}

bool MobiController::is_periodic_update_enabled(SubDevice sub_device) {
  for (PeriodicUpdate *it = this->periodic_updates.begin(); it < this->periodic_updates.end(); ++it) {
    if (it->sub_device.min_id == sub_device.min_id && it->sub_device.sub_device_mask == sub_device.sub_device_mask) return true;
  }
  return false;
}

bool MobiController::is_periodic_update_enabled(DATA data) {
  SubDevice sub_device = {
      .min_id = data,
      .sub_device_mask = 0,
  };
  return this->is_periodic_update_enabled(sub_device);
}

// -------------------------------------------------------------------------------------------------------
// Private
// -------------------------------------------------------------------------------------------------------

etl::vector<uint8_t, 8> MobiController::extract_subdevices_from_byte(uint8_t byte) {
  etl::vector<uint8_t, 8> sub_devices;

  for (size_t i = 0; i < 8; i++) {
    // extract subdevice ids
    uint8_t current_mask = 0x01 << i;
    if ((byte & current_mask) == current_mask)
      sub_devices.push_back(current_mask);
  }

  return sub_devices;
}

MobiController::MobiController() {
  min_init_context(&min_ctx, 0);  // Init min proto

  // Init all devices
  this->pwr_manager = new PowerManager();
  this->imu = new Bno055(&hi2c1);
  this->user_btn = new UserButtton(USER_BTN_GPIO_Port, USER_BTN_Pin);
  this->light_sensor = new BH1750(&hi2c1, BH1750::DEFAULT_ADDRESS);
  this->ultrasonic_sensor_1 = new HCSR04(&htim1, TIM_Channel_1, TRIG_1_GPIO_Port, TRIG_1_Pin);
  this->ultrasonic_sensor_2 = new HCSR04(&htim2, TIM_Channel_3, TRIG_2_GPIO_Port, TRIG_2_Pin);
  this->ultrasonic_sensor_3 = new HCSR04(&htim1, TIM_Channel_2, TRIG_3_GPIO_Port, TRIG_3_Pin);
  this->ultrasonic_sensor_4 = new HCSR04(&htim2, TIM_Channel_1, TRIG_4_GPIO_Port, TRIG_4_Pin);
  this->ultrasonic_sensor_5 = new HCSR04(&htim2, TIM_Channel_4, TRIG_5_GPIO_Port, TRIG_5_Pin);
  this->ultrasonic_sensor_6 = new HCSR04(&htim2, TIM_Channel_2, TRIG_6_GPIO_Port, TRIG_6_Pin);
  this->encoder_1 = new Encoder(ENCODER_1_A_GPIO_Port, ENCODER_1_A_Pin, ENCODER_1_B_GPIO_Port, ENCODER_1_B_Pin);
  this->encoder_2 = new Encoder(ENCODER_2_A_GPIO_Port, ENCODER_2_A_Pin, ENCODER_2_B_GPIO_Port, ENCODER_2_B_Pin);
  this->encoder_3 = new Encoder(ENCODER_3_A_GPIO_Port, ENCODER_3_A_Pin, ENCODER_3_B_GPIO_Port, ENCODER_3_B_Pin);
  this->encoder_4 = new Encoder(ENCODER_4_A_GPIO_Port, ENCODER_4_A_Pin, ENCODER_4_B_GPIO_Port, ENCODER_4_B_Pin);
  this->can_lib = new CAN_LIB(&hcan1);
  this->can_lib->send_stop();

  LED_STRIP::init();
  LED_STRIP::clear_and_update();
  LED_STRIP::set_brightness(50);
}

void MobiController::handle_advanced_command(QueuedCommand cmd, DATA data) {
  // Check which sub devices
  auto sub_device_masks = extract_subdevices_from_byte(cmd.payload[0]);

  // Go through all subdevices
  for (auto it = sub_device_masks.begin(); it < sub_device_masks.end(); ++it) {
    SubDevice sub_device = {.min_id = data,
                            .sub_device_mask = *it};

    if (cmd.payload_length == 3) {
      PayloadBuilder *pb = new PayloadBuilder(cmd.payload + 1, cmd.payload_length - 1);  // Payload builder without first byte
      uint16_t freq = pb->read_uint16();
      this->enable_periodic_update_if_disabled(sub_device, freq);
      delete pb;
      continue;
    }

    this->disable_periodic_update_if_enabled(sub_device);
    this->queue_data_frame(sub_device);
  }
}

void MobiController::handle_basic_command(QueuedCommand cmd, DATA data) {
  if (cmd.payload_length == 2) {
    PayloadBuilder *pb = new PayloadBuilder(cmd.payload, cmd.payload_length);
    uint16_t freq = pb->read_uint16();
    delete pb;
    this->enable_periodic_update_if_disabled(data, freq);
    return;
  }

  // Disable periodic update if read without frequency
  this->disable_periodic_update_if_enabled(data);
  this->queue_data_frame(data);
}

void MobiController::send_status(STATUS status) {
  USB_COM_PORT::queue_byte(0, static_cast<uint8_t>(status));
}

void MobiController::handle_periodic_update() {
  uint32_t time_now_ms = HAL_GetTick();

  for (PeriodicUpdate *it = this->periodic_updates.begin(); it < this->periodic_updates.end(); ++it) {
    // Check if should be sent
    if (time_now_ms >= it->last_sent_ms + it->interval) {
      debug_print("Sending periodic update of ID: %#.2x\n", it->sub_device.min_id);

      it->last_sent_ms = time_now_ms;
      this->queue_data_frame(it->sub_device);
    }
  }
}

void MobiController::handle_command_queue() {
  // Allow for the processing of 10 commands
  for (size_t i = 0; i < 10; i++) {
    if (this->command_queue.size() == 0) return;

    QueuedCommand cmd;
    this->command_queue.pop_into(cmd);

    debug_print("Got command from queue with ID: %#.2x\n", cmd.min_id);

    switch (cmd.min_id) {
      case COMMANDS::IMU: {
        if (cmd.payload_length == 0 || cmd.payload[0] == 0 || (cmd.payload[0] | 0b01111111) == 0xff) {
          debug_print("Got invalid IMU subdevice!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        this->handle_advanced_command(cmd, DATA::IMU);
        break;
      }

      case COMMANDS::ULTRASONIC_SENSOR: {
        if (cmd.payload_length == 0 || cmd.payload[0] == 0 || (cmd.payload[0] & 0x80) != 0 || (cmd.payload[0] & 0x40) != 0) {
          debug_print("Got invalid ultrasonic sensor subdevice!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        this->handle_advanced_command(cmd, DATA::ULTRASONIC_SENSOR);
        break;
      }

      case COMMANDS::ENCODER: {
        if (cmd.payload_length == 0 || cmd.payload[0] == 0 || cmd.payload[0] > 0xF) {
          debug_print("Got invalid encoder subdevice!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        this->handle_advanced_command(cmd, DATA::ENCODER);
        break;
      }

      case COMMANDS::BRIGHTNESS: {
        this->handle_basic_command(cmd, DATA::BRIGHTNESS);
        break;
      }

      case COMMANDS::TEMPERATURE: {
        this->handle_basic_command(cmd, DATA::TEMPERATURE);
        break;
      }

      case COMMANDS::BAT_VOLTAGE: {
        this->handle_basic_command(cmd, DATA::BAT_VOLTAGE);
        break;
      }

      case COMMANDS::USER_BUTTON: {
        if (cmd.payload_length > 1) {
          debug_print("Got invalid parameter for user button!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        if (cmd.payload_length == 1) {
          // Set the recived mode
          UserButtton::MODE mode = static_cast<UserButtton::MODE>(cmd.payload[0]);
          this->user_btn->set_mode(mode);
        } else {
          // Toggle the mode
          auto current_mode = this->user_btn->get_mode();
          if (current_mode == UserButtton::MODE::EXTERNAL)
            this->user_btn->set_mode(UserButtton::MODE::INTERNAL);
          else if (current_mode == UserButtton::MODE::INTERNAL)
            this->user_btn->set_mode(UserButtton::MODE::EXTERNAL);
        }

        this->send_status(STATUS::OK);
        break;
      }

      case COMMANDS::LED_STRIP: {
        if (cmd.payload_length == 0) {  // turn off the leds
          LED_STRIP::stop_animation();
          LED_STRIP::clear_and_update();
          this->pwr_manager->set_power_led(false);

        } else if (cmd.payload_length == 1 || (cmd.payload_length >= 5 && cmd.payload_length <= 10)) {  // Select preset
          PayloadBuilder *pb = new PayloadBuilder(cmd.payload, cmd.payload_length);
          uint8_t preset = pb->read_uint8();

          LED_STRIP::COLOR_RGBW color = {255, 255, 0, 0};

          if (cmd.payload_length >= 5) {
            color.r = pb->read_uint8();
            color.g = pb->read_uint8();
            color.b = pb->read_uint8();
            color.w = pb->read_uint8();
          }

          this->pwr_manager->set_power_led(true);
          LED_STRIP::stop_animation();
          LED_STRIP::clear_and_update();

          switch (preset) {
            // Driving lights
            case 0: {
              LED_STRIP::driving_light();
              break;
            }

            // Beacon
            case 1: {
              uint8_t update_rate = cmd.payload_length >= 6 ? pb->read_uint8() : 5;
              uint8_t line_length = cmd.payload_length >= 7 ? pb->read_uint8() : 4;
              uint8_t line_count = cmd.payload_length >= 8 ? pb->read_uint8() : 4;
              uint8_t rotate_left = cmd.payload_length >= 9 ? static_cast<bool>(pb->read_uint8()) : false;
              uint8_t frame_count = cmd.payload_length == 10 ? pb->read_uint8() : NUM_PIXELS;

              LED_STRIP::beacon_rgbw(color, update_rate, frame_count, line_length, line_count, rotate_left);
              break;
            }

            // Blink
            case 2: {
              uint8_t update_rate = cmd.payload_length >= 6 ? pb->read_uint8() : 50;
              uint8_t line_length = cmd.payload_length >= 7 ? pb->read_uint8() : NUM_PIXELS;
              uint8_t line_count = cmd.payload_length == 8 ? pb->read_uint8() : 1;

              LED_STRIP::blink(color, update_rate, line_length, line_count);
              break;
            }

            // On
            case 3: {
              LED_STRIP::fill_rgbw(color);
              LED_STRIP::update();
              break;
            }

            // Default: invalid preset id
            default: {
              this->send_status(STATUS::INVALID_PARAMETER);
              break;
            }
          }

          delete pb;

        } else {  // Error
          debug_print("Got invalid parameter for led strip!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        this->send_status(STATUS::OK);
        break;
      }

      case COMMANDS::MOTOR_CONTROL: {
        if (cmd.payload_length > 6 || cmd.payload_length % 2 != 0) {
          debug_print("Got invalid parameter for motor control!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        int16_t v[3] = {};

        if (cmd.payload_length != 0) {
          PayloadBuilder *pb = new PayloadBuilder(cmd.payload, cmd.payload_length);

          for (size_t i = 0; i < 3; ++i) {
            if (pb->size() == 0) break;
            v[i] = pb->read_int16();
          }
          delete pb;
        }

        auto status = this->can_lib->drive(v[0], v[1], v[2]);
        if (status == HAL_OK)
          this->send_status(STATUS::OK);
        else
          this->send_status(STATUS::ERROR);

        break;
      }

      case COMMANDS::POZYX_POWER: {
        if (cmd.payload_length > 1 || (cmd.payload_length == 1 && cmd.payload[0] > 1)) {
          debug_print("Got invalid parameter for pozyx power!\n");
          this->send_status(STATUS::INVALID_PARAMETER);
          break;
        }

        if (cmd.payload_length == 1) {
          // Set the recived mode
          this->pwr_manager->set_power_pozyx(cmd.payload[0]);
        } else {
          this->pwr_manager->toggle_power_pozyx();
        }

        this->send_status(STATUS::OK);
        break;
      }

        // TODO: Handle all other commands

      default:
        debug_print("Queued unkown command!\n");
        this->send_status(STATUS::UNKOWN_COMMAND);
        break;
    }
  }
}

void MobiController::handle_data_frame_queue() {
  // Allow for the processing of 10 data frames
  for (size_t i = 0; i < 10; i++) {
    if (this->data_frame_queue.size() == 0) return;
    SubDevice sub_device;
    this->data_frame_queue.pop_into(sub_device);

    debug_print("Got item from data queue with ID: %#.2x\n", static_cast<uint8_t>(sub_device.min_id));

    switch (sub_device.min_id) {
      case DATA::IMU: {
        PayloadBuilder *pb = new PayloadBuilder();  // Create a pb for the imu data
        pb->append_uint8(sub_device.sub_device_mask);

        switch (static_cast<IMU_SUB_DEVICES>(sub_device.sub_device_mask)) {
          case IMU_SUB_DEVICES::ACCELEROMETER: {
            pb->append_vector(this->imu->get_vector_accelerometer());  // Read the accel value from the imu and append it to the pb
            break;
          }
          case IMU_SUB_DEVICES::MAGNETOMETER: {
            pb->append_vector(this->imu->get_vector_magnetometer());
            break;
          }
          case IMU_SUB_DEVICES::GYROSCOPE: {
            pb->append_vector(this->imu->get_vector_gyroscope());
            break;
          }
          case IMU_SUB_DEVICES::EULER: {
            pb->append_vector(this->imu->get_vector_euler());
            break;
          }
          case IMU_SUB_DEVICES::LINEAR_ACCEL: {
            pb->append_vector(this->imu->get_vector_linear_accel());
            break;
          }
          case IMU_SUB_DEVICES::GRAVITY: {
            pb->append_vector(this->imu->get_vector_gravity());
            break;
          }
          case IMU_SUB_DEVICES::QUATERNION: {
            pb->append_vector(this->imu->get_vector_quaternion());
            break;
          }
        }

        USB_COM_PORT::queue_payload(DATA::IMU, pb);  // queue the payload as a min frame
        delete pb;
        break;
      }

      case DATA::ULTRASONIC_SENSOR: {
        PayloadBuilder *pb = new PayloadBuilder();
        pb->append_uint8(sub_device.sub_device_mask);

        switch (static_cast<ULTRASONIC_SENSOR_SUB_DEVICES>(sub_device.sub_device_mask)) {
          case ULTRASONIC_SENSOR_SUB_DEVICES::US_1: {
            this->ultrasonic_sensor_1->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_1->get_distance());
            break;
          }

          case ULTRASONIC_SENSOR_SUB_DEVICES::US_2: {
            this->ultrasonic_sensor_2->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_2->get_distance());
            break;
          }

          case ULTRASONIC_SENSOR_SUB_DEVICES::US_3: {
            this->ultrasonic_sensor_3->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_3->get_distance());
            break;
          }

          case ULTRASONIC_SENSOR_SUB_DEVICES::US_4: {
            this->ultrasonic_sensor_4->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_4->get_distance());
            break;
          }

          case ULTRASONIC_SENSOR_SUB_DEVICES::US_5: {
            this->ultrasonic_sensor_5->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_5->get_distance());
            break;
          }

          case ULTRASONIC_SENSOR_SUB_DEVICES::US_6: {
            this->ultrasonic_sensor_6->read();
            HAL_Delay(10);
            pb->append_float(this->ultrasonic_sensor_6->get_distance());
            break;
          }
        }

        USB_COM_PORT::queue_payload(DATA::ULTRASONIC_SENSOR, pb);
        delete pb;
        break;
      }

      case DATA::ENCODER: {
        PayloadBuilder *pb = new PayloadBuilder();
        pb->append_uint8(sub_device.sub_device_mask);

        switch (static_cast<ENCODER_SUB_DEVICES>(sub_device.sub_device_mask)) {
          case ENCODER_SUB_DEVICES::ENCODER_1: {
            pb->append_uint16(this->encoder_1->get_counter());
            break;
          }

          case ENCODER_SUB_DEVICES::ENCODER_2: {
            pb->append_uint16(this->encoder_2->get_counter());
            break;
          }

          case ENCODER_SUB_DEVICES::ENCODER_3: {
            pb->append_uint16(this->encoder_3->get_counter());
            break;
          }

          case ENCODER_SUB_DEVICES::ENCODER_4: {
            pb->append_uint16(this->encoder_4->get_counter());
            break;
          }
        }

        USB_COM_PORT::queue_payload(DATA::ENCODER, pb);
        delete pb;
        break;
      }

      case DATA::BRIGHTNESS: {
        PayloadBuilder *pb = new PayloadBuilder();
        pb->append_float(this->light_sensor->read_light());
        USB_COM_PORT::queue_payload(DATA::BRIGHTNESS, pb);
        delete pb;
        break;
      }

      case DATA::TEMPERATURE:
        USB_COM_PORT::queue_byte(DATA::TEMPERATURE, this->imu->get_temp());
        break;

      case DATA::BAT_VOLTAGE: {
        PayloadBuilder *pb = new PayloadBuilder();
        pb->append_float(this->pwr_manager->get_battery_voltage());
        USB_COM_PORT::queue_payload(DATA::BAT_VOLTAGE, pb);
        delete pb;
        break;
      }
        // TODO: Read all other sensors
      default:
        break;
    }
  }
}
