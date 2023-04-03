

#include "usb_com_port.hpp"

#include "etl/string.h"
#include "stm32l4xx.h"
#include "usbd_cdc_if.h"

void USB_COM_PORT::send_str(etl::string<64> str) {
  CDC_Transmit_FS(reinterpret_cast<uint8_t *>(&str[0]), str.length());
}

void USB_COM_PORT::send_data(uint8_t *data, uint16_t len) {
  CDC_Transmit_FS(data, len);
  memset(data, '\0', len);
}
