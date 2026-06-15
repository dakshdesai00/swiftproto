#pragma once
#include <cstdint>

enum WireType : uint8_t {
  VARINT = 0,
  FIXED64 = 1,
  LENGTH_DELIMITED = 2,
  FIXED32 = 5
};
