@echo off
setlocal
cmake --build Build
if errorlevel 1 goto Error
exit

:Error
pause