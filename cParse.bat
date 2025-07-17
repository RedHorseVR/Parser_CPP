echo off
set BAT_FILE_PATH=%~dp0

rem set BAT_FILE_PATH=C:\Users\lopezl10\AppData\Local\RedHorseVR\C2VFC_parser

echo The absolute path of this batch file is: %BAT_FILE_PATH%

SET COMMENTPARSERPATH=%BAT_FILE_PATH%

rem pip install clang-format

echo off

set filepath=%1
for %%A in ("%filepath%") do set filename=%%~nxA
echo Filename: %filename%

echo PARSING : %filepath%%filename% 

cmd /c python %BAT_FILE_PATH%cppParser.py %1

pause

start vfc2000 %1.vfc -Reload


 

