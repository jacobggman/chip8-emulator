// chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// CHIP8
// NES
#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <windows.h>
#include <cmath>
#include "CPU.h"
#include "testConsole.h"

using std::vector;
using std::istream_iterator;
using std::ifstream;
using std::noskipws;


typedef unsigned short int opcode;

bool printValue(const char * command, opcode value, opcode opCode)
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

int main()
{
    Con game;
    game.ConstructConsole(64*2, 32*2, 7, 7);
    game.Start();
}

int notmain()
{
    //Test b;
    //b.ConstructConsole(64, 32, 20, 20);
    //b.Draw(1, 2);
    //b.Draw(1, 3);
    //b.Start();
    
    // make window
    opcode opCode = 0x0F00;
    printf("0X%04x secend byte is: 0X%01x", opCode, CPU::getSecend(opCode));
    //Choose any color
    
    CPU a("pong.rom");

    while (true)
    {
        a.fetch();
    }
}
