#include"Platform.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    //1. Khởi động SDL (Video subsystem)
    SDL_Init(SDL_INIT_VIDEO);

    //2. Tạo cửa sổ ứng dụng 
    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    //3.Tạo bộ cọ vẽ ( render) có hôc trợ tăng tốc phần cứng 
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //4.Tạo tấm vải vẽ (texture) - nơi đẩy pixel của CHip-8 vào
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight 
    );

}