@echo off
echo [BUILD STARTED]
rem cmake -B build -G "Ninja" .
 cmake -B build .
cd build
cmake --build . --config Release || exit 1
echo [BUILD COMPLETE]
echo.
cd ../bin/Release
OpenGLProject
cd ..