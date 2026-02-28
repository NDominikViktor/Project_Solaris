cmake_minimum_required(VERSION 3.20)
project(Solaris C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

# Soroljuk fel pontosan, így nem lesz kavarodás
set(SOURCES
        main.c
        src/camera.c
        include/camera.h
        include/scene.h
        src/scene.c
)

add_executable(Solaris ${SOURCES})

target_link_libraries(Solaris mingw32 SDL2main SDL2 opengl32)