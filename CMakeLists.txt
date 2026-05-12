CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Iinclude -D_USE_MATH_DEFINES
TARGET  = Solaris
SRCS    = main.c src/app.c src/camera.c src/scene.c src/hud.c src/ui.c src/comet.c

# ── Platform detection ────────────────────────────────────────────────────────
ifeq ($(OS),Windows_NT)
# Windows / MinGW
SDK     = ../c_sdk_220203/MinGW
CFLAGS += -I$(SDK)/include
LIBS    = -L$(SDK)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lopengl32 -lglu32
EXT     = .exe
else
# Linux
CFLAGS += $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lGL -lGLU -lm
EXT     =
endif

# ── Build ─────────────────────────────────────────────────────────────────────
all: $(TARGET)$(EXT)

$(TARGET)$(EXT): $(SRCS)
$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

clean:
rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean