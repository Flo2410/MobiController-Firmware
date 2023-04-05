#include "MobiController.hpp"

#include "Bno055.hpp"
#include "cpp_main.hpp"
#include "i2c.h"
#include "min.h"
#include "stdio.h"
#include "usb_com_port.hpp"

void MobiController::loop() {
  min_poll(&min_ctx, {}, 0);

  this->handle_command_queue();
  this->handle_data_frame_queue();
}

void MobiController::queue_command(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload) {
  QueuedCommand cmd = {
      .min_id = static_cast<COMMANDS>(min_id),
      .payload = min_payload,
      .payload_length = len_payload};
  this->command_queue.push(cmd);
}

void MobiController::handle_command_queue() {
  while (this->command_queue.size() > 0) {
    QueuedCommand cmd;
    this->command_queue.pop_into(cmd);

#ifdef MOBI_DEBUG
    printf("Got command from queue with ID: %d\n", static_cast<uint8_t>(cmd.min_id));
#endif

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
#ifdef MOBI_DEBUG
        printf("MobiController: recieved unkown command!\n");
#endif
        break;
    }
  }
}

void MobiController::queue_data_frame(DATA data) {
  this->data_frame_queue.push(data);
}

void MobiController::handle_data_frame_queue() {
  while (this->data_frame_queue.size() > 0) {
    DATA data;
    this->data_frame_queue.pop_into(data);

#ifdef MOBI_DEBUG
    printf("Got item from data queue with ID: %d\n", static_cast<uint8_t>(data));
#endif

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
