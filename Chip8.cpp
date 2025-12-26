#include"Chip8.h"
#include<fstream>
#include<vector>
#include<cstring>
#include<cstdlib>
#include<cstdio>

const unsigned int FONTSET_SIZE = 80;
uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    //1. PC bắt đầu từ vị trí 0x200 trong bộ nhớ
    pc = 0x200;

    //2.Reset các chỉ số về 0
    opcode = 0;
    index = 0;
    sp = 0;

    //3. Xóa màn hình, stack , thanh ghi, bộ nhớ
    // Dùng memset để set toàn bộ mảng về 0
    memset(video, 0, sizeof(video));
    memset(registers, 0, sizeof(registers));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));
    memset(keypad, 0, sizeof(keypad));
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[0x50 + i] = fontset[i];
    }
}

void Chip8::loadROM(char const* filename){
    // Mở file dưới dạng binary và đặt con trỏ ở cuối file để lấy kích thước
    std::ifstream file (filename, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        //Lấy kích thước file và cấp phát bộ đệm để chứa dữ liệu
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        //Quay lại đầu file để bắt đầu đọc 
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //Nạp nội dung ROM vào bộ nhớ của Chip-8, bắt đầu từ 0x200
        for(long i = 0; i < size; ++i){
            memory[0x200 + i] = buffer[i];
        }

        // Giải phóng bộ đệm
        delete[] buffer;
    }
}

void Chip8::cycle() {
    //1.Fetch (lấy lệnh)
    //lấy byte tại PC và byte kế tiếp, ghép lại thành opcode 16-bit
    opcode = (memory[pc] << 8 | memory[pc + 1]);

    //2.Tăng PC để chuẩn bị cho lệnh tiếp theo 
    
    pc += 2;

    //3. DECODE & EXECUTE (Giải mã và thực thi)
    //Chúng ta dùng Bitwwise AND (&) để lấy chữ số đầu tiên
    switch (opcode & 0xF000) {
        // ... (Các case 0x0, 0x1 đã có) ...

        case 0x2000: // 2NNN: Gọi hàm con (Call Subroutine)
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000: // 3XNN: Bỏ qua lệnh tiếp theo nếu Vx == NN
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = opcode & 0x00FF;
            if (registers[x] == nn) pc += 2;
        }
        break;

        case 0x4000: // 4XNN: Bỏ qua lệnh tiếp theo nếu Vx != NN
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = opcode & 0x00FF;
            if (registers[x] != nn) pc += 2;
        }
        break;

        case 0x5000: // 5XY0: Bỏ qua lệnh tiếp theo nếu Vx == Vy
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if (registers[x] == registers[y]) pc += 2;
        }
        break;
        
        // ... (Case 0x6, 0x7 đã có) ...

        case 0x8000: // Lệnh tính toán số học (Toán 8XYn)
            switch (opcode & 0x000F) {
                case 0x0: // 8XY0: Vx = Vy
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[x] = registers[y];
                }
                break;
                case 0x1: // 8XY1: Vx = Vx OR Vy
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[x] |= registers[y];
                }
                break;
                case 0x2: // 8XY2: Vx = Vx AND Vy
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[x] &= registers[y];
                }
                break;
                case 0x3: // 8XY3: Vx = Vx XOR Vy
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[x] ^= registers[y];
                }
                break;
                case 0x4: // 8XY4: Vx += Vy (Có cờ nhớ VF)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    uint16_t sum = registers[x] + registers[y];
                    registers[0xF] = (sum > 255U) ? 1 : 0;
                    registers[x] = sum & 0xFF;
                }
                break;
                case 0x5: // 8XY5: Vx -= Vy (Có cờ mượn VF)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[0xF] = (registers[x] > registers[y]) ? 1 : 0;
                    registers[x] -= registers[y];
                }
                break;
                case 0x6: // 8XY6: Vx >>= 1 (Dịch phải)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    registers[0xF] = (registers[x] & 0x1);
                    registers[x] >>= 1;
                }
                break;
                case 0x7: // 8XY7: Vx = Vy - Vx
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    registers[0xF] = (registers[y] > registers[x]) ? 1 : 0;
                    registers[x] = registers[y] - registers[x];
                }
                break;
                case 0xE: // 8XYE: Vx <<= 1 (Dịch trái)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    registers[0xF] = (registers[x] & 0x80) >> 7;
                    registers[x] <<= 1;
                }
                break;
            }
            break;

        case 0x9000: // 9XY0: Bỏ qua lệnh nếu Vx != Vy
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if (registers[x] != registers[y]) pc += 2;
        }
        break;

        case 0xA000: // ANNN: Gán Index = NNN (Quan trọng cho việc vẽ!)
            index = opcode & 0x0FFF;
            break;

        case 0xB000: // BNNN: Nhảy tới NNN + V0
            pc = (opcode & 0x0FFF) + registers[0];
            break;

        case 0xC000: // CXNN: Random (Vx = Random AND NN)
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = opcode & 0x00FF;
            registers[x] = (rand() % 256) & nn; // Cần #include <cstdlib>
        }
        break;

        // ... (Các case D, E, F đã có) ...
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: //00E0: Xóa màn hình (Clear Screen)
                    memset(video, 0, sizeof(video));
                    break;
                case 0x000E: // 00E0: Trả về từ hàm con (Return from subroutine)
                    sp--;   //giảm con trỏ stack
                    pc = stack[sp];  // lấy địa chỉ cũ và gán lại cho pc
                    break;
            }
            break;
        case 0x1000: //1NNN: Nhảy tới đia chỉ NNN (Jump)
        pc = opcode & 0x0FFF; // Lấy 3 số cuối gán vào PC
        break;

        case 0x6000: //6XNN : Gán gtri NN cho thanh ghi VX
            {
                uint8_t x = (opcode & 0x0F00) >> 8; // Lấy vị trí X
                uint8_t nn = opcode & 0x00FF;  // Lấy giá trị NN
                registers[x] = nn;
            }
            break;
        
        case 0x7000: // 7XNN: Cộng NN vào thanh ghi VX 
            {
                uint8_t x = (opcode &0x0F00) >> 8;
                uint8_t nn = opcode & 0x00FF;
                registers[x] += nn;
            }
            break;
        case 0xD000: //DXYN: Vẽ Sprite tại (Vx,Vy) cao N dòng
        {
            //1. Lấy tọa độ (X,Y) từ thanh ghi
            uint8_t x = registers[(opcode & 0x0F00) >> 8];
            uint8_t y = registers[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F; // Chiều cao N
            uint8_t pixel;

            //2.Reset cờ va chạm (VF) về 0 trước khi vẽ
            registers[0xF] = 0;

            //3. Vòng lặp qua từng dòng Y của Sprite
            for(int yline = 0; yline < height; yline++) {
                //Lấy byte dữ liệu hình ảnh từ bộ nhớ tai I + yline
                pixel = memory [index + yline];

                //4. Vòng lặp qua từng bit (X) của dòng đó (8 pixel)
                for(int xline = 0; xline < 8; xline++){
                    //Kiểm tra xem bit hiện tại của sprite có là 1 không
                    //
                    if((pixel &(0x80 >> xline)) != 0 ) {

                        //TÍnh toán vị trí thực trên màn hình 
                        //xử lý cuộn nếu vẽ quá lề
                        int xCoord = (x + xline) % 64;
                        int yCoord = (y + yline) % 32;
                        int videoIndex = xCoord + (yCoord * 64);

                        //5.Kiểm tra va chạm và cập nhật pixel ở dạng RGBA32
                        // Nếu đang có pixel (không bằng 0) thì va chạm và tắt pixel,
                        // ngược lại bật pixel với màu trắng (0xFFFFFFFF)
                        if (video[videoIndex] != 0) {
                            registers[0xF] = 1;
                            video[videoIndex] = 0; // tắt
                        } else {
                            video[videoIndex] = 0xFFFFFFFF; // bật (trắng, opaque)
                        }
                    }
                }
            }
        }
        break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: //Ex9E: skip next instruction if key VX is pressed
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t key = registers[x];
                    if(keypad[key] != 0) {
                        pc += 2; //bỏ qua lệnh tiếp theo
                    }
                }
                break;

                case 0x00A1: //ExA1 :Skip next instruction if key VX is NOT pressed
                {
                    uint8_t x= (opcode & 0x0F00) >> 8;
                    uint8_t key = registers[x];
                    if(keypad[key] == 0) {
                        pc += 2; //bỏ qua lệnh kế tiếp
                    }
                }
                break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x07: // Fx07: Gán Vx = giá trị Delay Timer
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    registers[x] = delayTimer;
                }
                break;

                case 0x0A: // Fx0A: Wait for key press (Bạn đã có cái này, giữ nguyên)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    bool keyPress = false;
                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i] != 0) {
                            registers[x] = i;
                            keyPress = true;
                        }
                    }
                    if (!keyPress) pc -= 2;
                }
                break;

                case 0x15: // Fx15: Gán Delay Timer = Vx
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    delayTimer = registers[x];
                }
                break;

                case 0x18: // Fx18: Gán Sound Timer = Vx
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    soundTimer = registers[x];
                }
                break;

                case 0x1E: // Fx1E: Cộng Vx vào Index (I += Vx)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    index += registers[x];
                }
                break;

                case 0x29: // Fx29: Set I tới vị trí sprite của ký tự trong Vx
                {
                    // Mỗi ký tự font cao 5 byte. Font bắt đầu từ địa chỉ 0x50 (trong Constructor)
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t character = registers[x];
                    index = 0x50 + (character * 5);
                }
                break;

                case 0x33: // Fx33: Lưu mã BCD (Binary-Coded Decimal)
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t value = registers[x];
                    // Hàng trăm
                    memory[index] = value / 100;
                    // Hàng chục
                    memory[index + 1] = (value / 10) % 10;
                    // Hàng đơn vị
                    memory[index + 2] = value % 10;
                }
                break;

                case 0x55: // Fx55: Lưu từ V0 đến Vx vào bộ nhớ
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    for (uint8_t i = 0; i <= x; ++i) {
                        memory[index + i] = registers[i];
                    }
                }
                break;

                case 0x65: // Fx65: Load từ bộ nhớ ra V0 đến Vx
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    for (uint8_t i = 0; i <= x; ++i) {
                        registers[i] = memory[index + i];
                    }
                }
                break;
            }
            break;
            //....

            default:
                printf("Opcode chưa được xử lý: %X\n", opcode);
    }

    // Cập nhật bộ đếm tgian (Timers)
    if(delayTimer > 0) --delayTimer;
    if(soundTimer > 0) --soundTimer;
}