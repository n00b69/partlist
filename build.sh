#!/bin/sh
[ -z "$CC" ] && CC="aarch64-w64-mingw32-clang"
[ -z "$CXX" ] && CXX="aarch64-w64-mingw32-clang++"
[ -z "$WINDRES" ] && WINDRES="llvm-windres"
[ -z "$OPT" ] && OPT="3"

cd "$(dirname "$0")"
rm *.exe *.o 2>/dev/null
"$WINDRES" partlist.rc partlist.o || { cd - && exit 1;}
"$CC" -o partlist.exe partlist.o partlist.c -std=c99 -pedantic -Wall -Wextra "-O$OPT" -s || { cd - && exit 1;}
"$CC" -o partlist_debug.exe partlist.o partlist.c -std=c99 -pedantic -DDEBUG -Wall -Wextra "-O$OPT" -s || { cd - && exit 1;}
cd -
