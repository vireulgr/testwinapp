@echo off

:: cl /nologo main.cpp             /Fo.\main.obj   /Zi /c /I..\include /DDEBUG=1
cl /nologo main2.cpp             /Fo.\main2.obj   /Zi /c /I..\include /DDEBUG=1
cl /nologo ..\include\mylib.cpp /Fo.\mylib.obj  /Zi /c /I..\include /DDEBUG=1

cl /nologo .\main2.obj .\mylib.obj /Fe..\myTcpServ.exe

:: vim: ff=dos
