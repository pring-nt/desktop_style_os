# Vendored third-party code

Files here are kept unmodified. Regenerate or re-download them with the commands below rather than editing them.

| Library   | Version                                                  | Source                                                                                         | License                                                     |
| --------- | -------------------------------------------------------- | ---------------------------------------------------------------------------------------------- | ----------------------------------------------------------- |
| glad      | glad2 generator 2.0.8, OpenGL 3.3 core, no extensions    | `uvx --from glad2==2.0.8 glad --api gl:core=3.3 --extensions="" --out-path third_party/glad c` | `(WTFPL OR CC0-1.0) AND Apache-2.0` (SPDX header in `gl.c`) |
| stb_image | v2.30, commit `2c980bb59875b0d32144a71867fbdebb2f77cd20` | `https://raw.githubusercontent.com/nothings/stb/<commit>/stb_image.h`                          | MIT or public domain, see `stb/LICENSE`                     |

GLFW, Dear ImGui and doctest are not vendored; `cmake/Dependencies.cmake` fetches them at pinned versions.
