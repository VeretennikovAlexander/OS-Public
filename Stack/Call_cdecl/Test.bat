set VCDir=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0
call "%VCDir%\Common7\Tools\vsvars32.bat"
set BASEDIR=%CD%

rem cl Test.cpp /FiTest.i /P

cl Test.cpp /FAc /c
cl Test.cpp /FA /c

link Test.obj

test.exe

pause