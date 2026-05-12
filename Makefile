SDK_REL = ../c_sdk_220203/MinGW
TARGET  = Solaris
SRCS    = main.c src/app.c src/camera.c src/scene.c src/hud.c src/ui.c src/comet.c

ifeq ($(OS),Windows_NT)
    SDK     = $(SDK_REL)
    CC      = $(SDK)/bin/gcc.exe
    CFLAGS  = -std=c11 -Iinclude -I$(SDK)/include
    LIBS    = -L$(SDK)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lopengl32 -lglu32
    OUT     = $(TARGET).exe
    PATH   := $(SDK)/bin;$(PATH)
    DLLS    = SDL2.dll SDL2_image.dll SDL2_ttf.dll \
              libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll
else
    CC      = gcc
    CFLAGS  = -std=c11 -Iinclude -D_USE_MATH_DEFINES $(shell sdl2-config --cflags)
    LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lGL -lGLU -lm
    OUT     = $(TARGET)
endif

all: $(OUT) copy_dlls

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

copy_dlls:
ifeq ($(OS),Windows_NT)
	$(foreach dll,$(DLLS),cmd /c copy "$(subst /,\,$(SDK)/bin/$(dll))" . &)
endif

clean:
	rm -f $(TARGET) $(TARGET).exe $(DLLS)

.PHONY: all copy_dlls cleanSDK_REL = ../c_sdk_220203/MinGW
TARGET  = Solaris
SRCS    = main.c src/app.c src/camera.c src/scene.c src/hud.c src/ui.c src/comet.c

ifeq ($(OS),Windows_NT)
    SDK     = $(SDK_REL)
    CC      = $(SDK)/bin/gcc.exe
    CFLAGS  = -std=c11 -Iinclude -I$(SDK)/include
    LIBS    = -L$(SDK)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lopengl32 -lglu32
    OUT     = $(TARGET).exe
    PATH   := $(SDK)/bin;$(PATH)
    DLLS    = SDL2.dll SDL2_image.dll SDL2_ttf.dll \
              libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll
else
    CC      = gcc
    CFLAGS  = -std=c11 -Iinclude -D_USE_MATH_DEFINES $(shell sdl2-config --cflags)
    LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lGL -lGLU -lm
    OUT     = $(TARGET)
endif

all: $(OUT) copy_dlls

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

copy_dlls:
ifeq ($(OS),Windows_NT)
	$(foreach dll,$(DLLS),cmd /c copy "$(subst /,\,$(SDK)/bin/$(dll))" . &)
endif

clean:
	rm -f $(TARGET) $(TARGET).exe $(DLLS)

.PHONY: all copy_dlls clean
