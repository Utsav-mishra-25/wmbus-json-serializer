wM-Bus JSON Serializer Library

## Project Overview
Embedded-friendly C library for serializing structured meter data into predefined JSON format. Implements a transport-agnostic serialization layer suitable for wM-Bus/smart-meter gateway applications. Zero external dependencies, designed for constrained embedded environments.

## Platform & Language Choice
- Platform: STM32CubeIDE (STM32F407G-DISC1)
- Language: C (C11 standard)

# Justification:
 STM32 microcontrollers are industry-standard for industrial embedded systems with wide toolchain support. C provides direct memory control, minimal runtime overhead, and predictable execution - critical for embedded systems where resource constraints and determinism are paramount. Avoiding C++ eliminates runtime polymorphism overhead and reduces binary size.

### Build and Run Instructions

# Prerequisites
- CMake 3.10+
- GCC or ARM GCC toolchain
- (Optional) STM32CubeIDE for embedded deployment

# Building
bash
# Clone repository
git clone https://github.com/[username]/wmbus-json-serializer.git
cd wmbus-json-serializer

# Build
mkdir build && cd build
cmake ..
make