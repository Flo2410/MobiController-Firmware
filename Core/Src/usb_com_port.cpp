

#include "usb_com_port.hpp"

#include "etl/string.h"
#include "min.h"
#include "stm32l4xx.h"
#include "usbd_cdc_if.h"

void USB_COM_PORT::send_str(etl::string<64> str) {
  CDC_Transmit_FS(reinterpret_cast<uint8_t *>(&str[0]), str.length());
}

void USB_COM_PORT::send_data(uint8_t *data, uint16_t len) {
  CDC_Transmit_FS(data, len);
}

//--------------------------------------------------------------------------------------------------------------
// MIN Callback functions
//--------------------------------------------------------------------------------------------------------------
void min_tx_start(uint8_t port) {
  USB_COM_PORT::tx_queue.clear();
}

void min_tx_finished(uint8_t port) {
  USB_COM_PORT::send_data(&USB_COM_PORT::tx_queue.front(), USB_COM_PORT::tx_queue.size());
}

// Tell MIN how much space there is to write to the serial port. This is used
// inside MIN to decide whether to bother sending a frame or not.
uint16_t min_tx_space(uint8_t port) {
  // Ignore 'port' because we have just one context. But in a bigger application
  // with multiple ports we could make an array indexed by port to select the serial
  // port we need to use.
  return USB_COM_PORT::tx_queue.available();
}

// Send a character on the designated port.
void min_tx_byte(uint8_t port, uint8_t byte) {
  // Ignore 'port' because we have just one context.
  USB_COM_PORT::tx_queue.push(byte);
}

// Tell MIN the current time in milliseconds.
uint32_t min_time_ms(void) {
  return HAL_GetTick();
}

#ifdef MIN_DEBUG_PRINTING
void min_debug_print(const char *msg, ...) {
  printf("MIN: ");
  printf(msg);
}
#endif

// Handle the reception of a MIN frame. This is the main interface to MIN for receiving
// frames. It's called whenever a valid frame has been received (for transport layer frames
// duplicates will have been eliminated).
void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port) {
  // We ignore the port because we have one context, but we could use it to index an array of
  // contexts in a bigger application.
  printf("MIN frame with ID 0x%02x, length = %d and data: %s\n", min_id, len_payload, (char *)min_payload);
}