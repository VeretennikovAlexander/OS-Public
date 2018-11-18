set VCDir=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0
call "%VCDir%\Common7\Tools\vsvars32.bat"
set BASEDIR=%CD%

del Test.exe
del Test.asm
del Test.cod
del Test.obj

rem cl Test.cpp /FiTest.i /P

rem For asm files generation, if we want them

cl Test.cpp /FAc /c /EHcs
cl Test.cpp /FA /c /EHcs /O2

link Test.obj

test.exe

pause

rem /EH (Exception Handling Model)
rem /O2 (Maximize Speed)
