#ifndef PAYLOAD_BUILDER_H_
#define PAYLOAD_BUILDER_H_

#include "etl/vector.h"

class PayloadBuilder {
 public:
  PayloadBuilder();
  ~PayloadBuilder();

  void append_uint8(uint8_t number);
  void append_uint16(uint16_t number);
  void append_uint32(uint32_t number);
  void append_float(float number);
  void append_double(double number);

  uint8_t* get_payload();
  size_t size();

 private:
  etl::vector<uint8_t, 255> payload;
};

#endif