echo ------------------
echo off
set /p exten="Enter code file extension: "
echo EXTEN:%exten%!

choice /C YN /M "Do you want to delete %1.%exten%.vfc and revert to original?"
if errorlevel 2 (
    echo Operation canceled.
	pause
	exit
) else (
    echo Proceeding...
	del %1.%exten% 
	ren _%1.%exten% %1.%exten%
	pause
	cParse.bat %1.%exten%
	pause
	vfc2000 %1.%exten%.vfc -Reload	
)

pause
