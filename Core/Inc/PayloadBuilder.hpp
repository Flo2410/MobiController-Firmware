#ifndef PAYLOAD_BUILDER_H_
#define PAYLOAD_BUILDER_H_

#include "Bno055.hpp"
#include "Pozyx.hpp"
#include "etl/string.h"
#include "etl/vector.h"
#include "min.h"

class PayloadBuilder {
 public:
  PayloadBuilder();
  PayloadBuilder(uint8_t const* bytes, uint8_t length);
  PayloadBuilder(etl::vector<uint8_t, MAX_PAYLOAD> payload);
  ~PayloadBuilder();

  void append_uint8(uint8_t number);
  void append_uint16(uint16_t number);
  void append_uint32(uint32_t number);
  void append_float(float number);
  void append_double(double number);
  void append_vector(Bno055::vector_t vec);
  void append_vector(Pozyx::vector_t vec);
  void append_string(etl::string<255> str);

  uint8_t read_uint8();
  uint16_t read_uint16();
  int16_t read_int16();

  uint8_t* get_payload();
  size_t size();

 private:
  etl::vector<uint8_t, 255> payload;
};

#endif