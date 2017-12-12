@echo off
cd client
echo " "
echo "  vvv  CLIENT"
cmd /D /C .\make.bat
echo " " 
echo "  vvv  SERVER"
cd ..\server
cmd /D /C .\make.bat
cd ..\

:: vim: ff=dos
