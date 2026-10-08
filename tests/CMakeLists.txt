add_executable(csopesy_tests clock_test.cc smoke_test.cc state_machine_test.cc)
target_link_libraries(csopesy_tests PRIVATE csopesy_core doctest::doctest_with_main)
csopesy_set_warnings(csopesy_tests)
csopesy_enable_sanitizers(csopesy_tests)
csopesy_use_portable_runtime(csopesy_tests)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(csopesy_tests)
