#include "../include/varint.h"
#include <cstdint>

void encode_varint(uint64_t value, std::vector<uint8_t> &buf) {
  while (value >= 0b10000000) {
    buf.push_back((value & 0b01111111) | 0b10000000);
    value >>= 7;
  }
  buf.push_back((value));
}

uint64_t decode_varint(const std::vector<uint8_t> &buf, size_t &pos) {
  uint64_t value = 0;
  uint64_t shift = 0;
  while (pos < buf.size()) {
    uint8_t byte = buf[pos++];
    value |= (uint64_t)(byte & 0b01111111) << shift;
    shift += 7;
    if (!(byte & 0b10000000)) {
      break;
    }
  }
  return value;
}

uint64_t encode_zigzag(int64_t value) {
  return ((uint64_t)value << 1) ^ (uint64_t)(value >> 63);
}

int64_t decode_zigzag(uint64_t value) { return (value >> 1) ^ (-(value & 1)); }

void encode_signed(int64_t value, std::vector<uint8_t> &buf) {
  encode_varint(encode_zigzag(value), buf);
}

int64_t decode_signed(const std::vector<uint8_t> &buf, size_t &pos) {
  return decode_zigzag(decode_varint(buf, pos));
}