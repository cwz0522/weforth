del *.obj
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat"
nmake -f makefile

del ..\*.exp
del ..\*.lib

pause