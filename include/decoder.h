#pragma once
#include "varint.h"
#include <cstdint>
#include <string>
#include <vector>
#include "types.h"

struct Tag {
  uint32_t field_number;
  WireType wire_type;
};

Tag read_tag(const std::vector<uint8_t> &buf, size_t &pos);
int64_t read_int64(const std::vector<uint8_t> &buf, size_t &pos);
std::string read_string(const std::vector<uint8_t> &buf, size_t &pos);
float read_float(const std::vector<uint8_t> &buf, size_t &pos);
bool read_bool(const std::vector<uint8_t> &buf, size_t &pos);