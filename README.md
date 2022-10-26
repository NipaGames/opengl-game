# opengl-game
## Repository status <img src="/images/status.png?" width="150" height="30" align="right"/>

- Working: you can clone and build the repo and expect working results
- In progress: probably some debug build testing something

## About
At the moment this is just a rendering test with WASD-controls and first-person camera.
## Progress
![Image showing the game](/images/screenshot.png)
*Cube rendering test* 30.9.2022 (DD-MM-YYYY)

![Image also showing the game](/images/screenshot2.png)
*Model loading* 4.10.2022

![Image also showing the game](/images/screenshot3.png)
*Happy monkey circle* 5.10.2022

![Image showing the lighting](/images/screenshot4.png)
*Dark monkey cult and dramatic lighting* 7.10.2022

![Image showing amogus](/images/screenshot5.png)
*Sussy situation* 8.10.2022

![Image of rtx monke](/images/screenshot6.png)
*RTX monkey <sub>(not actually)</sub>* 26.10.2022

### Current features:
- [x] FPS controls
- [x] Type-based entity component system (like in Unity)
- [x] Model loading
- [ ] UI/HUD
- [x] Lighting (very *very* basic)

*(unchecked features are yet to be implemented)*
## Building
Builds only on Windows for now. ``build.bat`` should build everything. The executable can be found from ``bin/Release`` or ``bin/Debug`` depending on the build type. Build type can be changed in ``build.bat``.
## Requirements
- MSVC build tools
- C++17 or higher
- CMake