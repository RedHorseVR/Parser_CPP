REM CODE DIFFERENCE CHECK ...
@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Usage: compare_cleaned.bat file1.c file2.c
    exit /b
)
if "%~2"=="" (
    echo Usage: compare_cleaned.bat file1.c file2.c
    exit /b
)

set "clean1=clean1.c"
set "clean2=clean2.c"

:: Step 1: Remove comments, blank lines, indentation, trailing spaces, tabs, multiple spaces, and fix commas BEFORE Clang
powershell -Command "(Get-Content '%~1') -replace '/\*.*?\*/', '' -replace '//.*$', '' -replace '^\s+', '' -replace '\s+$', '' -replace '^\s+$', '' -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean1%'"
powershell -Command "(Get-Content '%~2') -replace '/\*.*?\*/', '' -replace '//.*$', '' -replace '^\s+', '' -replace '\s+$', '' -replace '^\s+$', '' -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean2%'"

:: Step 2: Run Clang-Format to clean and structure the files
clang-format -style="Google" -i "%clean1%" "%clean2%"

:: Step 3: Remove blank lines AGAIN after Clang-Format and reapply space fixes + comma correction
powershell -Command "(Get-Content '%clean1%') -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean1%'"
powershell -Command "(Get-Content '%clean2%') -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean2%'"

:: Launch WinMergeU.exe for comparison
WinMergeU.exe "%clean1%" "%clean2%"

ECHO C CODE DIFF CHECK EXIT ... 

del "%clean1%" 
del "%clean2%"
