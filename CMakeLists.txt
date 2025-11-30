cmake_minimum_required(VERSION 3.30)
project(raycaster C)

set(CMAKE_C_STANDARD 11)

set(SDL3_INCLUDE_DIR "C:/libs/SDL/x86_64-w64-mingw32/include")
set(SDL3_LIB_DIR     "C:/libs/SDL/x86_64-w64-mingw32/lib")

set(SDL3_image_INCLUDE_DIR "C:/libs/SDL3_image/x86_64-w64-mingw32/include")
set(SDL3_image_LIB_DIR "C:/libs/SDL3_image/x86_64-w64-mingw32/lib")

set(SDL2_mixer_INCLUDE_DIR "C:/libs/SDL2_mixer/x86_64-w64-mingw32/include")
set(SDL2_mixer_LIB_DIR "C:/libs/SDL2_mixer/x86_64-w64-mingw32/lib")

include_directories(${SDL3_INCLUDE_DIR})
link_directories(${SDL3_LIB_DIR})

include_directories(${SDL3_image_INCLUDE_DIR})
link_directories(${SDL3_image_LIB_DIR})

add_executable(raycaster main.c
        player.h
        player.c
        graphics.c
        graphics.h
        audio.h
        audio.c
)
target_link_libraries(raycaster SDL3 SDL3_image)
