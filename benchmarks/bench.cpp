#include "../include/generated_messages.h"
#include "../include/json.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#ifdef USE_PROTOBUF
#include "schema.pb.h"
#endif

using json = nlohmann::json;
using namespace std::chrono;

const int ITERATIONS = 1000000;

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

void bench_swiftproto_binary(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    // 1. Serialize
    std::vector<uint8_t> buf = serialize_message(m);

    // 2. Deserialize
    Merchant decoded = deserialize_message<Merchant>(buf);

    // Assert/Access to prevent optimization
    (void)decoded.id;
    (void)decoded.name;
    (void)decoded.fraud_rate;
    (void)decoded.active;
    (void)decoded.balance;
    (void)decoded.address.city;
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "SwiftProto (Binary Only): " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "SwiftProto Speed:         " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}

void bench_swiftproto_json_roundtrip(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    // 1. C++ Struct -> JSON
    json j = m;

    // 2. JSON -> C++ Struct
    Merchant m2 = j.get<Merchant>();

    // 3. C++ Struct -> Binary
    std::vector<uint8_t> buf = serialize_message(m2);

    // 4. Binary -> C++ Struct
    Merchant decoded = deserialize_message<Merchant>(buf);

    // 5. C++ Struct -> JSON
    json j2 = decoded;
    
    (void)j2;
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "SwiftProto (Full JSON Roundtrip): " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "SwiftProto JSON Speed:            " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}

void bench_json_raw(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    // 1. Dump to JSON string
    json j;
    j["id"] = m.id;
    j["name"] = std::string(m.name);
    j["fraud_rate"] = m.fraud_rate;
    j["active"] = m.active;
    j["balance"] = m.balance;
    j["address"] = {{"city", std::string(m.address.city)}};
    j["tags"] = m.tags;
    
    std::vector<std::string> products_str;
    for (auto sv : m.products) products_str.push_back(std::string(sv));
    j["products"] = products_str;

    std::string s = j.dump();

    // 2. Parse from JSON string
    json parsed = json::parse(s);
    
    int64_t id = parsed["id"];
    std::string name = parsed["name"];
    float fraud = parsed["fraud_rate"];
    bool active = parsed["active"];
    int64_t balance = parsed["balance"];
    std::string city = parsed["address"]["city"];
    std::vector<int64_t> tags = parsed["tags"];
    std::vector<std::string> products = parsed["products"];

    (void)id; (void)name; (void)fraud; (void)active; (void)balance; (void)city;
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "Standard JSON (Parse & Dump):     " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "Standard JSON Speed:              " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}

#ifdef USE_PROTOBUF
void bench_protobuf(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    // 1. Serialize
    proto::Address* addr = new proto::Address();
    addr->set_city(std::string(m.address.city));

    proto::Merchant proto;
    proto.set_id(m.id);
    proto.set_name(std::string(m.name));
    proto.set_fraud_rate(m.fraud_rate);
    proto.set_active(m.active);
    proto.set_balance(m.balance);
    proto.set_allocated_address(addr);
    for (auto t : m.tags) proto.add_tags(t);
    for (auto p : m.products) proto.add_products(std::string(p));

    std::string s;
    proto.SerializeToString(&s);

    // 2. Deserialize
    proto::Merchant decoded;
    decoded.ParseFromString(s);

    // Read values to verify it works and is not optimized out
    int64_t id = decoded.id();
    std::string name = decoded.name();
    float fraud = decoded.fraud_rate();
    bool active = decoded.active();
    int64_t balance = decoded.balance();
    std::string city = decoded.address().city();
    (void)id; (void)name; (void)fraud; (void)active; (void)balance; (void)city;
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "Google Protobuf (Binary Only):    " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "Google Protobuf Speed:            " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}
#endif

int main() {
  std::vector<Merchant> merchants;
  for (int i = 0; i < ITERATIONS; i++) {
    Merchant m;
    m.id = i;
    m.name = "ShopX";
    m.fraud_rate = 0.035f;
    m.active = true;
    m.balance = -99;
    m.address.city = "San Francisco";
    m.tags = {1, 2, 3, 4, 5};
    m.products = {"Apple", "Orange", "Banana"};
    merchants.push_back(m);
  }

  std::cout << "=== SwiftProto vs JSON vs Protobuf — " << ITERATIONS << " records ===\n\n";
  
  bench_json_raw(merchants);
  bench_swiftproto_json_roundtrip(merchants);
  bench_swiftproto_binary(merchants);

#ifdef USE_PROTOBUF
  bench_protobuf(merchants);
#else
  std::cout << "Google Protobuf bench skipped (re-run with USE_PROTOBUF=1 after compilation)\n\n";
#endif

  return 0;
}