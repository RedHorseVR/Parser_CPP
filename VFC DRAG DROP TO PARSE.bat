rem DRAG DROP TO PARSE AND MAKE BACKUP FOR CODE FILE 
rem MUST BE VALID PYTHON TO WORK - NO SYNTAX ERRORS 
rem TO VIEW AS FLOW YOU WILL NEED TO OBTAIN VFC2000.exe 
rem EMAIL luis.r.lopez@outlook.com for access or check for a GitHub.com/RedhorseVR/VFCode open version
rem NEED TO SET VFCTOOLPATH and COMMENTPARSERPATH to where you have the tools 

@echo off
set BAT_FILE_PATH=%~dp0
echo The absolute path of this batch file is: %BAT_FILE_PATH%

SET VFCTOOLPATH=C:\VFC1.0
SET COMMENTPARSERPATH=%BAT_FILE_PATH%

echo off
set filepath=%1
for %%A in ("%filepath%") do set filename=%%~nxA
echo Filename: %filename%

rem copy %filename% _%filename%

python %COMMENTPARSERPATH%\inoCommentParser.py %1 -o _%filename%



dir


start %VFCTOOLPATH%\VFC2000.exe %1.vfc -Reload

pause
