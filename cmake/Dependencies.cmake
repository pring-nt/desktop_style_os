# Third-party dependencies. Fetched ones are pinned to exact release archives
# with SHA-256 hashes; glad and stb are vendored unmodified in third_party/.
# Every include directory is SYSTEM so third-party warnings never fail our
# build or lint.

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
  URL_HASH SHA256=b5ec004b2712fd08e8861dc271428f048775200a2df719ccf575143ba749a3e9
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.tar.gz
  URL_HASH SHA256=21d8a0a565e85dce943e375db00812c2f3f0ab21f3f0f7964e364a63422d7f99
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_Declare(
  doctest
  URL https://github.com/doctest/doctest/archive/refs/tags/v2.5.3.tar.gz
  URL_HASH SHA256=174ebc4e769928959614789c5b4e9c3d0a0f81a62bb608756b127bfebfb21331
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_MakeAvailable(glfw imgui doctest)

# glad provides the GL declarations, so GLFW must not include the system GL
# header (clang-format sorts <GLFW/glfw3.h> before <glad/gl.h>).
target_compile_definitions(glfw INTERFACE GLFW_INCLUDE_NONE)
set_target_properties(
  glfw PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                  $<TARGET_PROPERTY:glfw,INTERFACE_INCLUDE_DIRECTORIES>)
set_target_properties(
  doctest PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                     $<TARGET_PROPERTY:doctest,INTERFACE_INCLUDE_DIRECTORIES>)

# Dear ImGui has no CMake file of its own.
add_library(
  imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui SYSTEM PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(imgui PUBLIC glfw)

add_library(glad STATIC ${PROJECT_SOURCE_DIR}/third_party/glad/src/gl.c)
target_include_directories(glad SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/third_party/glad/include)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/third_party/stb)
