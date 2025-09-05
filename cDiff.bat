REM CODE DIFFERENCE CHECK ...
@echo off
setlocal enabledelayedexpansion
echo %1


rem set /p exten="Enter code file extension: "
rem echo EXTEN:%exten%!


if "%~1"=="" (
    echo Usage: compare_cleaned.bat file1.c file2.c
    exit /b
)
if "%~2"=="" (
    echo Usage: compare_cleaned.bat file1.c file2.c
    exit /b
)

ECHO ---------------------------------
set "fileDir=%CD%"
echo Current directory is: %fileDir%
cd
ECHO FILE1: %~1.%exten%
ECHO FILE2: %~2.%exten%
ECHO ---------------------------------


set "clean1=%1"
set "clean2=%2"

set BAT_FILE_PATH=%~dp0

echo FOMRATTING ::::::::::::::::::::::::::::::::::
clang-format -style=file:"%BAT_FILE_PATH%.clang-format" -i "%clean1%"
clang-format -style=file:"%BAT_FILE_PATH%.clang-format" -i "%clean2%"

WinMergeU.exe "%clean1%" "%clean2%"

ECHO C CODE DIFF CHECK EXIT ... 

rem :: Step 1: Remove comments, blank lines, indentation, trailing spaces, tabs, multiple spaces, and fix commas BEFORE Clang
rem powershell -Command "& { (Get-Content -LiteralPath \"\\?\%CD%\%1\") -replace '/\*.*?\*/', '' -replace '//.*$', '' -replace '^\s+', '' -replace '\s+$', '' -replace '^\s+$', '' -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content \"\\?\%CD%\%clean1%\" }"
rem powershell -Command "& { (Get-Content -LiteralPath \"\\?\%CD%\%2\") -replace '/\*.*?\*/', '' -replace '//.*$', '' -replace '^\s+', '' -replace '\s+$', '' -replace '^\s+$', '' -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content \"\\?\%CD%\%clean2%\" }"
rem :: Step 2: Run Clang-Format to clean and structure the files
rem :: Step 3: Remove blank lines AGAIN after Clang-Format and reapply space fixes + comma correction
rem powershell -Command "(Get-Content '%clean1%') -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean1%'"
rem powershell -Command "(Get-Content '%clean2%') -replace '\t', ' ' -replace ' {2,}', ' ' -replace ', ', ',' | Where-Object {$_ -match '\S'} | Set-Content '%clean2%'"
rem :: Launch WinMergeU.exe for comparison
rem :set "clean1=clean1.c"
rem :set "clean2=clean2.c"

