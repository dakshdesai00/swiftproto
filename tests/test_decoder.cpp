#include "../include/decoder.h"
#include "../include/encoder.h"
#include <cassert>
#include <iostream>

int main() {
  std::vector<uint8_t> buf;

  write_int64(1, 12345, buf);
  write_string(2, "Helloooo", buf);
  write_float(3, 0.035f, buf);
  write_bool(4, true, buf);
  write_int64(5, -99, buf);

  std::cout << "Encoded " << buf.size() << " bytes\n";

  size_t pos = 0;

  Tag t1 = read_tag(buf, pos);
  assert(t1.field_number == 1);
  assert(t1.wire_type == VARINT);
  int64_t id = read_int64(buf, pos);
  assert(id == 12345);

  Tag t2 = read_tag(buf, pos);
  assert(t2.field_number == 2);
  assert(t2.wire_type == LENGTH_DELIMITED);
  std::string name = read_string(buf, pos);
  assert(name == "Helloooo");

  Tag t3 = read_tag(buf, pos);
  assert(t3.field_number == 3);
  assert(t3.wire_type == FIXED32);
  float fraud = read_float(buf, pos);
  assert(fraud == 0.035f);

  Tag t4 = read_tag(buf, pos);
  assert(t4.field_number == 4);
  assert(t4.wire_type == VARINT);
  bool active = read_bool(buf, pos);
  assert(active == true);

  Tag t5 = read_tag(buf, pos);
  assert(t5.field_number == 5);
  assert(t5.wire_type == VARINT);
  int64_t balance = read_int64(buf, pos);
  assert(balance == -99);

  assert(pos == buf.size());

  std::cout << "id=" << id << " name=" << name << " fraud=" << fraud
            << " active=" << active << " balance=" << balance << "\n";
  std::cout << "All round-trip tests passed\n";
  return 0;
}