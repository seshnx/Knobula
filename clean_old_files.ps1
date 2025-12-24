# Aetheri Cleanup Script
# Removes old Knobula build artifacts and source files after rebranding

$ErrorActionPreference = "Continue"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Aetheri Cleanup - Removing Old Knobula Files" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$buildDir = Join-Path $PSScriptRoot "build"
$sourceDir = Join-Path $PSScriptRoot "Source"

# Remove old Knobula build directories
$knobulaDirs = @(
    "Knobula_All.dir",
    "Knobula_artefacts",
    "Knobula_rc_lib.dir",
    "Knobula_Standalone.dir",
    "Knobula_VST3.dir",
    "Knobula.dir"
)

Write-Host "Removing old Knobula build directories..." -ForegroundColor Yellow
foreach ($dir in $knobulaDirs)
{
    $fullPath = Join-Path $buildDir $dir
    if (Test-Path $fullPath)
    {
        Write-Host "  Removing: $dir" -ForegroundColor Gray
        Remove-Item -Path $fullPath -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Remove old Knobula project files
$knobulaFiles = @(
    "Knobula_All.vcxproj",
    "Knobula_All.vcxproj.filters",
    "Knobula_rc_lib.vcxproj",
    "Knobula_rc_lib.vcxproj.filters",
    "Knobula_Standalone.vcxproj",
    "Knobula_Standalone.vcxproj.filters",
    "Knobula_VST3.vcxproj",
    "Knobula_VST3.vcxproj.filters",
    "Knobula.vcxproj",
    "Knobula.vcxproj.filters",
    "Knobula.slnx"
)

Write-Host "Removing old Knobula project files..." -ForegroundColor Yellow
foreach ($file in $knobulaFiles)
{
    $fullPath = Join-Path $buildDir $file
    if (Test-Path $fullPath)
    {
        Write-Host "  Removing: $file" -ForegroundColor Gray
        Remove-Item -Path $fullPath -Force -ErrorAction SilentlyContinue
    }
}

# Remove old Knobula CMake rule files
$cmakeRulePath = Join-Path $buildDir "CMakeFiles\071124a87af36c1294041195ada6a591\Knobula_resources.rc.rule"
if (Test-Path $cmakeRulePath)
{
    Write-Host "Removing old Knobula CMake rule file..." -ForegroundColor Yellow
    Remove-Item -Path $cmakeRulePath -Force -ErrorAction SilentlyContinue
}

# Remove old source files (already deleted, but check)
$sourceFiles = @(
    "UI\KnobulaLookAndFeel.h",
    "UI\KnobulaLookAndFeel.cpp"
)

Write-Host "Checking for old Knobula source files..." -ForegroundColor Yellow
foreach ($file in $sourceFiles)
{
    $fullPath = Join-Path $sourceDir $file
    if (Test-Path $fullPath)
    {
        Write-Host "  Removing: $file" -ForegroundColor Gray
        Remove-Item -Path $fullPath -Force -ErrorAction SilentlyContinue
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Cleanup completed!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Note: You may need to reconfigure CMake to remove" -ForegroundColor Yellow
Write-Host "      remaining references in CMakeCache.txt" -ForegroundColor Yellow
Write-Host ""

