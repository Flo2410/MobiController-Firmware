#include "PayloadBuilder.hpp"

#include "etl/memory.h"

PayloadBuilder::PayloadBuilder() {
}

void PayloadBuilder::append_uint8(uint8_t number) {
  this->payload.push_back(number);
}

void PayloadBuilder::append_uint16(uint16_t number) {
  uint8_t buffer[2];
  etl::mem_copy((uint8_t*)&number, 2, buffer);
  this->payload.push_back(buffer[0]);
  this->payload.push_back(buffer[1]);
}

void PayloadBuilder::append_uint32(uint32_t number) {
}

void PayloadBuilder::append(float number) {
}

void PayloadBuilder::append(double number) {
}

uint8_t* PayloadBuilder::get_payload() {
  return this->payload.begin();
}

size_t PayloadBuilder::size() {
  return this->payload.size();
}

PayloadBuilder::~PayloadBuilder() {
  this->payload.clear();
}