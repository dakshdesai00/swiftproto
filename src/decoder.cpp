#include "../include/decoder.h"
#include "../include/varint.h"
#include <cstdint>
#include <cstring>

Tag read_tag(const std::vector<uint8_t> &buf, size_t &pos) {
  uint32_t tag_raw = decode_varint(buf, pos);
  return {tag_raw >> 3, (WireType)(tag_raw & 0x07)};
}

int64_t read_int64(const std::vector<uint8_t> &buf, size_t &pos) {
  return decode_signed(buf, pos);
}

std::string read_string(const std::vector<uint8_t> &buf, size_t &pos) {
  uint64_t len = decode_varint(buf, pos);
  std::string s(buf.begin() + pos, buf.begin() + pos + len);
  pos += len;
  return s;
}

float read_float(const std::vector<uint8_t> &buf, size_t &pos) {
  uint32_t bits = 0;
  for (int i = 0; i < 4; i++) {
    bits |= ((uint32_t)buf[pos++] << (i * 8));
  }
  float value;
  std::memcpy(&value, &bits, sizeof(float));
  return value;
}

bool read_bool(const std::vector<uint8_t> &buf, size_t &pos) {
  return decode_varint(buf, pos) != 0;
}