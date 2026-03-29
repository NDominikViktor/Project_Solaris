# Solaris – Solar System Simulator

A 3D interactive solar system simulation built with C, OpenGL, and SDL2.

## Assets

The `assets/` folder (textures, models) is not included in this repository due to file size.
**Download link:** [assets.zip – OneDrive/DropBox](https://onedrive.live.com/?viewid=6d6dc4bb%2D467d%2D4598%2Daeca%2D559634bb3e1c&id=%2Fpersonal%2F9603013e49ded85d%2FDocuments%2Fassets%2Ezip&parent=%2Fpersonal%2F9603013e49ded85d%2FDocuments)
Extract the archive so that the `assets/` folder sits next to the executable.

## Build

### Requirements

- GCC (MinGW on Windows)
- SDL2 + SDL2_image development libraries
- OpenGL / GLU

### Compile

```bash
make
```

Run with:
```bash
./Solaris        # Linux / macOS
Solaris.exe      # Windows
```

Clean build artefacts:
```bash
make clean
```

## Controls

| Key / Input | Action |
|---|---|
| W / S / A / D | Move forward / backward / left / right |
| Q / E | Move down / up |
| Arrow keys | Rotate camera |
| Right mouse button + drag | Look around |
| Left mouse click | Select planet |
| 1 – 9 | Jump camera to planet |
| 0 | Release planet follow |
| + / - (numpad or main) | Increase / decrease sun intensity |
| F | Toggle fog |
| F1 / H | Toggle help overlay |
| ESC | Quit |

## Third-party licenses

| Library | License |
|---|---|
| [SDL2](https://www.libsdl.org/) | zlib license |
| [SDL2_image](https://github.com/libsdl-org/SDL_image) | zlib license |
| [stb_image.h](https://github.com/nothings/stb) | MIT / Public Domain |

## Author

Novák Dominik Viktor – 2026