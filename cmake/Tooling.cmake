# Custom targets for clang-format and clang-tidy: `format`, `format-check` and
# `tidy`. They cover our code in src/ and tests/ only.

find_program(CSOPESY_CLANG_FORMAT clang-format HINTS "C:/Program Files/LLVM/bin")
find_program(CSOPESY_CLANG_TIDY clang-tidy HINTS "C:/Program Files/LLVM/bin")

file(
  GLOB_RECURSE csopesy_format_files CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.cc"
  "${PROJECT_SOURCE_DIR}/tests/*.h"
  "${PROJECT_SOURCE_DIR}/tests/*.cc")
list(SORT csopesy_format_files)
set(csopesy_tidy_files ${csopesy_format_files})
list(FILTER csopesy_tidy_files INCLUDE REGEX "\\.cc$")

# Adds a target that runs `tool args... files...`, or fails with a clear
# message when the tool is missing. With no files it does nothing, because
# clang-format would otherwise wait on stdin.
function(csopesy_add_tool_target name tool files)
  if(NOT tool)
    add_custom_target(
      ${name}
      COMMAND ${CMAKE_COMMAND} -E echo "${name}: tool not found on PATH, see CONTRIBUTING.md"
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM)
  elseif(NOT files)
    add_custom_target(${name} COMMAND ${CMAKE_COMMAND} -E echo "${name}: no source files yet" VERBATIM)
  else()
    add_custom_target(
      ${name}
      COMMAND ${tool} ${ARGN} ${files}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      VERBATIM)
  endif()
endfunction()

csopesy_add_tool_target(format "${CSOPESY_CLANG_FORMAT}" "${csopesy_format_files}" -i)
csopesy_add_tool_target(format-check "${CSOPESY_CLANG_FORMAT}" "${csopesy_format_files}" --dry-run --Werror)
csopesy_add_tool_target(tidy "${CSOPESY_CLANG_TIDY}" "${csopesy_tidy_files}" -p ${PROJECT_BINARY_DIR} --quiet)
