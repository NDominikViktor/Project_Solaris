cmake_minimum_required(VERSION 3.20)
project(Solaris C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
        main.c
        src/camera.c
        src/scene.c
        src/ui.c
        include/camera.h
        include/scene.h
        include/ui.h
        include/stb_image.h
)

add_executable(Solaris ${SOURCES})

target_link_libraries(Solaris mingw32 SDL2main SDL2 SDL2_image SDL2_ttf opengl32 glu32)

set_target_properties(Solaris PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_command(TARGET Solaris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Solaris>/assets"
)