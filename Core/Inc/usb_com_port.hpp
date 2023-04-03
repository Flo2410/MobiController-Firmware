
#ifndef USB_COM_PORT_H_
#define USB_COM_PORT_H_
#include "etl/string.h"
#include "stm32l4xx.h"

namespace USB_COM_PORT {

void send_str(etl::string<64> str);
void send_data(uint8_t* data, uint16_t len);

}  // namespace  USB_COM_PORT

#endif /*USB_COM_PORT_H_*/