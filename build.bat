if not defined CC set CC=gcc
if not defined CXX set CXX=g++
if not defined WINDRES set WINDRES=windres
if not defined OPT set OPT=3

pushd "%~dp0"
del /f /q partlist.exe partlist_debug.exe partlist.o
"%WINDRES%" partlist.rc partlist.o || goto fail
"%CC%" -o partlist.exe partlist.o partlist.c -std=c99 -pedantic -Wall -Wextra -O%OPT% -s || goto fail
"%CC%" -o partlist_debug.exe partlist.o partlist.c -std=c99 -pedantic -DDEBUG -Wall -Wextra -O%OPT% -s || goto fail
popd
exit /b

:fail
popd
pause
exit /b 1