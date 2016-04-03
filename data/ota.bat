@echo off
@echo Incep
:start
IF NOT EXIST %1 GOTO :end
@echo Transfer
d:\bat\curl -F "image=@%1" %2/update
del %1
@echo .
goto :start
:end
ping 1.1.1.1 -n 1 -w 1000 > nul
goto :start

