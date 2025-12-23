#include<cstdint>
#include<random>

class Chip8{
    public: 
    // Khởi tạo 
    Chip8();
    // Nạp Game (ROM)
    void loadROM(char const* filename);

    // vòng lặp xử lý 
    void cycle();

    // CÁC PHẦN CỨNG 

    //1. Thanh ghi : 16 thanh ghi 8-bit (V0->VF)
    uint8_t registers[16];

    //2. Bộ nhớ : 4KB (4096 bytes)
    uint16_t memory[4096];

    //3. Index register (I) : Trỏ địa chỉ bộ nhớ (Cần 16 bit vì RAM > 255)
    uint16_t index;

    //4. Program Counter (PC) : Trỏ tới lệnh hiện tại 
    uint16_t pc;

    //5.Stack : Lưu trữ địa chỉ khi gọi hàm ( tối đa 16 cấp)
    uint16_t stack[16];

    //6.Stack Pointer (SP) : Trỏ tới đỉnh stack
    uint8_t sp;

    //7.Timer: Đếm ngược về 0 (60 HZ)
    uint8_t delayTimer;
    uint8_t soundTimer;

    //8.Input: Trạng thái 16 phím (được ấn hay ko)
    uint8_t keypad[16];

    //9.Graphic: Màn hình 64x32 pixel (Đen trắng)
    // dùng mảng 1 chiều kích thước 64x32 = 2048
    uint32_t video[64*32];

    //opcode: Lưu lệnh hiện tại (2byte)
    uint16_t opcode;
};
