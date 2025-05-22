echo off

IF NOT EXIST %1.vfc (
    echo %1 does not have a .vfc.
    echo parsing %1 to vfc...
    cParse.bat %1
)

start vfc2000 %1.vfc -Goto %2

pause