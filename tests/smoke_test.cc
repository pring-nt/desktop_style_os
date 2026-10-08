// Proves the doctest + CTest wiring works. Real unit tests start in Phase 2.

#include <doctest/doctest.h>

TEST_CASE("test harness runs") { CHECK(1 + 1 == 2); }
