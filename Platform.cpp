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

Platform::~Platform() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
    // Copy dữ liệu từ video[] của Chip8 vào Texture của SDL
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    //Xóa màn hình cũ 
    SDL_RenderClear(renderer);
    //copy texture lên màn hình
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    //Hiển thị nội dung
    SDL_RenderPresent(renderer);
}