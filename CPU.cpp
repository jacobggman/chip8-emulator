#include "Test.h"
#include "CPU.h"
#include <fstream>
#include <iostream>
#include <string>
#include <limits>


// Define our static variables


const char CPU::chars[16] = { '1', '2', '3', '4', 'Q', 'W', 'E', 'R', 'A', 'S', 'D', 'F', 'Z', 'X', 'C', 'V' };

#pragma warning(disable : 4996) // cant find c++ way to read null chars
CPU::CPU(string ROMfilePath, int fpsLimit)
{
    this->fpsLimit = fpsLimit;
    srand(time(0));

    FILE* rom = fopen(ROMfilePath.c_str(), "rb");

    // Get file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Allocate memory to store rom
    char* rom_buffer = (char*)malloc(sizeof(char) * rom_size);

    // Copy ROM into buffer
    size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);

    // Copy buffer to memory
    for (int i = 0; i < rom_size; ++i) 
    {
        auto a = (unsigned char)rom_buffer[i];
;       this->ram[i + 512] = a;   // Load into memory starting
        printf("%d ", a);
                                                    // at 0x200 (=512)
    }
    // check why ram is not the working
    // Clean up
    fclose(rom);
    free(rom_buffer);

    int sizeNumbers = sizeof(NUMBERS_SPRITES) / sizeof(NUMBERS_SPRITES[0]);
    for (int i = 0; i < sizeNumbers; i++)
    {
        this->ram[i] = NUMBERS_SPRITES[i];
    }

    // program start at 0x200
    this->PCRegister = 0x200;
    //64x32

    this->screen = new Screen(this, false);

    if (!this->screen->ConstructConsole(SCREEN_WIDTH, SCREEN_HEIGHT, 8, 8))
    {
        throw "can't start screen";
    }
    
    Sleep(1000);
    this->screen->Start();
}

CPU::~CPU()
{
    delete this->screen;
}

void CPU::fetch()
{
    unsigned char firstPart = this->ram[this->PCRegister];
    unsigned char secendPart = this->ram[this->PCRegister + 1];

    unsigned short instruction = firstPart << 8 | secendPart;
    if (instruction == 0x3207)
    {
        printf("");  // why stuck are change?!
    }
    pi++;

    decode(instruction);
    down_counters();

    this->PCRegister += 2;
}

void CPU::decode(opcode opCode)
{
    switch ((opCode) / 0x1000)
    {
    case 0x0:
        //printf("0x is %04x", opCode);
        if (opCode == 0x00E0)
        {
            this->clearDisplay();
        }
        else if (opCode == 0x00EE)
        {
            // RET
            this->ret();
        }
        break; // 0 - 9
    case 0x1:
        this->setPC(opCode - 0x1000);
        break; // 10 - 19
    case 0x2:
        this->call(opCode - 0x2000);
        break; // 20 - 29
    case 0x3:
        this->VxEqual(this->getSecend(opCode), this->getLastTwo(opCode));
        break; // 30 - 39
    case 0x4:
        this->VxNotEqual(this->getSecend(opCode), this->getLastTwo(opCode));
        break; // 40 - 49
    case 0x5:
        this->compereVxs(this->getSecend(opCode), this->getThird(opCode));
        break; // 50 - 59
    case 0x6:
        this->setVx(this->getSecend(opCode), this->getLastTwo(opCode));
        break; // 60 - 69
    case 0x7:
        this->addVx(this->getSecend(opCode), this->getLastTwo(opCode));
        break; // 70 - 79
    case 0x8:
        (this->*arithmeticFuncs[opCode % 0x10])(this->getSecend(opCode), this->getThird(opCode));
        break; // 80 - 89
    case 0x9:
        this->VxNotEqualYx(this->getSecend(opCode), this->getThird(opCode));
        break; // 90 - 99
    case 0xA:
        this->setI(opCode - 0xA000);
        break; // A0 - A9
    case 0xB:
        this->jumpAddV0(opCode - 0xB000);
        break; // B0 - B9
    case 0xC:
        this->random(this->getSecend(opCode), this->getLastTwo(opCode));
        break; // C0 - C9
    case 0xD:
        this->display(this->getSecend(opCode), this->getThird(opCode), opCode % 0x10);
        break; // D0 - D9
    case 0xE:
        if (getLastTwo(opCode) == 0x9E)
        {
            isPressed(getSecend(opCode));
        }
        else // is A1
        {
            isNotPressed(getSecend(opCode));
        }
        break; // E0 - E9
    case 0xF:
        switch (getLastTwo(opCode))
        {
        case 0x07:
            this->setVxDelay(getSecend(opCode));
            break;
        case 0x0A:
            this->waitForKey(getSecend(opCode));
            break;
        case 0x15:
            this->setDelayVx(getSecend(opCode));
            break;
        case 0x18:
            this->setSoundVx(getSecend(opCode));
            break;
        case 0x1E:
            this->addIVx(getSecend(opCode));
            break;
        case 0x29:
            this->setIPointerOfVxNumber(getSecend(opCode));
            break;
        case 0x33:
            this->storeBCDVx(getSecend(opCode));
            break;
        case 0x55:
            this->saveRegisters(getSecend(opCode));
            break;
        case 0x65:
            this->loadRegisters(getSecend(opCode));
            break;
        default:
            break;
        }
        break; // F0 - F9
    default:
        return;
    }
}

kk CPU::getLastTwo(opcode opCode)
{
    return opCode % 0x100;
}

x CPU::getSecend(opcode opCode)
{
    return (opCode / 0x100) % 0x10;
}

x CPU::getThird(opcode opCode)
{
    return (opCode / 0x10) % 0x10;
}

bool CPU::printValue(const char* command, opcode value, opcode opCode)
{
    if (opCode < (value + 0x1000))
    {
        printf("%04x = ", opCode);
        printf(command);
        printf(" - %03x\n", opCode - value);
        return true;
    }
    return false;
}

void CPU::down_counters()
{
    // todo : make this thread

    int down = 60 / this->fpsLimit;
    if (this->delayRegister)
    {
        this->delayRegister -= down;
    }
    if (this->soundRegister)
    {
        this->soundRegister -= down;
        // todo sound
        //this->makeSound();
    }
}

void CPU::clearDisplay()
{
    this->screen->Fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 9608, BG_BLACK);
    this->screen->updateScreen();
}

void CPU::ret()
{
    this->SPRegister--;
    this->PCRegister = this->stack[this->SPRegister];
}

void CPU::setPC(nnn addr)
{
    this->PCRegister = addr;
}

void CPU::call(nnn addr)
{
    this->stack[this->SPRegister] = this->PCRegister;
    this->SPRegister++;
    setPC(addr - 2);  // less 2 because 'fetch' adding two after every command 
}

void CPU::VxEqual(x vx, kk value)
{
    if (this->Vx[vx] == value)
    {
        this->PCRegister += 2;
    }
}

void CPU::VxNotEqual(x vx, kk value)
{
    if (this->Vx[vx] != value)
    {
        this->PCRegister += 2;
    }
}

void CPU::compereVxs(x vx, x vy)
{
    if (this->Vx[vx] == this->Vx[vy])
    {
        this->PCRegister += 2;
    }
}

void CPU::setVx(x vx, kk value)
{
    this->Vx[vx] = value;
}

void CPU::addVx(x vx, kk value)
{
    this->Vx[vx] += value;
}

void CPU::SetVxYx(x vx, x vy)
{
    this->Vx[vx] = this->Vx[vy];
}

void CPU::orVxYx(x vx, x vy)
{
    this->Vx[vx] |= this->Vx[vy];
}

void CPU::andVxYx(x vx, x vy)
{
    this->Vx[vx] &= this->Vx[vy];
}

void CPU::xorVxYx(x vx, x vy)
{
    this->Vx[vx] ^= this->Vx[vy];
}

void CPU::addVxYx(x vx, x vy)
{
    this->Vx[vx] += this->Vx[vy];
    if (this->Vx[vy] > this->Vx[vx])
    {
        this->Vx[0xF] = 1;
    }
    else
    {
        this->Vx[0xF] = 0;
    }
}

void CPU::subVxYx(x vx, x vy)
{
    if (this->Vx[vy] > this->Vx[vx])
    {
        this->Vx[0xF] = 0;
    }
    else
    {
        this->Vx[0xF] = 1;
    }
    this->Vx[vx] -= this->Vx[vy];
}

void CPU::shrVxYx(x vx, x vy)
{
    Vx[0xF] = Vx[vx] & 0x1;
    Vx[vx] >>= 1;
}

void CPU::subnVxYx(x vx, x vy)
{
    if (this->Vx[vx] > this->Vx[vy])
    {
        this->Vx[0xF] = 0;
    }
    else
    {
        this->Vx[0xF] = 1;
    }
    this->Vx[vx] = this->Vx[vy] - this->Vx[vx];
}

void CPU::shlVxYx(x vx, x vy)
{
    Vx[0xF] = Vx[vx] >> 7;
    Vx[vx] <<= 1;
}

void CPU::VxNotEqualYx(x vx, x vy)
{
    if (this->Vx[vx] != this->Vx[vy])
    {
        this->PCRegister += 2;
    }
}

void CPU::setI(nnn addr)
{
    this->IRegister = addr;
}

void CPU::jumpAddV0(nnn addr)
{
    this->PCRegister = addr + this->Vx[0];
}

void CPU::random(x vx, kk andValue)
{
    this->Vx[vx] = (rand() % 256) & andValue;
}

void CPU::display(x vx, x yx, x f)
{
    this->Vx[0xF] = 0;
    for (int i = 0; i < f; i++)
    {
        auto sprit = this->ram[this->IRegister + i];
        auto x = this->Vx[vx];
        auto y = this->Vx[yx];

        /*
        for (int j = 0; j < 8; j++)
        {
            if ((sprit & (0x80 >> i)) != 0)
            {
                // check if destory
                if (this->screen->isDraw(x, y))
                {
                    this->Vx[0xF] = 1;
                    this->screen->setValue(x + j, y, 0);
                }
                else
                {
                    this->screen->setValue(x + j, y, 1);
                }
                // draw
            }
            // loop bits
        }
        */
        if (this->screen->draw(x, y + i, sprit))
            this->Vx[0xF] = 1;
    }
    this->screen->updateScreen();
}

void CPU::isPressed(x vx)
{
    static const char chars[] = {'1', '2', '3', '4', 'Q', 'W', 'E', 'R', 'A', 'S', 'D', 'F', 'Z', 'X', 'C', 'V'};
    if (GetKeyState(chars[vx]) & 0x8000)
    {
        this->PCRegister += 2;
    }

}

void CPU::isNotPressed(x vx)
{
    
    if (!(GetKeyState(chars[vx]) & 0x8000))
    {
        this->PCRegister += 2;
    }
    else
    {
        this->PCRegister = this->PCRegister;
    }
}

void CPU::setVxDelay(x vx)
{
    this->Vx[vx] = this->delayRegister;
}

void CPU::waitForKey(x vx)
{
    while (true)
    {
        for (int i = 0; i < 16; i++)
        {
            if ((GetKeyState(chars[i]) & 0x8000))
            {
                this->Vx[vx] = i;
                return;
            }
        }
        Sleep(1 / this->fpsLimit);
    }
    
}

void CPU::setDelayVx(x vx)
{
   this->delayRegister = this->Vx[vx];
}

void CPU::setSoundVx(x vx)
{
    this->soundRegister = this->Vx[vx];
}

void CPU::addIVx(x vx)
{
    if (this->IRegister + this->Vx[vx] > 0xFFF)
        this->Vx[0xF] = 1;
    else
        this->Vx[0xF] = 0;
    this->IRegister += this->Vx[vx];
}

void CPU::setIPointerOfVxNumber(x vx)
{
    this->IRegister = this->Vx[vx] * 0x5;
}

void CPU::storeBCDVx(x vx)
{
    auto a = this->Vx[vx] / 100;
    auto b = (this->Vx[vx] / 10) % 10;
    auto c = this->Vx[vx] % 10;
    this->ram[this->IRegister + 0] = a;
    this->ram[this->IRegister + 1] = b;
    this->ram[this->IRegister + 2] = c;

    //this->ram[this->IRegister + 0] = this->Vx[vx] / 100;
    //this->ram[this->IRegister + 1] = (this->Vx[vx] / 10) % 10;
    //this->ram[this->IRegister + 2] = this->Vx[vx] % 10;
}

void CPU::saveRegisters(x vx)
{
    for (int i = 0; i < vx; i++)
    {
        this->ram[this->IRegister + i] = this->Vx[i];
    }
    this->IRegister += vx + 1;
}

void CPU::loadRegisters(x vx)
{
    for (int i = 0; i < vx; i++)
    {
        this->Vx[i] = this->ram[this->IRegister + i];
    }
    this->IRegister += vx + 1;
}
