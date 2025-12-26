#include "Chip8.h"
#include "Platform.h"
#include <iostream>
#include <chrono> // Để xử lý thời gian delay

int main(int argc, char** argv) {
    // 1. Cài đặt tham số
    int videoScale = 10; // Phóng to màn hình lên 10 lần cho dễ nhìn
    int cycleDelay = 1;  // Tốc độ giả lập (ms)

    // 2. Khởi tạo Platform và Chip8
    Platform platform("CHIP-8 Emulator", 64 * videoScale, 32 * videoScale, 64, 32);
    Chip8 chip8;

    // 3. Nạp Game (Ví dụ: file PONG)
    chip8.loadROM("D:/Phuoc/C++/CHIP-8/UFO [Lutz V, 1992].ch8"); // Nhớ đổi tên file ROM bạn có

    // 4. Các biến để tính pitch (độ rộng dòng tính bằng byte cho Texture)
    int videoPitch = sizeof(chip8.video[0]) * 64;

    // 5. Vòng lặp Game (Game Loop)
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            
            chip8.cycle();
            
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}