#ifndef PAYLOAD_BUILDER_H_
#define PAYLOAD_BUILDER_H_

#include "Bno055.hpp"
#include "etl/vector.h"

class PayloadBuilder {
 public:
  PayloadBuilder();
  PayloadBuilder(uint8_t const* bytes, uint8_t length);
  ~PayloadBuilder();

  void append_uint8(uint8_t number);
  void append_uint16(uint16_t number);
  void append_uint32(uint32_t number);
  void append_float(float number);
  void append_double(double number);
  void append_vector(Bno055::vector_t vec);

  uint8_t read_uint8();
  uint16_t read_uint16();
  uint32_t read_uint32();
  float read_float();
  double read_double();

  uint8_t* get_payload();
  size_t size();

 private:
  etl::vector<uint8_t, 255> payload;
};

#endif