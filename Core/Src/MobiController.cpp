#include "MobiController.hpp"

#include "Bno055.hpp"
#include "cpp_main.hpp"
#include "i2c.h"
#include "min.h"
#include "usb_com_port.hpp"

void MobiController::handle_commands(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload) {
  switch (static_cast<COMMANDS>(min_id)) {
    case COMMANDS::TEMPERATURE:
      mobictl().queue_data_frame(MobiController::DATA::TEMPERATURE);
      break;

    default:
      printf("MIN recived frame with unknown min_id!\n");
      break;
  }
}

void MobiController::loop() {
  min_poll(&min_ctx, {}, 0);

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

void MobiController::queue_data_frame(DATA data) {
  this->data_frame_queue.push(data);
}

MobiController::MobiController() {
  min_init_context(&min_ctx, 0);
  this->imu = new Bno055(&hi2c1);
}
