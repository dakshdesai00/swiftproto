#include "../include/encoder.h"
#include <cassert>
#include <iostream>

void print_buf(const std::vector<uint8_t> &buf) {
  for (uint8_t b : buf)
    printf("%02X ", b);
  printf("\n");
}

int main() {
  {
    std::vector<uint8_t> buf;
    write_int64(1, 300, buf);
    std::cout << "int64 300:   ";
    print_buf(buf);
    assert(!buf.empty());
  }
  {
    std::vector<uint8_t> buf;
    write_int64(1, -1, buf);
    std::cout << "int64 -1:    ";
    print_buf(buf);
    assert(!buf.empty());
  }
  {
    std::vector<uint8_t> buf;
    write_string(2, "Hello World", buf);
    std::cout << "string Hello World:";
    print_buf(buf);
    assert(!buf.empty());
  }
  {
    std::vector<uint8_t> buf;
    write_float(3, 0.03f, buf);
    std::cout << "float 0.03:  ";
    print_buf(buf);
    assert(buf.size() == 5);
  }
  {
    std::vector<uint8_t> buf;
    write_bool(4, true, buf);
    std::cout << "bool true:   ";
    print_buf(buf);
    assert(buf.size() == 2);
  }

  std::cout << "All encoder tests passed\n";
  return 0;
}