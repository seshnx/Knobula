# Aetheri Clean Script
# Removes all build artifacts and temporary files

param(
    [switch]$All = $false  # If true, also removes JUCE dependencies
)

$ErrorActionPreference = "Stop"

# Get the script directory (project root)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = $ScriptDir

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Aetheri Clean Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Project Directory: $ProjectDir" -ForegroundColor Gray
Write-Host ""

# Directories to clean
$BuildDir = Join-Path $ProjectDir "build"
$DepsDir = Join-Path $BuildDir "_deps"

# Clean build directory
if (Test-Path $BuildDir)
{
    Write-Host "Removing build directory..." -ForegroundColor Yellow
    Remove-Item -Path $BuildDir -Recurse -Force
    Write-Host "Build directory removed." -ForegroundColor Green
}
else
{
    Write-Host "Build directory not found. Nothing to clean." -ForegroundColor Gray
}

# Clean any other temporary files
$TempFiles = @(
    "*.user",
    "*.suo",
    "*.sdf",
    "*.opensdf",
    "*.db",
    "*.opendb",
    ".vs",
    ".idea",
    "*.swp",
    "*.swo",
    "*~"
)

$CleanedCount = 0
foreach ($pattern in $TempFiles)
{
    $files = Get-ChildItem -Path $ProjectDir -Filter $pattern -Recurse -ErrorAction SilentlyContinue -Force
    foreach ($file in $files)
    {
        try
        {
            Remove-Item -Path $file.FullName -Recurse -Force -ErrorAction SilentlyContinue
            $CleanedCount++
        }
        catch
        {
            # Ignore errors for files that might be in use
        }
    }
}

if ($CleanedCount -gt 0)
{
    Write-Host "Removed $CleanedCount temporary file(s)." -ForegroundColor Green
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Clean completed successfully!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "To rebuild, run: .\build.ps1" -ForegroundColor Cyan
Write-Host ""

