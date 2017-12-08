@echo off

cl /nologo main.cpp             /sdl /Fo.\main.obj   /Zi /c /I..\include /DDEBUG=1
cl /nologo ..\include\mylib.cpp /sdl /Fo.\mylib.obj  /Zi /c /I..\include /DDEBUG=1

cl /nologo .\*.obj /Fe.\wsock.exe
