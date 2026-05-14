\# Solaris – Interactive Solar System Simulator



A real-time 3D solar system simulation written in C using OpenGL and SDL2.

The program renders the planets of our solar system with textures, orbital

mechanics, atmospheric effects and a particle ring system.  A built-in

editor allows the user to create entirely custom solar systems, save them

to CSV and reload them at any time.



\## Features



\### Simulation

\- All planets orbit the Sun with correct relative speeds and axial tilts

\- Moons orbit their parent planets (hierarchical parent–child system)

\- Saturn and Uranus have particle ring systems with configurable colour and radius

\- Halley's comet follows an elliptical orbit with a glowing particle trail

\- An asteroid belt of 500 procedurally-placed objects between Mars and Jupiter

\- Moon shadow projected onto parent planet surface

\- Adjustable time scale (pause, slow-motion, up to 10×)



\### Rendering

\- Per-planet texture mapping loaded from PNG/JPEG via stb\_image

\- Atmospheric halo (semi-transparent blend) on Earth, Venus, Mars, the gas giants

\- Sun glow rendered with additive blending (multi-layer billboard)

\- Orbital path circles (toggle with O)

\- Skybox (star field cube)

\- OpenGL fog (toggle with F)

\- Level of Detail: sphere tessellation reduced at distance (64→8 segments)



\### Editor

\- Add, delete, rename planets, moons and stars

\- Set object type (Star / Planet / Moon) and parent body

\- Adjust size, distance, orbital speed, rotation speed, axial tilt via sliders

\- Pick texture from built-in presets or type a custom filename

\- Toggle and colour atmosphere and ring system

\- Save custom solar system to `assets/custom\_planets.csv`



\### UI / Controls

\- First-person camera with WASD + mouse look

\- Camera presets: top-down, side view, free

\- Planet selection by left-click (ray casting) or number keys 1–9

\- Sun intensity adjustable with +/–

\- F1 / H: in-game help overlay



\## Assets



The `assets/` folder (textures, font, OBJ model) is not included in this

repository due to file size.



\*\*Download:\*\* \[assets.zip – OneDrive]([https://onedrive.live.com/?viewid=6d6dc4bb%2D467d%2D4598%2Daeca%2D559634bb3e1c\&id=%2Fpersonal%2F9603013e49ded85d%2FDocuments%2Fassets%2Ezip\&parent=%2Fpersonal%2F9603013e49ded85d%2FDocuments](https://drive.google.com/drive/folders/1LKWDNLhagSK4efZwYKZQF9u9wZxawSx_?usp=sharing))



Extract so that the `assets/` folder sits next to the executable.



\## Project structure



```

Solaris/

├── main.c            # Entry point and main loop

├── src/

│   ├── camera.c      # First-person camera movement and collision

│   ├── scene.c       # Planet/moon/ring/asteroid loading and rendering

│   ├── hud.c         # HUD, projection setup, orbit paths, help overlay

│   ├── ui.c          # Main menu and editor panel (OpenGL + SDL2\_ttf)

│   └── comet.c       # OBJ model loader, comet animation and tail

├── include/

│   ├── camera.h

│   ├── scene.h

│   ├── hud.h

│   ├── ui.h

│   ├── comet.h

│   └── stb\_image.h

├── assets/           # Textures, font, OBJ model (see download link above)

├── Makefile

└── CMakeLists.txt

```



\## Build



\### Requirements

\- GCC (MinGW on Windows) or Clang

\- SDL2, SDL2\_image, SDL2\_ttf development libraries

\- OpenGL / GLU



\### Compile with make

```bash

make

./Solaris        # Linux / macOS

Solaris.exe      # Windows

```



\### Compile with CMake

```bash

cmake -B build

cmake --build build

```



\## Controls



| Key / Input | Action |

|---|---|

| W / A / S / D | Move forward / left / backward / right |

| Q / E | Move down / up |

| Shift | Sprint (3× speed) |

| Arrow keys | Rotate camera |

| Right mouse + drag | Look around |

| Left mouse click | Select planet |

| 1 – 9 | Jump camera to planet |

| 0 | Release planet follow |

| T / Y / U | Camera preset: top / side / free |

| P | Pause / resume simulation |

| \[ / ] | Decrease / increase time scale |

| + / – | Sun intensity |

| F | Toggle fog |

| O | Toggle orbit paths |

| F1 / H | Toggle help overlay |

| ESC | Quit |



\## Third-party licenses



| Library | License |

|---|---|

| \[SDL2](https://www.libsdl.org/) | zlib |

| \[SDL2\_image](https://github.com/libsdl-org/SDL\_image) | zlib |

| \[SDL2\_ttf](https://github.com/libsdl-org/SDL\_ttf) | zlib |

| \[stb\_image.h](https://github.com/nothings/stb) | MIT / Public Domain |



\## Author



Novák Dominik Viktor – 2026

