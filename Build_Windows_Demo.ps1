param(
    [string]$EngineRoot = $env:UE_ROOT,
    [string]$Output = ""
)
$ErrorActionPreference = "Stop"
if (-not $EngineRoot) { $EngineRoot = "C:\Program Files\Epic Games\UE_5.8" }
if (-not $Output) { $Output = Join-Path $PSScriptRoot ("BuildOutput\" + (Get-Date -Format "yyyyMMdd-HHmmss")) }
$Python = Get-Command py -ErrorAction SilentlyContinue
if ($Python) { & $Python.Source -3 (Join-Path $PSScriptRoot "Scripts\build_windows.py") --engine-root $EngineRoot --output $Output }
else { & python (Join-Path $PSScriptRoot "Scripts\build_windows.py") --engine-root $EngineRoot --output $Output }
if ($LASTEXITCODE -ne 0) { throw "Windows build did not complete. Inspect the reported error and build logs." }
Write-Host "Build pipeline finished. Complete Docs\WINDOWS_DEMO_ACCEPTANCE.md before calling the demo verified."
