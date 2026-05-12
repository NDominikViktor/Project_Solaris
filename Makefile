CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Iinclude \
           $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lopengl32 -lglu32

SRCS    = main.c src/app.c src/camera.c src/scene.c src/hud.c src/ui.c src/comet.c
TARGET  = Solaris

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe
