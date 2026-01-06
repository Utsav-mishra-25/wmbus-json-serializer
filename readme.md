# wM-Bus JSON Serializer Library

## Project Overview
An embedded-friendly C library for serializing structured meter data into a predefined JSON format. Designed specifically for wM-Bus/smart-meter gateway applications on resource-constrained microcontrollers. This implementation follows the exact specification provided in the assignment, with manual JSON generation and zero external dependencies.

## Chosen Platform and Programming Language
- **Platform**: STM32CubeIDE (STM32F4 Discovery series)
- **Language**: C (C11 standard)

**Justification**: STM32 microcontrollers are the industry standard for industrial embedded systems with extensive toolchain support. C provides direct memory control, minimal runtime overhead, and predictable execution - critical for embedded systems where resource constraints and determinism are paramount. Avoiding C++ eliminates runtime polymorphism overhead and reduces binary size.

## Build and Run Instructions

### Building with CMake
```bash
git clone https://github.com/[username]/wmbus-json-serializer.git
cd wmbus-json-serializer
mkdir build && cd build
cmake ..
make
```

### Running Examples
```bash
./demo          # Example application with sample data
./test_basic    # Run test suite
```

### Manual Compilation
```bash
gcc -std=c11 -I./src src/wmbus_json.c examples/demo.c -o demo
```

### STM32 Deployment
1. Import project into STM32CubeIDE
2. Set ARM GCC toolchain
3. Build and flash to STM32F4 Discovery board
4. Monitor output via UART (115200 baud)

## Description of the Public API

### Data Structures
```c
typedef struct {
    char timestamp[17];         // "YYYY-MM-DD HH:MM"
    char meter_datetime[17];    // Meter timestamp
    float total_value;          // Cumulative value
    char status[8];             // "OK", "ERROR"
} DataPoint;

typedef struct {
    char media[16];             // "water", "electricity"
    char meter[32];             // Meter model
    char device_id[64];         // Unique identifier
    char unit[8];               // "m3", "kWh"
    DataPoint* data;            // Array of data points
    uint8_t data_count;         // Number of data points
} DeviceReading;

typedef struct {
    char gateway_id[32];        // Gateway identifier
    char date[11];              // "YYYY-MM-DD"
    char device_type[32];       // "stromleser"
    uint16_t interval_minutes;  // Reading interval
    uint32_t total_readings;    // Total data points
    uint8_t device_count;       // Number of devices
    DeviceReading* readings;    // Array of readings
} GatewayData;
```

### Core Functions

#### `size_t calculate_max_json_size(uint8_t device_count, uint32_t total_readings)`
Calculates maximum buffer size needed for JSON output.

#### `bool validate_gateway_data(const GatewayData* data)`
Validates input data structure integrity.

#### `int serialize_to_json(const GatewayData* data, char* buffer, size_t buffer_size)`
Serializes data to JSON format. Returns bytes written or error code.

### Error Codes
- `> 0`: Success (bytes written)
- `-1`: Buffer too small
- `-2`: Invalid input data
- `-3`: JSON generation error

## Example JSON Output
```json
[{
    "gatewayId": "gateway_1234",
    "date": "1970-01-01",
    "deviceType": "stromleser",
    "interval_minutes": 15,
    "total_readings": 1,
    "values": {
        "device_count": 1,
        "readings": [{
            "media": "water",
            "meter": "waterstarm",
            "deviceId": "stromleser_50898527",
            "unit": "m3",
            "data": [{
                "timestamp": "1970-01-01 00:00",
                "meter_datetime": "1970-01-01 00:00",
                "total_m3": 107.752,
                "status": "OK"
            }]
        }]
    }
}]
```

## Design Decisions and Assumptions

### Manual JSON Serialization
Implemented custom JSON generation instead of using external libraries. This eliminates dependencies, reduces code size (~4KB vs ~50KB), provides complete memory control, and ensures exact format compliance.

### Fixed Buffer API
Caller provides output buffer with size validation. This prevents heap fragmentation, allows stack/static allocation, enables predictable memory usage, and simplifies RTOS integration.

### Error-First Design
All functions validate inputs before processing with clear error codes. This prevents partial failures, supports defensive programming, and enables early error detection.

### Transport-Agnostic Core
Library only handles serialization, not data transmission. This follows single-responsibility principle, allows protocol flexibility, and simplifies testing.

### Static Configuration
Compile-time constants for all string sizes. This eliminates dynamic allocation, provides predictable memory footprint, and enables worst-case analysis.

### Assumptions
1. Maximum payload sizes known at compile time
2. System has sufficient stack space for buffers
3. Floating-point support available (can be replaced)
4. Character encoding is ASCII/UTF-8 compatible
5. Standard C library available (string.h, stdio.h)

## Notes on Possible Extensions

### Encryption
```c
int serialize_and_encrypt(const GatewayData* data, char* buffer, size_t size, const uint8_t* key);
// AES-128 encryption for sensitive meter data
```

### Streaming API
```c
typedef struct {
    char* buffer;
    size_t pos;
    size_t size;
} JsonWriter;

void json_writer_init(JsonWriter* writer, char* buffer, size_t size);
int json_write_gateway(JsonWriter* writer, const GatewayData* data);
// For memory-constrained systems or large datasets
```

### Binary Protocol Buffers
```c
int serialize_to_protobuf(const GatewayData* data, uint8_t* buffer, size_t size);
// Alternative to JSON for bandwidth-constrained links
```

### Security Extensions
```c
int serialize_and_sign(const GatewayData* data, char* buffer, size_t size, const uint8_t* key);
// Add HMAC or digital signatures for data integrity
```

### Unit Conversion
```c
void set_output_unit(const char* unit);
// Convert between units (m³ → liters, kWh → MWh)
```

### Timezone Support
```c
void set_timezone_offset(int offset_minutes);
// Handle timezone conversions for distributed systems
```

### RTOS Integration
```c
int serialize_to_json_rtos(const GatewayData* data, char* buffer, size_t size, SemaphoreHandle_t mutex);
// Thread-safe version with mutex protection
```

### Configuration File Support
```c
int load_config(const char* config_json);
// Load field mappings and validation rules from JSON config
```

These extensions could be implemented based on specific application requirements while maintaining the core design principles of memory safety, determinism, and embedded suitability.
