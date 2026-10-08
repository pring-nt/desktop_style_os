#!/usr/bin/env bash
# Runs every quality gate in a fixed order and stops at the first failure.
# Gates are described in docs/spec/04-quality-gates.md. On Windows PowerShell
# use scripts/check.ps1, which runs the same gates.

set -euo pipefail

readonly LLVM_MAJOR="23"
readonly PRETTIER_VERSION="3.9.9"
readonly PRESET="ci"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

gate() { printf '\n==> Gate %s: %s\n' "$1" "$2"; }
fail() {
  printf 'FAIL: %s\n' "$*" >&2
  exit 1
}

# Tracked and new (not ignored) files matching the pathspecs, sorted, never
# from third_party/ or build/.
list_files() {
  git ls-files --cached --others --exclude-standard -- "$@" |
    awk '!/^(third_party|build)\//' | LC_ALL=C sort -u |
    while IFS= read -r file; do
      if [[ -f "${file}" ]]; then
        printf '%s\n' "${file}"
      fi
    done
}

require_tool() {
  command -v "$1" >/dev/null 2>&1 ||
    fail "$1 not found on PATH. See CONTRIBUTING.md (Tool setup)."
}

llvm_major() {
  "$1" --version | sed -n 's/.*version \([0-9][0-9]*\)\..*/\1/p' | head -n 1
}

mapfile -t cpp_files < <(list_files 'src/*.h' 'src/*.cc' 'tests/*.h' 'tests/*.cc')
mapfile -t tidy_files < <(printf '%s\n' "${cpp_files[@]}" | awk '/\.cc$/')
mapfile -t md_files < <(list_files 'docs/*.md')
mapfile -t header_files < <(list_files 'src/*.h')
mapfile -t logic_files < <(list_files 'src/data/*' 'src/core/clock.*' 'src/core/state_machine.*')

gate 1 "Tool versions"
for tool in clang-format clang-tidy; do
  require_tool "${tool}"
  major="$(llvm_major "${tool}")"
  [[ "${major}" == "${LLVM_MAJOR}" ]] ||
    fail "${tool} major version is ${major:-unknown}, expected ${LLVM_MAJOR}. See CONTRIBUTING.md."
  echo "${tool} ${major} OK"
done
for tool in cmake ninja bun; do
  require_tool "${tool}"
done
prettier_version="$(bunx "prettier@${PRETTIER_VERSION}" --version | tr -d '\r')"
[[ "${prettier_version}" == "${PRETTIER_VERSION}" ]] ||
  fail "prettier is ${prettier_version:-unknown}, expected ${PRETTIER_VERSION}."
echo "prettier ${prettier_version} OK"

gate 2 "C++ formatting"
if ((${#cpp_files[@]})); then
  clang-format --dry-run --Werror "${cpp_files[@]}" ||
    fail "C++ files are not formatted. Run: cmake --build --preset ${PRESET} --target format"
fi
echo "${#cpp_files[@]} files OK"

gate 3 "Markdown formatting"
if ((${#md_files[@]})); then
  bunx "prettier@${PRETTIER_VERSION}" --check "${md_files[@]}" ||
    fail "Markdown is not formatted. Run: bunx prettier@${PRETTIER_VERSION} --write \"docs/**/*.md\""
fi

gate 4 "Configure + build"
cmake --preset "${PRESET}" || fail "CMake configure failed."
cmake --build --preset "${PRESET}" || fail "Build failed."

gate 5 "Unit tests"
ctest --preset "${PRESET}" --output-on-failure || fail "Unit tests failed."

gate 6 "Static analysis"
if ((${#tidy_files[@]})); then
  clang-tidy -p "build/${PRESET}" --quiet "${tidy_files[@]}" ||
    fail "clang-tidy reported problems."
fi
echo "${#tidy_files[@]} files OK"

gate 7 "Layering and header guards"
problems=0
for file in "${logic_files[@]}"; do
  if grep -nE '^[[:space:]]*#[[:space:]]*include[[:space:]]*[<"](imgui|backends/|GLFW/|glad/|GL/|KHR/)' "${file}"; then
    printf '%s: UI or GL header included in testable logic\n' "${file}" >&2
    problems=1
  fi
done
for file in "${header_files[@]}"; do
  stem="${file%.h}"
  guard="CSOPESY_$(printf '%s' "${stem}" | tr '[:lower:]/.-' '[:upper:]___')_H_"
  content="$(tr -d '\r' <"${file}")"
  if ! grep -qx "#ifndef ${guard}" <<<"${content}" ||
    ! grep -qx "#define ${guard}" <<<"${content}" ||
    ! grep -qx "#endif  // ${guard}" <<<"${content}"; then
    printf '%s: expected header guard %s (#ifndef, #define, #endif  // %s)\n' \
      "${file}" "${guard}" "${guard}" >&2
    problems=1
  fi
done
((problems == 0)) || fail "Layering or header guard problems found."
echo "${#logic_files[@]} logic files, ${#header_files[@]} headers OK"

printf '\nAll gates passed.\n'
