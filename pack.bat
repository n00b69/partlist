pushd "%~dp0"
del /f /q *.7z *.exe *.o *.bak
"C:\Program Files\7-Zip\7z.exe" a -mx9 partlist.7z *
popd