# 03 — Coding Standards

All C++ follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html), plus the project and Dear ImGui rules below. Wherever a rule can be checked by a tool, the tool enforces it (see [04-quality-gates.md](04-quality-gates.md)) rather than code review. Where this section is silent, the Google guide decides.

## Naming (Google style, enforced by clang-tidy `readability-identifier-naming`)

| Element                               | Style                           | Example                                |
| ------------------------------------- | ------------------------------- | -------------------------------------- |
| Namespaces                            | snake_case                      | `csopesy::shell`                       |
| Classes, structs, enums, type aliases | PascalCase                      | `WindowManager`, `enum class AppState` |
| Enum values                           | `k` + PascalCase                | `AppState::kDesktop`                   |
| Functions, methods                    | PascalCase                      | `DrawTaskbar()`                        |
| Accessors / mutators                  | snake_case, matching the member | `is_open()`, `set_is_open()`           |
| Local variables, parameters           | snake_case                      | `frame_time`                           |
| Class data members                    | snake_case + trailing `_`       | `is_open_`                             |
| Struct data members                   | snake_case, no trailing `_`     | `cpu_percent`                          |
| Constants, `constexpr`                | `k` + PascalCase                | `kTaskbarHeight`                       |
| Macros (avoid)                        | UPPER_SNAKE with project prefix | `CSOPESY_DEBUG`                        |
| Files                                 | snake_case, `.h` / `.cc`        | `task_manager.h`, `task_manager.cc`    |

Dear ImGui's own API is already PascalCase (`ImGui::Begin`), so our function names read consistently next to it.

## Google style rules we rely on most

- `#define` header guards, not `#pragma once`, in the form `CSOPESY_SRC_<DIR>_<FILE>_H_` (e.g. `CSOPESY_SRC_CORE_APP_H_`); clang-tidy `llvm-header-guard` is configured to that pattern.
- Include order, set by clang-format: related header, C system headers, C++ standard headers, other libraries (GLFW, ImGui, glad), project headers; each group separated by a blank line. Include what you use; no forward declarations of types from other libraries.
- Everything inside `namespace csopesy { ... }` (with sub-namespaces per folder); no `using namespace` directives anywhere; unnamed namespaces for file-local helpers in `.cc` files.
- No exceptions thrown by our code: errors are returned (`bool`, `std::optional`, or a small result struct) and checked at startup (GLFW, GL loader, texture load).
- No run-time type info (`dynamic_cast`, `typeid`) in our code; virtual dispatch through `AppWindow` instead.
- `explicit` on single-argument constructors; copy/move operations declared explicitly (`= default` / `= delete`) on every class that owns a resource.
- 2-space indent, 80-column limit, `char* p` pointer alignment: all applied by clang-format, never by hand.
- `auto` only when the type is obvious from the line or truly noisy (iterators, lambdas).
- Comments: Google-style file and class comments on public headers; inside functions, comments only to explain _why_. No commented-out code.

## Project C++ rules (on top of Google)

- RAII for every resource: the GLFW window, GL textures and ImGui context are owned by classes whose destructors release them. No raw `new` / `delete`; `std::unique_ptr` owns, references or plain pointers borrow.
- `const` everywhere it fits, `[[nodiscard]]` on functions whose result must be used, `enum class` only, `std::string_view` for read-only string parameters.
- No global mutable state; no singletons.
- No magic numbers: sizes, colors and timings live in `Theme` or named `constexpr` values.
- C++ casts only (`static_cast`, etc.); no C-style casts.
- `core/Clock` formatting, `data/` and `StateMachine` logic never include `imgui.h` or GL headers, so they can be unit-tested without a window.

## Dear ImGui rules

- `Begin()` / `BeginChild()` → always call the matching `End()` / `EndChild()`, whatever `Begin` returned.
- `BeginTable`, `BeginPopup`, `BeginPopupModal`, `BeginTabBar`, `BeginMenu` → call the matching `End*` **only** if `Begin*` returned true.
- Every `PushStyleColor` / `PushStyleVar` / `PushID` / `PushFont` is popped in the same function, same count.
- IDs: unique labels; `##suffix` to hide an ID (`"##terminal-input"`), `###id` when the visible text changes but identity must not (window titles with counters); `PushID(i)` inside loops.
- Immediate mode means no UI state in ImGui: window open/minimized flags, selections and input buffers live in our classes, never in `static` locals inside draw functions.
- Draw functions draw and report actions; they do not own app logic. Example: the taskbar returns "Task Manager clicked" and `WindowManager` decides what that means.
- Fixed panels (desktop, taskbar, clock) are placed every frame with `SetNextWindowPos/Size` from `GetMainViewport()->WorkPos/WorkSize`.
- Window flag combinations are named `constexpr ImGuiWindowFlags` values, not repeated inline.
- All styling goes through `Theme` (applied once at startup with `ImGui::GetStyle()`); no ad-hoc colors inside features.
- Define `IMGUI_DISABLE_OBSOLETE_FUNCTIONS`; `ShowDemoWindow` only in debug builds behind a flag. Debug builds keep `IM_ASSERT` on, so unbalanced Begin/End or Push/Pop fails loudly during development.
