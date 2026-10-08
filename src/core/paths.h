#ifndef CSOPESY_SRC_CORE_PATHS_H_
#define CSOPESY_SRC_CORE_PATHS_H_

#include <filesystem>

namespace csopesy::core {

// The directory holding the running executable, so assets load the same way
// whatever the working directory is. Falls back to the working directory if
// the platform can't tell.
[[nodiscard]] std::filesystem::path ExecutableDirectory();

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_PATHS_H_
