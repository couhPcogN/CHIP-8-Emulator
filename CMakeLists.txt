cmake_minimum_required(VERSION 3.10)

# Tên dự án của bạn
project(Chip8_Emulator)

# Yêu cầu chuẩn C++17 (để code hiện đại chạy mượt)
set(CMAKE_CXX_STANDARD 17)

# --- Tự động tìm và nạp SDL2 ---
# (Phần này giúp bạn không phải tải thủ công SDL2)
include(FetchContent)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.28.x  # Phiên bản ổn định
)

FetchContent_MakeAvailable(SDL2)

# --- Kết nối các file code của bạn ---
# Liệt kê tất cả các file .cpp của bạn ở đây
add_executable(Chip8 
    main.cpp
    Chip8.cpp
    Platform.cpp
)

# --- Liên kết thư viện SDL2 vào dự án ---
target_link_libraries(Chip8 PRIVATE SDL2::SDL2-static SDL2::SDL2main)

# Nếu dùng Windows, dòng này giúp copy file SDL2.dll ra thư mục chạy
if(WIN32)
    add_custom_command(TARGET Chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:Chip8>
    )
endif()