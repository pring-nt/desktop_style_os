#include "core/paths.h"

#include <filesystem>

#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

TEST_CASE("ExecutableDirectory is an existing directory") {
  const std::filesystem::path directory = ExecutableDirectory();
  CHECK_FALSE(directory.empty());
  CHECK(std::filesystem::is_directory(directory));
}

}  // namespace
}  // namespace csopesy::core
