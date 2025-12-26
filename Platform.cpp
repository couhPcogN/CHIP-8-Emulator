#include "Platform.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    // 1. Khởi động SDL (Video subsystem)
    SDL_Init(SDL_INIT_VIDEO);

    // 2. Tạo cửa sổ ứng dụng
    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    // 3. Tạo bộ renderer (cọ vẽ) có hỗ trợ tăng tốc phần cứng
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // 4. Tạo Texture (tấm vải vẽ) - Đây là nơi chúng ta đẩy pixel của Chip-8 vào
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight
    );
}

Platform::~Platform() {
    // Dọn dẹp bộ nhớ khi tắt chương trình
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
    // UpdateTexture: Copy dữ liệu từ mảng video[] của Chip8 vào Texture của SDL
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    // Xóa màn hình cũ
    SDL_RenderClear(renderer);
    // Copy Texture lên màn hình
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    // Hiển thị ra cho người dùng xem
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
    bool quit = false;
    SDL_Event event;

    // Vòng lặp lấy tất cả sự kiện đang chờ (bấm phím, di chuột, tắt máy...)
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: // Nút X trên cửa sổ
                quit = true;
                break;

            case SDL_KEYDOWN: // Phím đang nhấn xuống
            {
                switch (event.key.keysym.sym) {
                    case SDLK_x: keys[0] = 1; break; // Map phím x bàn phím -> Phím 0 Chip8
                    case SDLK_1: keys[1] = 1; break;
                    case SDLK_2: keys[2] = 1; break;
                    case SDLK_3: keys[3] = 1; break;
                    case SDLK_q: keys[4] = 1; break;
                    case SDLK_w: keys[5] = 1; break;
                    case SDLK_e: keys[6] = 1; break;
                    case SDLK_a: keys[7] = 1; break;
                    case SDLK_s: keys[8] = 1; break;
                    case SDLK_d: keys[9] = 1; break;
                    case SDLK_z: keys[0xA] = 1; break;
                    case SDLK_c: keys[0xB] = 1; break;
                    case SDLK_4: keys[0xC] = 1; break;
                    case SDLK_r: keys[0xD] = 1; break;
                    case SDLK_f: keys[0xE] = 1; break;
                    case SDLK_v: keys[0xF] = 1; break;
                }
            } break;

            case SDL_KEYUP: // Phím đã nhả ra
            {
                switch (event.key.keysym.sym) {
                    case SDLK_x: keys[0] = 0; break;
                    case SDLK_1: keys[1] = 0; break;
                    case SDLK_2: keys[2] = 0; break;
                    case SDLK_3: keys[3] = 0; break;
                    case SDLK_q: keys[4] = 0; break;
                    case SDLK_w: keys[5] = 0; break;
                    case SDLK_e: keys[6] = 0; break;
                    case SDLK_a: keys[7] = 0; break;
                    case SDLK_s: keys[8] = 0; break;
                    case SDLK_d: keys[9] = 0; break;
                    case SDLK_z: keys[0xA] = 0; break;
                    case SDLK_c: keys[0xB] = 0; break;
                    case SDLK_4: keys[0xC] = 0; break;
                    case SDLK_r: keys[0xD] = 0; break;
                    case SDLK_f: keys[0xE] = 0; break;
                    case SDLK_v: keys[0xF] = 0; break;
                }
            } break;
        }
    }
    return quit;
}