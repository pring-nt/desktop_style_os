# Runs every quality gate in a fixed order and stops at the first failure.
# Gates are described in docs/spec/04-quality-gates.md. On macOS, Linux or Git
# Bash use scripts/check.sh, which runs the same gates.

Set-StrictMode -Version Latest
# Native tools report failure through $LASTEXITCODE; 'Stop' would also turn
# their normal stderr output into errors.
$ErrorActionPreference = 'Continue'

$LlvmMajor = '23'
$PrettierVersion = '3.9.9'
$Preset = 'ci'

Set-Location -LiteralPath (Join-Path $PSScriptRoot '..')

function Write-Gate([string]$Number, [string]$Name) {
  Write-Host ''
  Write-Host "==> Gate ${Number}: $Name"
}

function Fail([string]$Message) {
  Write-Host "FAIL: $Message" -ForegroundColor Red
  exit 1
}

# Tracked and new (not ignored) files matching the pathspecs, sorted, never
# from third_party/ or build/.
function Get-RepoFiles([string[]]$Pathspecs) {
  $files = & git ls-files --cached --others --exclude-standard '--' @Pathspecs
  if ($LASTEXITCODE -ne 0) { Fail 'git ls-files failed.' }
  return @($files |
      Where-Object { $_ -and $_ -notmatch '^(third_party|build)/' -and (Test-Path -LiteralPath $_ -PathType Leaf) } |
      Sort-Object -Unique -CaseSensitive)
}

function Assert-Tool([string]$Name) {
  if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
    Fail "$Name not found on PATH. See CONTRIBUTING.md (Tool setup)."
  }
}

function Get-LlvmMajor([string]$Tool) {
  $output = (& $Tool --version) -join "`n"
  if ($output -match 'version (\d+)\.') { return $Matches[1] }
  return ''
}

$cppFiles = @(Get-RepoFiles @('src/*.h', 'src/*.cc', 'tests/*.h', 'tests/*.cc'))
$tidyFiles = @($cppFiles | Where-Object { $_ -match '\.cc$' })
$mdFiles = @(Get-RepoFiles @('docs/*.md'))
$headerFiles = @(Get-RepoFiles @('src/*.h'))
$logicFiles = @(Get-RepoFiles @('src/data/*', 'src/core/clock.*', 'src/core/state_machine.*'))

Write-Gate 1 'Tool versions'
foreach ($tool in @('clang-format', 'clang-tidy')) {
  Assert-Tool $tool
  $major = Get-LlvmMajor $tool
  if ($major -ne $LlvmMajor) {
    Fail "$tool major version is '$major', expected $LlvmMajor. See CONTRIBUTING.md."
  }
  Write-Host "$tool $major OK"
}
foreach ($tool in @('cmake', 'ninja', 'bun')) { Assert-Tool $tool }
$prettierOutput = @(& bunx "prettier@$PrettierVersion" --version)
$prettierVersion = if ($prettierOutput.Count -gt 0) { "$($prettierOutput[-1])".Trim() } else { '' }
if ($prettierVersion -ne $PrettierVersion) {
  Fail "prettier is '$prettierVersion', expected $PrettierVersion."
}
Write-Host "prettier $prettierVersion OK"

Write-Gate 2 'C++ formatting'
if ($cppFiles.Count -gt 0) {
  & clang-format --dry-run --Werror @cppFiles
  if ($LASTEXITCODE -ne 0) {
    Fail "C++ files are not formatted. Run: cmake --build --preset $Preset --target format"
  }
}
Write-Host "$($cppFiles.Count) files OK"

Write-Gate 3 'Markdown formatting'
if ($mdFiles.Count -gt 0) {
  & bunx "prettier@$PrettierVersion" --check @mdFiles
  if ($LASTEXITCODE -ne 0) {
    Fail "Markdown is not formatted. Run: bunx prettier@$PrettierVersion --write `"docs/**/*.md`""
  }
}

Write-Gate 4 'Configure + build'
& cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { Fail 'CMake configure failed.' }
& cmake --build --preset $Preset
if ($LASTEXITCODE -ne 0) { Fail 'Build failed.' }

Write-Gate 5 'Unit tests'
& ctest --preset $Preset --output-on-failure
if ($LASTEXITCODE -ne 0) { Fail 'Unit tests failed.' }

Write-Gate 6 'Static analysis'
if ($tidyFiles.Count -gt 0) {
  & clang-tidy -p "build/$Preset" --quiet @tidyFiles
  if ($LASTEXITCODE -ne 0) { Fail 'clang-tidy reported problems.' }
}
Write-Host "$($tidyFiles.Count) files OK"

Write-Gate 7 'Layering and header guards'
$problems = $false
foreach ($file in $logicFiles) {
  $hits = @(Select-String -LiteralPath $file -Pattern '^\s*#\s*include\s*[<"](imgui|backends/|GLFW/|glad/|GL/|KHR/)')
  foreach ($hit in $hits) {
    Write-Host "${file}:$($hit.LineNumber): UI or GL header included in testable logic" -ForegroundColor Red
    $problems = $true
  }
}
foreach ($file in $headerFiles) {
  $guard = 'CSOPESY_' + (($file -replace '\.h$', '').ToUpperInvariant() -replace '[/.-]', '_') + '_H_'
  $lines = @(Get-Content -LiteralPath $file)
  $expected = @("#ifndef $guard", "#define $guard", "#endif  // $guard")
  foreach ($line in $expected) {
    if ($lines -cnotcontains $line) {
      Write-Host "${file}: expected header guard $guard (#ifndef, #define, #endif  // $guard)" -ForegroundColor Red
      $problems = $true
      break
    }
  }
}
if ($problems) { Fail 'Layering or header guard problems found.' }
Write-Host "$($logicFiles.Count) logic files, $($headerFiles.Count) headers OK"

Write-Host ''
Write-Host 'All gates passed.'
exit 0
