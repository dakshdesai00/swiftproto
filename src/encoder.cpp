#include "../include/encoder.h"
#include "../include/varint.h"
#include <cstdint>

void write_tag(uint32_t field_number, WireType wire_type,
               std::vector<uint8_t> &buf) {
  uint64_t tag = ((uint64_t)field_number << 3) | wire_type;
  encode_varint(tag, buf);
}

void write_int64(uint32_t field_number, int64_t value,
                 std::vector<uint8_t> &buf) {
  write_tag(field_number, VARINT, buf);
  encode_signed(value, buf);
}

void write_bool(uint32_t field_number, bool value, std::vector<uint8_t> &buf) {
  write_tag(field_number, VARINT, buf);
  encode_varint(value ? 1 : 0, buf);
}

void write_string(uint32_t field_number, std::string_view value,
                  std::vector<uint8_t> &buf) {
  write_tag(field_number, LENGTH_DELIMITED, buf);
  encode_varint(value.size(), buf);
  for (uint8_t byte : value) {
    buf.push_back(byte);
  }
}

void write_float(uint32_t field_number, float value,
                 std::vector<uint8_t> &buf) {
  write_tag(field_number, FIXED32, buf);
  uint32_t bits;
  memcpy(&bits, &value, sizeof(float));
  for (int i = 0; i < 4; i++) {
    buf.push_back((bits >> (i * 8)) & 0xFF);
  }
}