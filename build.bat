@echo off
echo [BUILD STARTED]
rem cmake -B build -G "Ninja" .
 cmake -B build .
cd build
cmake --build . --config Release
echo [BUILD COMPLETE]
echo.
cd ../bin/Release
OpenGLProject
cd ..