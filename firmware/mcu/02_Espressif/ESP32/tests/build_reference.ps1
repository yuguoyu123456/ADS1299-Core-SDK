param([int]$Jobs = 8)
$ErrorActionPreference = 'Stop'
$platformPath = Split-Path $PSScriptRoot -Parent
$localCore = Join-Path $PSScriptRoot '.tools/pio'
if (-not (Test-Path -LiteralPath (Join-Path $localCore 'platforms/espressif32/platform.json'))) {
    throw 'Prepare the documented local PlatformIO 6.10.0 tool cache first; no global install is performed by this script.'
}
$saved = @{}
$values = @{
    PLATFORMIO_CORE_DIR = $localCore
    PLATFORMIO_CACHE_DIR = (Join-Path $localCore '.cache')
    IDF_TOOLS_PATH = (Join-Path $localCore 'idf-tools')
    PIP_CACHE_DIR = (Join-Path $localCore 'pip-cache')
}
try {
    foreach ($key in $values.Keys) {
        $saved[$key] = [Environment]::GetEnvironmentVariable($key, 'Process')
        [Environment]::SetEnvironmentVariable($key, $values[$key], 'Process')
    }
    & platformio run -d $platformPath -j $Jobs
    if ($LASTEXITCODE -ne 0) { throw "Reference build failed: $LASTEXITCODE" }
} finally {
    foreach ($key in $saved.Keys) {
        [Environment]::SetEnvironmentVariable($key, $saved[$key], 'Process')
    }
}
