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

include_directories(src)

add_executable(raycaster src/main.c
        src/game/player.h
        src/game/player.c
        src/engine/graphics.c
        src/engine/graphics.h
        src/audio/audio.h
        src/audio/audio.c
        src/engine/sprite.h
        src/engine/sprite.c
        src/game/map.h
        src/game/map.c
        src/game/map_gen.h
        src/game/map_gen.c
        src/game/entity.h
        src/game/entity.c
        src/game/input.c
        src/game/input.h
        src/utils/stack.h
        src/utils/stack.c
        src/utils/queue.h
        src/utils/queue.c
        src/game/gamestate.h
        src/game/gamestate.c
)
target_link_libraries(raycaster SDL3 SDL3_image)
