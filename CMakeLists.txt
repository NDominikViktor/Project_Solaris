cmake_minimum_required(VERSION 3.20)
project(Solaris C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
        main.c
        src/camera.c
        include/camera.h
        include/scene.h
        src/scene.c
        include/stb_image.h
)

add_executable(Solaris ${SOURCES})

target_link_libraries(Solaris mingw32 SDL2main SDL2 opengl32 glu32 SDL2_image)

# Set the working directory to the project root so assets/ is always found,
# regardless of which build folder the executable lands in.
set_target_properties(Solaris PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Copy the assets folder next to the executable after every build.
add_custom_command(TARGET Solaris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Solaris>/assets"
)