#include "core/paths.h"

#include <cstddef>
#include <filesystem>
#include <system_error>
#include <vector>

#ifdef _WIN32
#include <windows.h>  // IWYU pragma: keep
#include <libloaderapi.h>
#include <minwindef.h>
#elif defined(__APPLE__)
#include <cstdint>

#include <mach-o/dyld.h>
#endif

namespace csopesy::core {

namespace {

[[nodiscard]] std::filesystem::path ExecutablePath() {
#ifdef _WIN32
  constexpr std::size_t kInitialLength = 260;
  constexpr int kMaxAttempts = 8;
  std::vector<wchar_t> buffer(kInitialLength);
  for (int attempt = 0; attempt < kMaxAttempts; ++attempt) {
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(),
                                            static_cast<DWORD>(buffer.size()));
    if (length == 0) {
      return {};
    }
    if (length < buffer.size()) {
      return {buffer.data()};
    }
    buffer.resize(buffer.size() * 2);
  }
  return {};
#elif defined(__APPLE__)
  std::uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  std::vector<char> buffer(size);
  if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
    return {};
  }
  return {buffer.data()};
#else
  std::error_code error;
  std::filesystem::path path =
      std::filesystem::read_symlink("/proc/self/exe", error);
  return error ? std::filesystem::path{} : path;
#endif
}

}  // namespace

std::filesystem::path ExecutableDirectory() {
  const std::filesystem::path executable = ExecutablePath();
  if (!executable.empty()) {
    return executable.parent_path();
  }
  std::error_code error;
  const std::filesystem::path working_directory =
      std::filesystem::current_path(error);
  return error ? std::filesystem::path{} : working_directory;
}

}  // namespace csopesy::core
