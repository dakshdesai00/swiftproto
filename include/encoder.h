#pragma once
#include "varint.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "types.h"

void write_tag(uint32_t field_number, WireType wire_type,
               std::vector<uint8_t> &buf);
void write_int64(uint32_t field_number, int64_t value,
                 std::vector<uint8_t> &buf);
void write_string(uint32_t field_number, std::string_view value,
                  std::vector<uint8_t> &buf);
void write_float(uint32_t field_number, float value, std::vector<uint8_t> &buf);
void write_bool(uint32_t field_number, bool value, std::vector<uint8_t> &buf);