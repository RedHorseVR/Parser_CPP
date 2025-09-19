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


