CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BENCH_DIR = benchmarks
LIB_DIR = lib
BIN_DIR = bin
BUILD_DIR = build

# Library target
LIB_TARGET = $(LIB_DIR)/libswiftproto.a
LIB_SRCS = $(SRC_DIR)/varint.cpp $(SRC_DIR)/encoder.cpp $(SRC_DIR)/decoder.cpp
LIB_OBJS = $(BUILD_DIR)/varint.o $(BUILD_DIR)/encoder.o $(BUILD_DIR)/decoder.o

# Compiler target
COMPILER_TARGET = $(BIN_DIR)/swiftproto-compiler

# Tests
TESTS = $(BIN_DIR)/test_varint $(BIN_DIR)/test_encoder $(BIN_DIR)/test_decoder $(BIN_DIR)/test_generated $(BIN_DIR)/test_json_roundtrip

.PHONY: all clean test dirs run_tests compile_schema

all: dirs $(LIB_TARGET) $(COMPILER_TARGET) compile_schema $(TESTS)

dirs:
	mkdir -p $(LIB_DIR) $(BIN_DIR) $(BUILD_DIR)

# Build static library
$(LIB_TARGET): $(LIB_OBJS)
	ar rcs $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build SwiftProto compiler
$(COMPILER_TARGET): $(SRC_DIR)/compiler.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Generate messages header using SwiftProto compiler
compile_schema: $(COMPILER_TARGET) schema.proto
	$(COMPILER_TARGET) schema.proto $(INC_DIR)/generated_messages.h

# Compile tests
$(BIN_DIR)/test_varint: $(TEST_DIR)/test_varint.cpp $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) $< $(LIB_TARGET) -o $@

$(BIN_DIR)/test_encoder: $(TEST_DIR)/test_encoder.cpp $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) $< $(LIB_TARGET) -o $@

$(BIN_DIR)/test_decoder: $(TEST_DIR)/test_decoder.cpp $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) $< $(LIB_TARGET) -o $@

$(BIN_DIR)/test_generated: $(TEST_DIR)/test_generated.cpp $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) $< $(LIB_TARGET) -o $@

$(BIN_DIR)/test_json_roundtrip: $(TEST_DIR)/test_json_roundtrip.cpp $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) $< $(LIB_TARGET) -o $@

test: all run_tests

run_tests:
	$(BIN_DIR)/test_varint
	$(BIN_DIR)/test_encoder
	$(BIN_DIR)/test_decoder
	$(BIN_DIR)/test_generated
	$(BIN_DIR)/test_json_roundtrip

clean:
	rm -rf $(LIB_DIR) $(BIN_DIR) $(BUILD_DIR) $(INC_DIR)/generated_messages.h schema.pb.h schema.pb.cc include/schema.pb.h include/schema.pb.cc

bench: dirs compile_schema $(LIB_TARGET)
	/opt/homebrew/bin/protoc --cpp_out=include schema.proto
	$(CXX) $(CXXFLAGS) -DUSE_PROTOBUF benchmarks/bench.cpp include/schema.pb.cc $(LIB_TARGET) $(shell pkg-config --libs --cflags protobuf) -o $(BIN_DIR)/bench
