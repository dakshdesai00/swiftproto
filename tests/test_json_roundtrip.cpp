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
  std::cout << "=== JSON Round-Trip Test ===\n";

  // Create original JSON
  nlohmann::json original_json = {
    {"id", 777},
    {"name", "JSON_Shop"},
    {"fraud_rate", 0.042f},
    {"active", true},
    {"balance", -1000},
    {"address", {
      {"city", "Chicago"}
    }},
    {"tags", {1, 3, 5, 7, 9}},
    {"products", {"Keyboard", "Mouse", "Monitor"}}
  };

  std::cout << "Original JSON:\n" << original_json.dump(2) << "\n\n";

  // 1. Convert JSON to C++ Struct (Deserialization from JSON)
  Merchant merchant = original_json.get<Merchant>();

  assert(merchant.id == 777);
  assert(merchant.name == "JSON_Shop");
  assert(merchant.fraud_rate == 0.042f);
  assert(merchant.active == true);
  assert(merchant.balance == -1000);
  assert(merchant.address.city == "Chicago");
  assert(merchant.tags.size() == 5);
  assert(merchant.products.size() == 3);

  // 2. Convert C++ Struct to Serialized Binary Buffer
  std::vector<uint8_t> binary_buf = serialize_message(merchant);
  std::cout << "Serialized binary buffer size: " << binary_buf.size() << " bytes\n\n";

  // 3. Convert Serialized Binary Buffer back to C++ Struct
  Merchant decoded_merchant = deserialize_message<Merchant>(binary_buf);

  // 4. Convert C++ Struct back to JSON (Serialization to JSON)
  nlohmann::json reconstructed_json = decoded_merchant;

  std::cout << "Reconstructed JSON:\n" << reconstructed_json.dump(2) << "\n\n";

  // Assertions to verify the reconstruction is identical to the original JSON
  assert(reconstructed_json["id"] == original_json["id"]);
  assert(reconstructed_json["name"] == original_json["name"]);
  // floating-point comparison with tolerance
  assert(std::abs(reconstructed_json["fraud_rate"].get<float>() - original_json["fraud_rate"].get<float>()) < 0.0001f);
  assert(reconstructed_json["active"] == original_json["active"]);
  assert(reconstructed_json["balance"] == original_json["balance"]);
  assert(reconstructed_json["address"]["city"] == original_json["address"]["city"]);
  assert(reconstructed_json["tags"] == original_json["tags"]);
  assert(reconstructed_json["products"] == original_json["products"]);

  std::cout << "JSON Round-Trip (JSON -> C++ -> Binary -> C++ -> JSON): PASSED!\n";
  return 0;
}
