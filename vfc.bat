echo off

IF NOT EXIST %1.vfc (
    echo %1 does not have a .vfc.
    echo parsing %1 to vfc...
    cParse.bat %1
)
rem start vfc2000 %1.vfc -Reload
start vfc2000 %1.vfc -Goto %2
