$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ScriptDir "build"

Write-Host "[RT1189 host smoke] configuring..."
cmake -S $ScriptDir -B $BuildDir
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[RT1189 host smoke] building C/API/include smoke targets..."
cmake --build $BuildDir --target rt1189_host_smoke
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[RT1189 host smoke] PASS"
Write-Host "Scope: host compilation only. This does NOT verify MCUXpresso SDK target linking, startup/linker files, pinmux/clocks, LPSPI hardware, flashing, or MIMXRT1180-EVK behavior."
