#include "MobiController.hpp"

#include "Bno055.hpp"
#include "cpp_main.hpp"
#include "i2c.h"
#include "min.h"
#include "stdio.h"
#include "usb_com_port.hpp"

void MobiController::debug_print(const char *msg, ...) {
#ifdef MOBI_DEBUG
  printf("MobiController: ");
  printf(msg);
#endif
}

void MobiController::loop() {
  min_poll(&min_ctx, {}, 0);

  this->handle_command_queue();
  this->handle_data_frame_queue();
}

void MobiController::queue_command(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload) {
  // Check that the recived id is a valid command.
  if (min_id < 0x20 || min_id > 0x2B) {
    debug_print("Recieved unkown command with ID: %d!\n", min_id);
    return;
  }

  QueuedCommand cmd = {
      .min_id = static_cast<COMMANDS>(min_id),
      .payload = min_payload,
      .payload_length = len_payload};
  this->command_queue.push(cmd);
}

void MobiController::handle_command_queue() {
  // Allow for the processing of 10 commands
  for (size_t i = 0; i < 10; i++) {
    if (this->command_queue.size() == 0) return;

    QueuedCommand cmd;
    this->command_queue.pop_into(cmd);

    debug_print("Got command from queue with ID: %d\n", static_cast<uint8_t>(cmd.min_id));

    switch (cmd.min_id) {
      case COMMANDS::TEMPERATURE: {
        if (cmd.payload_length == 2) {
          PayloadBuilder *pb = new PayloadBuilder(cmd.payload, cmd.payload_length);
          uint16_t freq = pb->read_uint16();
          printf("Freq: %d\n", freq);
          // TODO: repeatedly send temp with freq
          break;
        }

        mobictl().queue_data_frame(MobiController::DATA::TEMPERATURE);
        break;
      }

      default:
        debug_print("Queued unkown command!\n");
        break;
    }
  }
}

void MobiController::queue_data_frame(DATA data) {
  this->data_frame_queue.push(data);
}

void MobiController::handle_data_frame_queue() {
  // Allow for the processing of 10 data frames
  for (size_t i = 0; i < 10; i++) {
    if (this->data_frame_queue.size() == 0) return;

    DATA data;
    this->data_frame_queue.pop_into(data);
    debug_print("Got item from data queue with ID: %d\n", static_cast<uint8_t>(data));

    switch (data) {
      case DATA::TEMPERATURE:
        USB_COM_PORT::queue_byte(DATA::TEMPERATURE, this->imu->get_temp());
        break;

      default:
        break;
    }
  }
}

MobiController::MobiController() {
  min_init_context(&min_ctx, 0);
  this->imu = new Bno055(&hi2c1);
}
