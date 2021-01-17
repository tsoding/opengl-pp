@echo off
rem launch this from msvc-enabled console

set CFLAGS=/std:c11 /O2 /FC /W4 /WX /Zl /D_USE_MATH_DEFINES /wd4996 /nologo
set INCLUDES=/I Dependencies\GLFW\include /I Dependencies\GLEW\include
set LIBS=Dependencies\GLFW\lib\glfw3.lib Dependencies\GLEW\lib\glew32s.lib opengl32.lib User32.lib Gdi32.lib Shell32.lib

cl.exe %CFLAGS% /MTd %INCLUDES% /Fe"pp.exe" ./main.c %LIBS% /link /NODEFAULTLIB:libcmt.lib