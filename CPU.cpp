#include "CPU.h"

const char CPU::chars[16] = { '1', '2', '3', '4', 'Q', 'W', 'E', 'R', 'A', 'S', 'D', 'F', 'Z', 'X', 'C', 'V' };

CPU::CPU(string ROMfilePath)
{
    // todo make the screen size
    srand(time(0));

    this->instructions = new unsigned short[READ_SIZE];
    std::ifstream rfile(ROMfilePath, std::ios::binary);
    rfile.read((char*)this->instructions, 500 * sizeof(this->instructions[0]));
    rfile.close();

    int sizeNumbers = sizeof(NUMBERS_SPRITES) / sizeof(NUMBERS_SPRITES[0]);

    for (int i = 0; i < sizeNumbers; i++)
    {
        this->ram[i] = NUMBERS_SPRITES[i];
    }
}

CPU::~CPU()
{
    delete[] this->instructions;
}

void CPU::fetch()
{
    decode(this->instructions[this->PCRegister]);
    this->PCRegister += 1;
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
    return (opCode / 10) % 0x10;
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

void CPU::clearDisplay()
{
}

void CPU::ret()
{
    this->PCRegister = this->stack[this->SPRegister];
    this->SPRegister--;
}

void CPU::setPC(nnn addr)
{
    this->PCRegister = addr;
}

void CPU::call(nnn addr)
{
    this->SPRegister++;
    this->stack[this->SPRegister] = this->PCRegister;
    setPC(addr);
}

void CPU::VxEqual(x vx, kk value)
{
    if (this->Vx[vx] == value)
    {
        this->PCRegister += 1;
    }
}

void CPU::VxNotEqual(x vx, kk value)
{
    if (this->Vx[vx] != value)
    {
        this->PCRegister += 1;
    }
}

void CPU::compereVxs(x vx, x vy)
{
    if (this->Vx[vx] == this->Vx[vy])
    {
        this->PCRegister += 1;
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
    if (this->Vx[vx] + this->Vx[vy] > 255)
    {
        this->Vx[0xF] = 1;
    }
    else
    {
        this->Vx[0xF] = 0;
    }
    this->Vx[vx] += this->Vx[vy];
}

void CPU::subVxYx(x vx, x vy)
{
    if (this->Vx[vx] > this->Vx[vy])
    {
        this->Vx[0xF] = 1;
    }
    else
    {
        this->Vx[0xF] = 0;
    }
    this->Vx[vx] -= this->Vx[vy];
}

void CPU::shrVxYx(x vx, x vy)
{
    this->Vx[vx] >>= this->Vx[vy];
}

void CPU::subnVxYx(x vx, x vy)
{
    if (this->Vx[vy] > this->Vx[vx])
    {
        this->Vx[0xF] = 1;
    }
    else
    {
        this->Vx[0xF] = 0;
    }
    this->Vx[vx] = this->Vx[vy] - this->Vx[vx];
}

void CPU::shlVxYx(x vx, x vy)
{
    this->Vx[vx] <<= this->Vx[vy];
}

void CPU::VxNotEqualYx(x vx, x vy)
{
    if (this->Vx[vx] != this->Vx[vy])
    {
        this->PCRegister += 1;
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
    this->Vx[vx] = (rand() % 255) & andValue;
}

void CPU::display(x vx, x yx, x f)
{
    for (int i = 0; i < f; i++)
    {
        auto sprit = this->ram[this->IRegister + i];
        auto x = this->Vx[vx];
        auto y = this->Vx[yx];

        this->Vx[0xF] = this->screen.draw(x, y, sprit);
    }
}

void CPU::isPressed(x vx)
{
    static const char chars[] = {'1', '2', '3', '4', 'Q', 'W', 'E', 'R', 'A', 'S', 'D', 'F', 'Z', 'X', 'C', 'V'};
    if (GetKeyState(chars[vx]) & 0x8000)
    {
        this->PCRegister += 1;
    }

}

void CPU::isNotPressed(x vx)
{
    
    if (!(GetKeyState(chars[vx]) & 0x8000))
    {
        this->PCRegister += 1;
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
        Sleep(0.01);
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
    this->IRegister += this->Vx[vx];
}

void CPU::setIPointerOfVxNumber(x vx)
{
    this->IRegister = this->Vx[vx] * 5;
}

void CPU::storeBCDVx(x vx)
{
    unsigned char Digit = this->Vx[vx];
    for (int i = 3; i > 0; --i)
    {
        this->ram[this->IRegister + i - 1] = Digit % 10;
        Digit /= 10;
    }
}

void CPU::saveRegisters(x vx)
{
    for (int i = 0; i < vx; i++)
    {
        this->ram[this->IRegister + i] = this->Vx[i];
    }
}

void CPU::loadRegisters(x vx)
{
    for (int i = 0; i < vx; i++)
    {
        this->Vx[i] = this->ram[this->IRegister + i];
    }
}
