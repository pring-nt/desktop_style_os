#ifndef CSOPESY_SRC_CORE_PATHS_H_
#define CSOPESY_SRC_CORE_PATHS_H_

#include <filesystem>
#include <vector>

namespace csopesy::core {

// The directory holding the running executable, so assets load the same way
// whatever the working directory is. Falls back to the working directory if
// the platform can't tell.
[[nodiscard]] std::filesystem::path ExecutableDirectory();

// The whole file as bytes, or an empty vector if it can't be read.
[[nodiscard]] std::vector<unsigned char> ReadFileBytes(
    const std::filesystem::path& path);

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_PATHS_H_
