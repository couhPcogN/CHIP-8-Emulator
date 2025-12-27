cmake_minimum_required(VERSION 3.10)


project(Chip8_Emulator)


set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.28.x  # 
)

FetchContent_MakeAvailable(SDL2)

add_executable(Chip8 
    main.cpp
    Chip8.cpp
    Platform.cpp
)


target_link_libraries(Chip8 PRIVATE SDL2::SDL2-static SDL2::SDL2main)


if(WIN32)
    add_custom_command(TARGET Chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:Chip8>
    )
endif()