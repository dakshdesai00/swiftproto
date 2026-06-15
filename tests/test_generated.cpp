#include "../include/generated_messages.h"
#include <cassert>
#include <iostream>

// Generic helper function to serialize any message struct/class
template <typename T>
std::vector<uint8_t> serialize_message(const T &msg) {
  std::vector<uint8_t> buf;
  msg.serialize(buf);
  return buf;
}

// Generic helper function to deserialize any message struct/class
template <typename T>
T deserialize_message(const std::vector<uint8_t> &buf) {
  T msg;
  size_t pos = 0;
  msg.deserialize(buf, pos, buf.size());
  return msg;
}

int main() {
  Merchant original;
  original.id = 999;
  original.name = "SuperShop";
  original.fraud_rate = 0.007f;
  original.active = true;
  original.balance = -12345;
  original.address.city = "New York";
  original.tags = {10, 20, 30, 40};
  original.products = {"Apple", "Banana", "Cherry"};

  // Serialize
  std::vector<uint8_t> buf = serialize_message(original);
  std::cout << "Serialized generated Merchant size: " << buf.size() << " bytes\n";

  // Deserialize (Zero-copy, since name, city, products point to buf!)
  Merchant decoded = deserialize_message<Merchant>(buf);

  // Assertions
  assert(decoded.id == original.id);
  assert(decoded.name == original.name);
  assert(decoded.fraud_rate == original.fraud_rate);
  assert(decoded.active == original.active);
  assert(decoded.balance == original.balance);
  assert(decoded.address.city == original.address.city);
  
  // Assert repeated lists match
  assert(decoded.tags == original.tags);
  assert(decoded.products == original.products);

  // Check Zero-Copy verification:
  // The address of the string_view data should point inside the buffer vector.
  const char* buf_start = reinterpret_cast<const char*>(buf.data());
  const char* buf_end = buf_start + buf.size();

  assert(decoded.name.data() >= buf_start && decoded.name.data() < buf_end);
  assert(decoded.address.city.data() >= buf_start && decoded.address.city.data() < buf_end);
  for (const auto& product : decoded.products) {
    assert(product.data() >= buf_start && product.data() < buf_end);
  }

  std::cout << "All assertions passed successfully!\n";
  std::cout << "Decoded Merchant city: " << decoded.address.city << "\n";
  std::cout << "Decoded products size: " << decoded.products.size() << "\n";
  std::cout << "Zero-copy verification: PASSED (string_views point directly to buffer!)\n";

  return 0;
}
