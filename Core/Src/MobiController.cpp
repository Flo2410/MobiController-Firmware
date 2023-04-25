#include "MobiController.hpp"

#include "Bno055.hpp"
#include "UserButtton.hpp"
#include "adc.h"
#include "bh1750.hpp"
#include "cpp_main.hpp"
#include "etl/vector.h"
#include "i2c.h"
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
  this->imu = new Bno055(&hi2c1);
  this->user_btn = new UserButtton(USER_BTN_GPIO_Port, USER_BTN_Pin);
  this->light_sensor = new BH1750(&hi2c1, BH1750::DEFAULT_ADDRESS);
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

        // Check which sub devices
        auto sub_device_masks = extract_subdevices_from_byte(cmd.payload[0]);

        // Go through all subdevices
        for (auto it = sub_device_masks.begin(); it < sub_device_masks.end(); ++it) {
          SubDevice sub_device = {.min_id = DATA::IMU,
                                  .sub_device_mask = *it};

          if (cmd.payload_length == 3) {
            PayloadBuilder *pb = new PayloadBuilder(cmd.payload + 1, cmd.payload_length - 1);  // Payload builder without first byte
            uint16_t freq = pb->read_uint16();
            this->enable_periodic_update_if_disabled(sub_device, freq);
            continue;
          }

          this->disable_periodic_update_if_enabled(sub_device);
          this->queue_data_frame(sub_device);
        }

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
        // TODO: Handle all other commands

      default:
        debug_print("Queued unkown command!\n");
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
        HAL_ADC_Start(&hadc1);  // Start ADC
        HAL_ADC_PollForConversion(&hadc1, 1);
        float U = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, HAL_ADC_GetValue(&hadc1), LL_ADC_RESOLUTION_12B) * 0.001;
        PayloadBuilder *pb = new PayloadBuilder();
        pb->append_float(U);
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
