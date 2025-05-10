echo off
set BAT_FILE_PATH=%~dp0

set BAT_FILE_PATH="C:\Users\luisr\Arduino_VFC_parser"

echo The absolute path of this batch file is: %BAT_FILE_PATH%

SET VFCTOOLPATH=C:\VFC1.0
SET COMMENTPARSERPATH=%BAT_FILE_PATH%


rem pip install clang-format

echo off

set filepath=%1
for %%A in ("%filepath%") do set filename=%%~nxA
echo Filename: %filename%

rem copy %filename% _%filename%
rem  -o outputs a back up copy of the naem given
rem _%filename%

echo .
set filepath=%~dp1
echo filepath : %filepath%
echo run ---
ecgi on
	cmd /c python %COMMENTPARSERPATH%\cParser.py %1 -o %filepath%_%filename%
echo .

start vfc2000 %1.vfc -Reload

pause
