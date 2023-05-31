#include "PayloadBuilder.hpp"

#include "etl/memory.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "min.h"

PayloadBuilder::PayloadBuilder() {
}

PayloadBuilder::PayloadBuilder(uint8_t const* bytes, uint8_t length) {
  this->payload.insert(this->payload.begin(), bytes, bytes + length);
}

PayloadBuilder::PayloadBuilder(etl::vector<uint8_t, MAX_PAYLOAD> payload) {
  this->payload = payload;
}

void PayloadBuilder::append_uint8(uint8_t number) {
  this->payload.push_back(number);
}

void PayloadBuilder::append_uint16(uint16_t number) {
  uint8_t buffer[2];
  etl::mem_copy((uint8_t*)&number, 2, buffer);
  this->payload.insert(this->payload.end(), buffer, buffer + 2);
}

void PayloadBuilder::append_uint32(uint32_t number) {
  uint8_t buffer[4];
  etl::mem_copy((uint8_t*)&number, 4, buffer);
  this->payload.insert(this->payload.end(), buffer, buffer + 4);
}

void PayloadBuilder::append_float(float number) {
  uint8_t buffer[4];
  etl::mem_copy((uint8_t*)&number, 4, buffer);
  this->payload.insert(this->payload.end(), buffer, buffer + 4);
}

void PayloadBuilder::append_double(double number) {
  uint8_t buffer[8];
  etl::mem_copy((uint8_t*)&number, 8, buffer);
  this->payload.insert(this->payload.end(), buffer, buffer + 8);
}

void PayloadBuilder::append_vector(Bno055::vector_t vec) {
  this->append_double(vec.w);
  this->append_double(vec.x);
  this->append_double(vec.y);
  this->append_double(vec.z);
}

void PayloadBuilder::append_vector(Pozyx::vector_t vec) {
  this->append_float(vec.w);
  this->append_float(vec.x);
  this->append_float(vec.y);
  this->append_float(vec.z);
}

void PayloadBuilder::append_string(etl::string<255> str) {
  this->payload.insert(this->payload.end(), str.c_str(), str.c_str() + str.length());
}

uint8_t PayloadBuilder::read_uint8() {
  uint8_t number = this->payload.front();
  this->payload.erase(this->payload.begin());
  return number;
}

uint16_t PayloadBuilder::read_uint16() {
  uint16_t number;
  etl::mem_copy(this->payload.begin(), 2, (uint8_t*)&number);
  this->payload.erase(this->payload.begin(), this->payload.begin() + 2);
  return number;
}

int16_t PayloadBuilder::read_int16() {
  uint16_t number;
  if (this->payload.size() < 2) return 0;
  etl::mem_copy(this->payload.begin(), 2, (uint8_t*)&number);
  this->payload.erase(this->payload.begin(), this->payload.begin() + 2);
  return number;
}

uint8_t* PayloadBuilder::get_payload() {
  return this->payload.begin();
}

size_t PayloadBuilder::size() {
  return this->payload.size();
}

bool PayloadBuilder::is_empty() {
  return this->payload.size() == 0;
}

PayloadBuilder::~PayloadBuilder() {
  this->payload.clear();
}