@echo off

set NAME=gdshare.exe
set NAME32=gdshare32.exe

if "%1"=="x64"  ( goto x64  )
if "%1"=="test" ( goto test )
if "%1"=="run"  ( goto run  )
if "%1"=="x86"  ( goto x86  )
if "%1"=="x32"  ( goto x86  )

:x64

rem delete old files

del %NAME%

rem compile

echo Compiling x64...
clang++ test.cpp -std=c++20 -lGDShare-x64 -lshell32 -lole32 -luser32 -o %NAME%

:run
rem run test

echo Running...
%NAME%

goto done

:x86

rem compile and run 32-bit version

del %NAME32%

echo Compiling x86...
clang++ test.cpp -std=c++20 -lGDShare-x86 -lshell32 -lole32 -o %NAME32% -m32 -Xlinker /NODEFAULTLIB:msvcrt

echo Running...
%NAME32%

goto done

:done