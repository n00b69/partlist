#!/bin/sh
cd "$(dirname "$0")"
rm *.7z *.exe *.o *.bak 2>/dev/null
7z a -mx9 partlist.7z *
cd -