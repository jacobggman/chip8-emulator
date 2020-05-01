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

    //Test b;
    //b.ConstructConsole(64, 32, 20, 20);
    //b.Draw(1, 2);
    //b.Draw(1, 3);
    //b.Start();
    
    // make window
    //Choose any color
    
    //unsigned short value = 0xDAB6;
    //auto b = CPU::getThird(value);
    //printf("%04x\n", b);
    //return 0;
    CPU a("test_opcode.ch8", 60);

    while (true)
    {
        a.fetch();
    }
}
