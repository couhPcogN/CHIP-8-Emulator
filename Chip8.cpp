#include"Chip8.h"
#include<fstream>
#include<vector>
#include<cstring>

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
                uint8_t nn = opcode & 0X00FF;
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

                        //5.Kiểm tra va chạm (XOR logic)
                        // nếu pixel trên màn hình là 1 -> gặp 1 thành 0
                        if(video[videoIndex] == 1) {
                            registers[0xF] =1; // bật cờ VF
                        }

                        // thực hiện vẽ bằng phép XOR
                        video[videoIndex]^= 1; 
                    }
                }
            }
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