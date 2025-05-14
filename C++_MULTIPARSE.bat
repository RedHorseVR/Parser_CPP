@echo off
echo Processing files...
for %%F in (%*) do (
    echo Processing: %%F
	
	start cParse.bat %%F
    REM Add your processing commands here, such as:
    REM some_command "%%F"
)
echo Done!

