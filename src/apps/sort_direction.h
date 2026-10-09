#ifndef CSOPESY_SRC_APPS_SORT_DIRECTION_H_
#define CSOPESY_SRC_APPS_SORT_DIRECTION_H_

#include <cstdint>

namespace csopesy::apps {

// The order of a sorted table column, shared by File Explorer and Task
// Manager.
enum class SortDirection : std::uint8_t { kAscending, kDescending };

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_SORT_DIRECTION_H_
