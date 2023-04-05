#ifndef USB_COM_PORT_H_
#define USB_COM_PORT_H_

#include "MobiController.hpp"
#include "PayloadBuilder.hpp"
#include "etl/queue.h"
#include "etl/string.h"
#include "stm32l4xx.h"

namespace USB_COM_PORT {

inline etl::queue<uint8_t, 265, etl::memory_model::MEMORY_MODEL_MEDIUM> tx_queue;  // 265 max length of min frame

void send_str(etl::string<64> str);
void send_data(uint8_t* data, uint16_t len);

void queue_payload(uint8_t min_id, PayloadBuilder* pb);
void queue_payload(MobiController::DATA min_id, PayloadBuilder* pb);

void queue_byte(uint8_t min_id, uint8_t byte);
void queue_byte(MobiController::DATA min_id, uint8_t byte);

}  // namespace  USB_COM_PORT

#endif /*USB_COM_PORT_H_*/