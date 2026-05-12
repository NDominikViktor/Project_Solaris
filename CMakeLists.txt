cmake_minimum_required(VERSION 3.20)
project(Solaris C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ── SDK path (a bruh\ mappán belül, Project_Solaris melletti mappa) ───────────
set(SDK_ROOT "${CMAKE_SOURCE_DIR}/../c_sdk_220203/MinGW")

# ── Source files ──────────────────────────────────────────────────────────────
set(SOURCES
        main.c
        src/app.c
        src/camera.c
        src/scene.c
        src/hud.c
        src/ui.c
        src/comet.c
)

add_executable(Solaris ${SOURCES})

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(Solaris PRIVATE
        include                          # saját headerek (app.h, scene.h, stb.)
        "${SDK_ROOT}/include"            # SDL2/SDL.h, GL/gl.h, stb.
)

# ── Link paths + libraries ────────────────────────────────────────────────────
target_link_directories(Solaris PRIVATE "${SDK_ROOT}/lib")

target_link_libraries(Solaris PRIVATE
        mingw32
        SDL2main
        SDL2
        SDL2_image
        SDL2_ttf
        opengl32
        glu32
)

# ── M_PI fix (MinGW strict C11 nem definiálja alapból) ───────────────────────
target_compile_definitions(Solaris PRIVATE _USE_MATH_DEFINES)

# ── Assets másolása a build mappába ──────────────────────────────────────────
add_custom_command(TARGET Solaris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Solaris>/assets"
)cmake_minimum_required(VERSION 3.20)
project(Solaris C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ── SDK path (a bruh\ mappán belül, Project_Solaris melletti mappa) ───────────
set(SDK_ROOT "${CMAKE_SOURCE_DIR}/../c_sdk_220203/MinGW")

# ── Source files ──────────────────────────────────────────────────────────────
set(SOURCES
        main.c
        src/app.c
        src/camera.c
        src/scene.c
        src/hud.c
        src/ui.c
        src/comet.c
)

add_executable(Solaris ${SOURCES})

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(Solaris PRIVATE
        include                          # saját headerek (app.h, scene.h, stb.)
        "${SDK_ROOT}/include"            # SDL2/SDL.h, GL/gl.h, stb.
)

# ── Link paths + libraries ────────────────────────────────────────────────────
target_link_directories(Solaris PRIVATE "${SDK_ROOT}/lib")

target_link_libraries(Solaris PRIVATE
        mingw32
        SDL2main
        SDL2
        SDL2_image
        SDL2_ttf
        opengl32
        glu32
)

# ── M_PI fix (MinGW strict C11 nem definiálja alapból) ───────────────────────
target_compile_definitions(Solaris PRIVATE _USE_MATH_DEFINES)

# ── Assets másolása a build mappába ──────────────────────────────────────────
add_custom_command(TARGET Solaris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Solaris>/assets"
)
