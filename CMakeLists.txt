cmake_minimum_required(VERSION 3.10)

# Set the project name
project(mkpass VERSION 1.1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define build types
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo ASAN)

# Define ASAN build type flags
if (MSVC)
    set(CMAKE_CXX_FLAGS_ASAN "/fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_ASAN "/fsanitize=address")
else()
    set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS_ASAN "-fsanitize=address")
endif()

# UBSan, TSan and Coverage are not available with MSVC
if (NOT MSVC)
    list(APPEND CMAKE_CONFIGURATION_TYPES UBSAN TSAN COVERAGE)

    set(CMAKE_CXX_FLAGS_UBSAN "-fsanitize=undefined -fno-sanitize-recover=all")
    set(CMAKE_EXE_LINKER_FLAGS_UBSAN "-fsanitize=undefined")

    set(CMAKE_CXX_FLAGS_TSAN "-fsanitize=thread")
    set(CMAKE_EXE_LINKER_FLAGS_TSAN "-fsanitize=thread")

    set(CMAKE_CXX_FLAGS_COVERAGE "--coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage")
endif()

# MSan is only available with Clang on Linux
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND CMAKE_CONFIGURATION_TYPES MSAN)

    set(CMAKE_CXX_FLAGS_MSAN "-fsanitize=memory -fsanitize-memory-track-origins")
    set(CMAKE_EXE_LINKER_FLAGS_MSAN "-fsanitize=memory")
endif()

# An unsupported type has no flags, so it would build without instrumentation
if (CMAKE_BUILD_TYPE MATCHES "^(UBSAN|TSAN|MSAN|COVERAGE)$"
    AND NOT CMAKE_BUILD_TYPE IN_LIST CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not supported by this compiler")
endif()

# Add the executable
add_executable(mkpass main.cc)

# Specify the warning flags for GCC/Clang
if (NOT MSVC)
    target_compile_options(mkpass PRIVATE -Wall -Wextra -pedantic)
endif()

# macOS specific settings
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_target_properties(mkpass PROPERTIES
        OSX_ARCHITECTURES "x86_64;arm64"
    )
endif()

# Tests
enable_testing()
add_test(NAME cli
    COMMAND ${CMAKE_COMMAND} -DMKPASS=$<TARGET_FILE:mkpass>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_test.cmake
)
