#ifndef MOBI_CONTROLLER_H_
#define MOBI_CONTROLLER_H_

#include "Bno055.hpp"
#include "etl/queue.h"
#include "min.h"

class MobiController {
 public:
  static MobiController &mobictl() {
    static MobiController instance;
    return instance;
  }

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

  struct QueuedCommand {
    COMMANDS min_id;
    uint8_t const *payload;
    uint8_t payload_length;
  };

  Bno055 *imu;

  void loop();

  void handle_command_queue();
  void queue_command(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload);

  void handle_data_frame_queue();
  void queue_data_frame(DATA data);

 private:
  MobiController();                                            // private constructor
  MobiController(const MobiController &) = delete;             // disable copy constructor
  MobiController &operator=(const MobiController &) = delete;  // disable copy assignment operator

  etl::queue<DATA, 100> data_frame_queue;
  etl::queue<QueuedCommand, 100> command_queue;
};

#endif
