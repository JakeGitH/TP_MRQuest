<#
.SYNOPSIS
    Instaluje szablon TP_MRQuest w Unreal Engine 5.6 (okno New Project -> Games).

.DESCRIPTION
    Kopiuje szablon do <EnginePath>\Templates\TP_MRQuest, pomijając pliki budowania, cache,
    dokumentację repo i pliki git. Wymaga PowerShella uruchomionego jako administrator,
    bo silnik z Epic Games Launchera leży w Program Files.

.PARAMETER EnginePath
    Folder silnika UE 5.6. Domyślnie: C:\Program Files\Epic Games\UE_5.6

.PARAMETER Force
    Nadpisuje istniejącą instalację szablonu bez pytania.

.EXAMPLE
    .\scripts\install-template.ps1
    .\scripts\install-template.ps1 -EnginePath "D:\Epic Games\UE_5.6" -Force
#>
param(
    [string]$EnginePath = 'C:\Program Files\Epic Games\UE_5.6',
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

$TemplateName = 'TP_MRQuest'
$SourceDir = Split-Path -Parent $PSScriptRoot
$TemplatesDir = Join-Path $EnginePath 'Templates'
$TargetDir = Join-Path $TemplatesDir $TemplateName

# --- Sprawdzenia ---------------------------------------------------------------

$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
    [Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    throw 'Uruchom PowerShell jako administrator (prawy klik -> Uruchom jako administrator).'
}

if (-not (Test-Path (Join-Path $SourceDir "$TemplateName.uproject"))) {
    throw "Nie znaleziono $TemplateName.uproject w $SourceDir. Uruchom skrypt z folderu scripts/ repozytorium."
}

if (-not (Test-Path $TemplatesDir)) {
    throw "Nie znaleziono $TemplatesDir. Podaj poprawny folder silnika: -EnginePath ""...\UE_5.6"""
}

foreach ($plugin in 'MetaXR', 'MetaXRInteraction') {
    if (-not (Test-Path (Join-Path $EnginePath "Engine\Plugins\Marketplace\$plugin"))) {
        Write-Warning "Brak pluginu $plugin w Engine\Plugins\Marketplace. Projekty z szablonu nie otworzą się bez niego (patrz README)."
    }
}

if (Test-Path $TargetDir) {
    if (-not $Force) {
        $answer = Read-Host "Szablon już istnieje w $TargetDir. Nadpisać? [t/N]"
        if ($answer -notmatch '^[tTyY]') {
            Write-Output 'Przerwano.'
            return
        }
    }
    Remove-Item -Recurse -Force $TargetDir
}

# --- Kopiowanie ----------------------------------------------------------------

$excludeDirs = 'Binaries', 'Build', 'Intermediate', 'Saved', 'DerivedDataCache', '.vs', '.idea', '.git', 'docs', 'scripts'
$excludeFiles = '*.sln', 'CLAUDE.md', 'README.md', 'LICENSE', '.gitignore', '.gitattributes'

Write-Output "Kopiuję $SourceDir -> $TargetDir"
robocopy $SourceDir $TargetDir /E /NFL /NDL /NJH /NJS /NP /XD $excludeDirs /XF $excludeFiles | Out-Null
# robocopy: kody 0-7 oznaczają sukces, 8+ błąd.
if ($LASTEXITCODE -ge 8) {
    throw "robocopy zakończył się błędem (kod $LASTEXITCODE)."
}

# ProjectID musi być unikalny dla każdego projektu – edytor mógł go dopisać przy ostatnim otwarciu.
$gameIni = Join-Path $TargetDir 'Config\DefaultGame.ini'
if (Test-Path $gameIni) {
    $lines = Get-Content $gameIni -Encoding UTF8 | Where-Object { $_ -notmatch '^ProjectID=' }
    [IO.File]::WriteAllLines($gameIni, $lines)
}

Write-Output ''
Write-Output "Gotowe. Szablon zainstalowany w $TargetDir"
Write-Output 'Uruchom ponownie Unreal Engine 5.6 -> New Project -> Games -> "MR Quest 3 (Passthrough + Hands)".'
