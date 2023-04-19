#ifndef MOBI_CONTROLLER_H_
#define MOBI_CONTROLLER_H_

#include "Bno055.hpp"
#include "etl/queue.h"
#include "etl/vector.h"
#include "min.h"

class MobiController {
 public:
  static MobiController &mobictl() {
    static MobiController instance;
    return instance;
  }

  static void debug_print(const char *msg, ...);

  // commands
  enum class COMMANDS {
    IMU = 0x20,
    ULTRASONIC_SENSOR = 0x21,
    ENCODER = 0x22,
    BRIGHTNESS = 0x23,
    TEMPERATURE = 0x24,
    BAT_VOLTAGE = 0x25,
    USER_BUTTON = 0x26,
    LED_STRIP = 0x27,
    MOTOR_CONTROL = 0x28,
    POZYX_POWER = 0x29,
    POZYX = 0x2a,
    POZYX_CONFIG = 0x2b,
  };

  // data
  enum class DATA {
    COMMAND_STAUTS = 0x0,
    IMU = 0x1,
    ULTRASONIC_SENSOR = 0x2,
    ENCODER = 0x3,
    BRIGHTNESS = 0x4,
    TEMPERATURE = 0x5,
    BAT_VOLTAGE = 0x6,
    USER_BUTTON = 0x7,
    POZYX = 0x8,
    POZYX_CONFIG = 0x9,
  };

  enum class IMU_SUB_DEVICES {
    ACCELEROMETER = 0x1,
    MAGNETOMETER = 0x2,
    GYROSCOPE = 0x4,
    EULER = 0x8,
    LINEAR_ACCEL = 0x10,
    GRAVITY = 0x20,
    QUATERNION = 0x40,
  };

  enum class ULTRASONIC_SENSOR_SUB_DEVICES {
    US_1 = 0x1,
    US_2 = 0x2,
    US_3 = 0x4,
    US_4 = 0x8,
    US_5 = 0x10,
    US_6 = 0x20,
  };

  enum class ENCODER_SUB_DEVICES {
    ENCODER_1 = 0x1,
    ENCODER_2 = 0x2,
    ENCODER_3 = 0x4,
    ENCODER_4 = 0x8,
  };

  enum class USER_BUTTON_SUB_DEVICES {
    USE_USER_BUTTON = 0x1,
  };

  enum class POZYX_POWER_SUB_DEVICES {
    POWER = 0x1,
  };

  enum class POZYX_SUB_DEVICES {
    POSITION = 0x1,
    ORIENTATION = 0x2,
  };

  struct QueuedCommand {
    COMMANDS min_id;
    uint8_t const *payload;
    uint8_t payload_length;
  };

  struct SubDevice {
    DATA min_id;
    uint8_t sub_device_mask = 0;
  };

  struct PeriodicUpdate {
    SubDevice sub_device;
    uint16_t interval;
    uint32_t last_sent_ms = 0;
  };

  Bno055 *imu;

  void loop();

  void queue_command(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload);

  void queue_data_frame(SubDevice sub_device);
  void queue_data_frame(DATA data, uint8_t sub_device_mask = 0);

  void enable_periodic_update(SubDevice sub_device, uint16_t interval);
  void enable_periodic_update(DATA data, uint16_t interval);

  void disable_periodic_update(SubDevice sub_device);
  void disable_periodic_update(DATA data);

  void enable_periodic_update_if_disabled(SubDevice sub_device, uint16_t interval);
  void enable_periodic_update_if_disabled(DATA data, uint16_t interval);

  void disable_periodic_update_if_enabled(SubDevice sub_device);
  void disable_periodic_update_if_enabled(DATA data);

  bool is_periodic_update_enabled(SubDevice sub_device);
  bool is_periodic_update_enabled(DATA data);

 private:
  static etl::vector<uint8_t, 8> extract_subdevices_from_byte(uint8_t byte);

  MobiController();                                            // private constructor
  MobiController(const MobiController &) = delete;             // disable copy constructor
  MobiController &operator=(const MobiController &) = delete;  // disable copy assignment operator

  etl::queue<SubDevice, 100> data_frame_queue;
  etl::queue<QueuedCommand, 100> command_queue;
  etl::vector<PeriodicUpdate, 20> periodic_updates;

  void handle_periodic_update();
  void handle_command_queue();
  void handle_data_frame_queue();
};

#endif
