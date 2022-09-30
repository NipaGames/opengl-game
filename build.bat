@echo off
echo [BUILD STARTED]
rem cmake -B build -G "Ninja" .
 cmake -B build -G "Visual Studio 17 2022" .
cd build
cmake --build .
echo [BUILD COMPLETE]
echo.
cd ../bin/Debug
OpenGLProject
cd ..