@echo off
echo [BUILD STARTED]
rem cmake -B build -G "Ninja" .
 cmake -B build .
cd build
cmake --build .
echo [BUILD COMPLETE]
echo.
cd ../bin/Debug
OpenGLProject
cd ..