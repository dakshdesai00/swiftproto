# SwiftProto: High-Performance C++17 Binary & JSON Serializer

SwiftProto is an ultra-fast, lightweight C++17 binary serialization library and schema compiler. It is designed to act as a high-performance alternative to Google Protocol Buffers and standard JSON serialization, achieving up to **8.4x speedups over JSON** and **42% speedups over Google Protobuf** through a zero-allocation, zero-copy architecture.

---

## Key Features

1. **Zero-Copy String Deserialization**: String fields use C++17 `std::string_view` to point directly into the binary serialization buffer or JSON nodes. No memory allocation or copying occurs.
2. **Schema Compiler in C++**: A standalone C++ executable compiles standard `.proto` schema files directly into clean, optimized C++ structs with zero virtual table or runtime reflection overhead.
3. **Seamless JSON Integration**: Built-in support for `nlohmann/json`, allowing automated conversion: `JSON ➔ C++ Struct ➔ Binary ➔ C++ Struct ➔ JSON`.
4. **Nesting & Repeated Fields**: Supports nested message structs and repeated arrays (`std::vector`).
5. **Schema Evolution**: Supports forward and backward compatibility via automated field-skipping.

---

## 1. Architecture & Wire Format

SwiftProto utilizes a Google Protobuf-compatible binary layout. Each field in the binary payload is serialized as a **Tag-Length-Value (TLV)** block.

### The Tag Layout
Every field starts with a key tag encoded as a **Varint** of the value:
$$\text{Tag} = (\text{Field Number} \ll 3) \mid \text{Wire Type}$$

The lower 3 bits indicate the encoding type (`WireType`):
*   `0` (VARINT): Varint integers, booleans, and signed integers.
*   `1` (FIXED64): 64-bit raw bytes (fixed-size integers, doubles).
*   `2` (LENGTH_DELIMITED): Length prefix followed by bytes (strings, sub-messages, repeated fields).
*   `5` (FIXED32): 32-bit raw bytes (floats, 32-bit integers).

---

### Data Encoding Specifications

#### A. Varint (Wire Type 0)
Used for `int64`, `bool`, and `enum` types.
*   **Unsigned Varints**: Integers are split into 7-bit chunks. Each byte uses the Most Significant Bit (MSB) as a "continuation bit" (1 if there are more bytes, 0 if it is the last byte).
*   **Signed Integers (ZigZag)**: To prevent negative numbers from taking a constant 10 bytes, signed integers are mapped to unsigned integers using ZigZag encoding:
    $$\text{ZigZag}(v) = (v \ll 1) \oplus (v \gg 63)$$
    This maps small negative numbers to small positive numbers (e.g., $-1 \to 1$, $1 \to 2$, $-2 \to 3$), which Varint encodes in just 1 byte.

#### B. Length-Delimited (Wire Type 2)
Used for strings, nested messages, and arrays.
*   **Format**: `[Varint Length] [Raw Payload Bytes...]`
*   **Nested Messages**: The sub-message is fully serialized to a temporary buffer first, and then written as a Length-Delimited payload inside the parent buffer.

#### C. Fixed32 (Wire Type 5)
Used for `float` types.
*   **Format**: Exactly 4 bytes. Floats are serialized using their raw binary IEEE-754 representation in little-endian order.

#### D. Repeated Fields (Arrays)
*   **Format**: Represented as `std::vector<T>`.
*   To serialize repeated fields, SwiftProto writes individual fields back-to-back using the same tag. During deserialization, every encounter of the tag appends the parsed value to the vector.

---

## 2. Optimization Strategies

### Zero-Copy Deserialization
Standard deserializers copy string characters from the binary stream into a new heap-allocated `std::string` object. Heap allocations are extremely slow.

SwiftProto avoids this completely:
*   Its deserializer returns a `std::string_view` which holds a pointer directly to the input binary vector buffer and a length.
*   No heap allocations occur during string parsing.
*   *Requirement*: The input binary buffer must outlive the deserialized C++ structures.

### Zero-Copy JSON Mapping
Through custom `nlohmann::json` bindings, SwiftProto maps JSON strings directly to `std::string_view` fields:
*   Instead of copying JSON nodes to temporary variables, it extracts references directly from the internal JSON string nodes via `json::get_ref<const std::string&>()`.
*   This makes JSON translation exceptionally fast and memory-efficient.

---

## 3. Performance & Benchmarks

The benchmark suite processes **1,000,000 records** containing integers, floats, booleans, nested structs, and repeated arrays. 

### Benchmark Output

| Library / Parser | Time for 1M Records (ms) | Throughput (Records/sec) | Speedup vs Standard JSON |
| :--- | :--- | :--- | :--- |
| **SwiftProto (Binary)** | **1148.43 ms** | **870,752 records/sec** | **~8.4x faster** |
| Google Protobuf (Binary) | 1635.20 ms | 611,547 records/sec | ~5.9x faster |
| Standard JSON (`nlohmann/json`) | 9632.71 ms | 103,813 records/sec | Baseline (1.0x) |
| SwiftProto (Full JSON Roundtrip) | 12302.50 ms | 81,284 records/sec | 0.78x |

### Why is SwiftProto faster than Google Protobuf?
1. **No Virtual Tables**: SwiftProto messages are simple, contiguous C++ structs without virtual methods or inheritance overhead.
2. **True Zero-Copy**: SwiftProto uses `std::string_view` to point directly to the source buffer, whereas Protobuf copies strings into internal fields unless using advanced Arena allocators.
3. **Inlined Varints**: Low-level bit manipulation functions are highly optimized and fully inlined by the compiler.

### Trade-offs: SwiftProto vs. Google Protobuf
While SwiftProto is faster in this benchmark, it makes specific trade-offs:
*   **Memory Lifetime Management (Safety vs. Speed)**: SwiftProto uses `std::string_view` to point directly into the binary buffer for zero-copy deserialization. If the buffer is modified or goes out of scope, the deserialized struct contains dangling pointers. Google Protobuf copies values to owned `std::string` fields to ensure complete memory safety.
*   **Multi-Language Ecosystems**: Google Protobuf compiles schemas to C++, Java, Go, Python, Rust, Swift, and more. SwiftProto is built exclusively for C++17.
*   **Feature Completeness**: Google Protobuf supports maps, unions (`oneof`), and gRPC service definitions. SwiftProto is a minimal serializer optimized for performance.
*   **Target Domain**: SwiftProto is ideal for ultra-low latency domains (like High-Frequency Trading or game engine networking) where lifetimes are strictly controlled, whereas Protobuf is used for standard microservice communications.

---

## 4. Usage & Project Structure

### Repository Layout
*   `include/`: Header files for varint, encoder, decoder, shared types, and generated messages.
*   `src/`: Main implementation files and the C++ schema compiler code.
*   `tests/`: Complete test suites verifying varints, encoders, decoders, and JSON round-trips.
*   `benchmarks/`: Comparative benchmark suite.
*   `schema.proto`: Schema definition file.
*   `Makefile`: Automated build script.

### Getting Started

#### 1. Compile the Library and C++ Compiler
```bash
make
```
This builds:
*   `lib/libswiftproto.a` (The static library)
*   `bin/swiftproto-compiler` (The standalone schema compiler executable)
*   All test suites.

#### 2. Compile Your Custom Schema
Generate C++ structs from a `.proto` file:
```bash
./bin/swiftproto-compiler schema.proto include/generated_messages.h
```

#### 3. Run Test Suites
```bash
make test
```

#### 4. Run Performance Benchmarks
```bash
make bench && ./bin/bench
```
*(Note: Requires Google Protobuf to be installed on the system to run the Protobuf comparison. Installing via `brew install protobuf` will configure this automatically).*
