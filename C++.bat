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
		set backup=%%~dpF_%%~nxF
		set file=%%~dpF%%~nxF

    	echo -------------- !file! ... !backup!
    
    	copy "!file!" "!backup!"
	echo ...
	ECHO python %~dp0cppParser.py "!file!"

	python %~dp0cppParser.py "!file!"

	start VFC2000 !file!.vfc -Reload
	
)
timeout /T 6
echo Done!

