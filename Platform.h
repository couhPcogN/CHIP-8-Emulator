#include <SDL.h>

class Platform {
public:
    // Khởi tạo cửa sổ với chiều rộng, chiều cao, và tỉ lệ phóng to (scale)
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    // Cập nhật màn hình từ mảng video của Chip8
    void Update(void const* buffer, int pitch);

    // Xử lý phím bấm (trả về false nếu người dùng bấm X tắt cửa sổ)
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};