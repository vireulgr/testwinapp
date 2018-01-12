@echo off
cd client
echo    CLIENT
cmd /D /C .\make.bat
echo    SERVER
cd ..\server
cmd /D /C .\make.bat
cd ..\

:: vim: ff=dos
