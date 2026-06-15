#include "../include/decoder.h"
#include "../include/encoder.h"
#include "../include/json.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std::chrono;

const int ITERATIONS = 1000000;

struct Merchant {
  int64_t id;
  std::string name;
  float fraud_rate;
  bool active;
  int64_t balance;
};

void bench_swiftproto(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    std::vector<uint8_t> buf;
    write_int64(1, m.id, buf);
    write_string(2, m.name, buf);
    write_float(3, m.fraud_rate, buf);
    write_bool(4, m.active, buf);
    write_int64(5, m.balance, buf);

    size_t pos = 0;
    read_tag(buf, pos);
    read_int64(buf, pos);
    read_tag(buf, pos);
    read_string(buf, pos);
    read_tag(buf, pos);
    read_float(buf, pos);
    read_tag(buf, pos);
    read_bool(buf, pos);
    read_tag(buf, pos);
    read_int64(buf, pos);
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "SwiftProto: " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "SwiftProto: " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}

void bench_json(const std::vector<Merchant> &merchants) {
  auto start = high_resolution_clock::now();

  for (const auto &m : merchants) {
    json j;
    j["id"] = m.id;
    j["name"] = m.name;
    j["fraud_rate"] = m.fraud_rate;
    j["active"] = m.active;
    j["balance"] = m.balance;
    std::string s = j.dump();

    json parsed = json::parse(s);
    int64_t id = parsed["id"];
    std::string name = parsed["name"];
    float fraud = parsed["fraud_rate"];
    bool active = parsed["active"];
    int64_t balance = parsed["balance"];
    (void)id;
    (void)name;
    (void)fraud;
    (void)active;
    (void)balance;
  }

  auto end = high_resolution_clock::now();
  double ms = duration_cast<microseconds>(end - start).count() / 1000.0;
  std::cout << "JSON:       " << ms << " ms for " << ITERATIONS << " records\n";
  std::cout << "JSON:       " << (ITERATIONS / (ms / 1000.0) / 1e6)
            << "M records/sec\n\n";
}

int main() {
  std::vector<Merchant> merchants;
  for (int i = 0; i < ITERATIONS; i++) {
    merchants.push_back({i, "ShopX", 0.035f, true, -99});
  }

  std::cout << "=== SwiftProto vs JSON — " << ITERATIONS << " records ===\n\n";
  bench_swiftproto(merchants);
  bench_json(merchants);

  return 0;
}