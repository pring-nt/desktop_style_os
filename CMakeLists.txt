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
option(CSOPESY_IMGUI_DEMO "Show the Dear ImGui demo window (Debug builds only)" OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Dependencies)
include(Tooling)

add_library(
  csopesy_core STATIC
  src/apps/app_window.cc
  src/core/app.cc
  src/core/clock.cc
  src/core/state_machine.cc
  src/core/theme.cc
  src/shell/window_manager.cc)
target_include_directories(csopesy_core PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(csopesy_core PUBLIC imgui PRIVATE glad glfw)
target_compile_definitions(
  csopesy_core PRIVATE $<$<AND:$<BOOL:${CSOPESY_IMGUI_DEMO}>,$<CONFIG:Debug>>:CSOPESY_SHOW_IMGUI_DEMO>)
csopesy_set_warnings(csopesy_core)
csopesy_enable_sanitizers(csopesy_core)

add_executable(csopesy_os src/main.cc)
target_link_libraries(csopesy_os PRIVATE csopesy_core)
csopesy_set_warnings(csopesy_os)
csopesy_enable_sanitizers(csopesy_os)
csopesy_use_portable_runtime(csopesy_os)

if(EXISTS "${PROJECT_SOURCE_DIR}/assets")
  add_custom_command(
    TARGET csopesy_os
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:csopesy_os>/assets"
    VERBATIM)
endif()

if(CSOPESY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
