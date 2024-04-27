@echo off
setlocal
python.exe Source/Program/ProjectSetupTool/Sync.py %*
if errorlevel 1 goto Error
exit

:Error
pause