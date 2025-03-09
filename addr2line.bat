set /p ndkpath=<ndkpath.txt
%ndkpath%\toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-addr2line.exe -f -C --obj=%1 %2