#  opengl-game

## I MADE A NEW REPO I'VE BEEN UPDATING FOR THE ENGINE SIDE: https://github.com/NipaGames/latren


As it's probably evident from the first screenshots, my goal was never to make an actual "game", just mess around and figure how a game engine works (or more likely, how it shouldn't).
I never thought I'd get this far with this dumb project when I started cooking this spaghetti at 15, a bit over a year ago now.
However, now with over 300 commits and 10k lines of my horrifying C++, I'd say I've come up with a pretty good foundation for a game.
Therefore I've decided to work more on the creative game side and only shape the engine whenever needed for the game part.
There are already a few unfinished levels and some primitive game mechanics, but I have many more ideas left.
(Also there's all the Unity stuff but I can't see how I'd ever make that much money from any of this to start worrying about fees, licencing and all that bs.
At least I don't have to see that splash screen and shit still runs pretty well on my garbage laptop.)
I, for once, have a direction I want to take this, yippee!!!
please root for your local awkward transfem programmer :3  
  
**Since I'll be focusing on an actual game, this will probably be one of the last commits before I shut the repo private.**  
I might keep the engine core public or even structure it into a library (that would be a tremendous effort but who knows, maybe one day I don't have anything other to do, whenever that is).

Obviously, I'm still in high school and won't be getting anywhere soon since I'm kinda exhausted with my studies and my motivation is generally really low.
So, working gamedev full time (or even studying it, well, formally - in an actual school) is out of the question for now but I'll try to keep this as a passion project.
We'll see whatever abomination this turns into!

![main menu](/images/screenshot19.png)

*Latreus first time in the daylight!*

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/361a94a8bb7b4340b490dbdcbeb28b03)](https://app.codacy.com/gh/NipaGames/opengl-game/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

##  Repository status

![Status](/images/status-small.png)

- Working: you can clone and build the repo and expect working results

- In progress: probably some debug build testing something

  

##  About

At the moment this is just a rendering test with WASD-controls and first-person camera.

##  Progress

###  Version history:

-  **v0.8:** main menu, more game

-  **v0.7:** event system, console

-  **v0.6:** added some actual content (a cave level)

-  **v0.5:** more serialization (for example materials.json and objects.json)

-  **v0.4:** bullet physics engine

-  **v0.3:** stage loading basics

-  **v0.2:** whacky ass reflection things and shit for serialization later

  

###  Gallery

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

  

![Image of terrain generation](/images/screenshot7.png)

*Simple terrain generation* 2.11.2022

  

![Image of texture rendering](/images/screenshot8.png)

*Now everything is made of this stupid grass texture from Wikimedia* 6.3.2023

  

![Image of inverted color showcase](/images/screenshot9.png)

*Annoying filter to invert colors* 8.3.2023

  

![Image of text rendering showcase](/images/screenshot10.png)

*The words of wisdom* 16.3.2023

  

![Image of text rendering showcase](/images/screenshot11.png)

*Integrated Bullet physics library and introducing stage loading* 19.5.2023

 

![boring ass profiler stuff](/images/verysleepy.png)

*Got this thing running 2000 fps after a few minor optimizations.*  
*(highlighted functions directly called in OpenGLProject.exe)*  
*Yellow: math*  
*Blue: win32 window management stuff and input system*  
5.6.2023

 

![skybox](/images/screenshot12.png)

*The Great Skybox Update. (also the debug overlay and the level layout haven't been shown yet)*  
18.9.2023

  

![cool cave screenshot](/images/screenshot13.png)

 

![cool fortress room screenshot](/images/screenshot14.png)

*now it actually looks like a game*  
10.10.2023

 

![a gate opening](/images/gate.gif)

*added a cool event system* 1.11.2023

 

![hd 640p screenshot](/images/screenshot15.png)

*cool area message and also authentic 640p experience* 4.11.2023

 

![a desolate endless ocean with a lone ship](/images/screenshot16.png)

*The Ocean Update™* 8.11.2023

 

![game over](/images/screenshot17.png)

*game over* 16.11.2023

 

![nightmarish scenery](/images/screenshot18.png)

*welcome to hell* 24.11.2023

 

###  Current features:

-  [x] FPS controls

-  [x] Type-based entity component system (like in Unity)

-  [x] Model loading

-  [x] UI/HUD (basically just text rendering)

-  [x] Lighting (very *very* basic, no shadows yet)

-  [x] Bullet physics engine

-  [x] Loading stages with motherfuckin' homemade reflection/serialization

-  [ ] Unity level editing support: export levels from Unity to this garbage

-  [ ] actual gameplay

  

*(unchecked features are yet to be implemented)*

##  Building

Builds only on Windows for now. ``build.bat`` should build everything. The executable can be found from ``bin/Release`` or ``bin/Debug`` depending on the build type. Build type can be changed in ``build.bat``.

###  Requirements

- MSVC build tools

- C++17 or higher

- CMake
