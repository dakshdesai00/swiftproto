#include "../include/varint.h"
#include <cassert>
#include <iostream>

void test_unsigned(uint64_t value, size_t expected_bytes) {
  std::vector<uint8_t> buf;
  encode_varint(value, buf);

  size_t pos = 0;
  uint64_t decoded = decode_varint(buf, pos);

  assert(decoded == value);
  assert(buf.size() == expected_bytes);
  std::cout << "PASS unsigned: " << value << " → " << buf.size() << " bytes\n";
}

void test_signed(int64_t value, size_t expected_bytes) {
  std::vector<uint8_t> buf;
  encode_signed(value, buf);

  size_t pos = 0;
  int64_t decoded = decode_signed(buf, pos);

  assert(decoded == value);
  assert(buf.size() == expected_bytes);
  std::cout << "PASS signed:   " << value << " → " << buf.size() << " bytes\n";
}

int main() {
  std::cout << "=== Unsigned varint ===\n";
  test_unsigned(0, 1);
  test_unsigned(5, 1);
  test_unsigned(127, 1);
  test_unsigned(128, 2);
  test_unsigned(300, 2);
  test_unsigned(16383, 2);
  test_unsigned(16384, 3);
  test_unsigned(1234567890, 5);

  std::cout << "\n=== Signed zigzag ===\n";
  test_signed(0, 1);
  test_signed(1, 1);
  test_signed(-1, 1);
  test_signed(2, 1);
  test_signed(-2, 1);
  test_signed(63, 1);
  test_signed(-64, 1);
  test_signed(64, 2);
  test_signed(-65, 2);
  test_signed(-1234567890, 5);

  std::cout << "\nAll tests passed\n";
  return 0;
}