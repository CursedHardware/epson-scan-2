#!sh

find . -name "CMakeCache.txt" | xargs -I {} rm {}
find . -name "cmake_install.cmake" | xargs -I{} rm {}
find . -name "Makefile" | xargs -I{} rm {}
find . -name "CMakeFiles" | xargs -I {}  rm -r {}

