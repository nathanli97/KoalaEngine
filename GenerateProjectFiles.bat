@echo off
setlocal
python.exe Source/Program/ProjectSetupTool/GenerateProjectFiles.py %*
if errorlevel 1 goto Error
exit

:Error
pause