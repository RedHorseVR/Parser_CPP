echo off
echo DIR------------------
echo %CD%
echo FILE-----------------
for %%a in ("%1") do set FILE=%%~nxa
echo file: %FILE%
FOR %%f IN (%FILE%) DO set EXPORT=%%~nf
echo export: %EXPORT%

cd 	%CD%

echo run.bat ------------------
echo "%CD%\run.bat" %EXPORT%

timeout 1

%CD%\run.bat %EXPORT%

timeout 10

