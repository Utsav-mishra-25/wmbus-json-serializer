cmake_minimum_required(VERSION 3.10)
project(wmbus_json VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Windows/MSVC uses different warning flags
if(MSVC)
    # Visual Studio flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
else()
    # GCC/Clang flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Library
add_library(wmbus_json STATIC src/wmbus_json.c)
target_include_directories(wmbus_json PUBLIC src)

# Demo
add_executable(demo examples/demo.c)
target_link_libraries(demo wmbus_json)

# Tests (optional)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_basic.c)
    add_executable(test_basic tests/test_basic.c)
    target_link_libraries(test_basic wmbus_json)
    enable_testing()
    add_test(NAME basic_tests COMMAND test_basic)
endif()