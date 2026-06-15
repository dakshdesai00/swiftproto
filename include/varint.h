#pragma once
#include <cstdint>
#include <vector>

void encode_varint(uint64_t value, std::vector<uint8_t> &buf);
uint64_t decode_varint(const std::vector<uint8_t> &buf, size_t &pos);
void encode_signed(int64_t value, std::vector<uint8_t> &buf);
int64_t decode_signed(const std::vector<uint8_t> &buf, size_t &pos);
uint64_t encode_zigzag(int64_t value);
int64_t decode_zigzag(uint64_t value);