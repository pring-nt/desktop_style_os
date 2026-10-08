cmake_minimum_required(VERSION 3.24)

project(
  csopesy_os
  VERSION 1.0.0
  DESCRIPTION "CSOPESY Desktop OS Emulator"
  LANGUAGES C CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Use a preset instead, e.g. `cmake --preset debug`.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CSOPESY_WARNINGS_AS_ERRORS "Treat compiler warnings in our code as errors" ON)
option(CSOPESY_ENABLE_SANITIZERS "Build our code with ASan and UBSan (GCC/Clang outside Windows)" OFF)
option(CSOPESY_BUILD_TESTS "Build the doctest unit tests" ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Dependencies)
include(Tooling)
