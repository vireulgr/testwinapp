@echo off

cl /nologo main.cpp             /Fo.\main.obj   /Zi /c /I..\include /DDEBUG=1
cl /nologo ..\include\mylib.cpp /Fo.\mylib.obj  /Zi /c /I..\include /DDEBUG=1

cl /nologo .\*.obj /Fe.\myTcpCl.exe

:: vim: ff=dos
