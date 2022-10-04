# opengl-game
## Repository status <img src="status.png?" width="150" height="30" align="right"/>

- Working: you can clone and build the repo and expect working results
- In progress: probably some debug build testing something

## About
At the moment this is just a rendering test with WASD-controls and first-person camera.
![Image showing the game](/screenshot.png)
![Image also showing the game](/screenshot2.png)
![Image also showing the game](/screenshot3.png)
### Features:
- [x] FPS controls
- [x] Type-based entity component system (like in Unity)
- [x] Model loading
- [ ] UI/HUD
- [ ] Lighting

*(unchecked features are yet to be implemented)*
## Building
Builds only on Windows for now. ``build.bat`` should build everything. The executable can be found from ``bin/Release`` or ``bin/Debug`` depending on the build type. Build type can be changed in ``build.bat``.
## Requirements
- MSVC build tools
- C++17 or higher
- CMake