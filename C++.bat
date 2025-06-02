echo off
echo Processing files...

@echo off
setlocal enabledelayedexpansion

for %%F in (%*) do (
    	echo Processing: "%%F"
	
	rem  start %CD%/cParse.bat "%%F"

    	set filename=%%~nxF
    	echo root: !filename!
    	echo Folder: %%~dpF
    	echo -----------------------
    
    	copy %%F %%~dpF\_!filename!

	python %CD%\cParser.py %%F

	start VFC2000 %%F.vfc -Reload
	timeout /T 2
)
echo Done!

rem PAUSE